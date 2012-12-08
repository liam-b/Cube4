// ToDo:
// - Include license.
// - Extract Cube4.h
// - Demonstration sequence, e.g flying mote, audio input.
// - Test current gain control.
// - Implement pattern scripting over serial line.
// - Colour gamma correction map for each LED intensity in Flash memory.
//   - See post #37 http://www.microchip.com/forums/m479196-p2.aspx
//   - See neuroelec.com/2011/04/led-brightness-to-your-eye-gamma-correction-no
// - Scriptable sequence stored in EEPROM.
// - Plugable sequence in third-party code.

// Optimization:
// - Aim for 2 to 3 Khz scan rate (per colour plane).
// - loadColourPlaneZ() for 74154 set all 4 pins at once, using Port B directly.
// - Hand-code SPI data transfer function, allowing pattern processing during data transfer.

#define SerialX Serial1  // UART1: Zigbee module

#include <PString.h>
#include <SPI.h>
#include <TimerOne.h>

static const byte CUBE_SIZE = 4;

static const byte BUFFER_SIZE = 32;
char globalBuffer[BUFFER_SIZE];  // Store dynamically constructed strings
PString globalString(globalBuffer, sizeof(globalBuffer));

// Chip: 74154.  RGB LED plane Z0 to Z3 high-side drivers.

static const byte PIN_LED_EN =  7;  // Enable is LOW
static const byte PIN_LED_A0 =  8;
static const byte PIN_LED_A1 =  9;
static const byte PIN_LED_A2 = 10;
static const byte PIN_LED_A3 = 11;

// Chip: MY9262.  RGB LED 16 bits per pixel APDM low-side drivers.

static const byte PIN_LED_LAT = 12;  // Enable is HIGH

static const byte COLOUR_RED   = 0;
static const byte COLOUR_GREEN = 1;
static const byte COLOUR_BLUE  = 2;

byte currentColour = COLOUR_RED;
byte currentPlaneZ = 0;

struct rgbType {
  byte colour[3];
};

struct rgbType led[CUBE_SIZE][CUBE_SIZE][CUBE_SIZE];  // [x][y][z].colour[]

#define RGB(red, green, blue) (struct rgbType) { red, green, blue }

#define BLACK  RGB(0x00, 0x00, 0x00)
#define RED    RGB(0xff, 0x00, 0x00)
#define GREEN  RGB(0x00, 0xff, 0x00)
#define BLUE   RGB(0x00, 0x00, 0xff)
#define PURPLE RGB(0xff, 0x00, 0xff)
#define YELLOW RGB(0xff, 0xff, 0x00)
#define ORANGE RGB(0xff, 0x3f, 0x00)
#define WHITE  RGB(0xff, 0xff, 0xff)

#define DARK_BLUE   RGB(0x00, 0x00, 0x1f)
#define DARK_PURPLE RGB(0x1f, 0x00, 0x1f)


typedef struct {
  long nextTime;
  int  frequency;
  void (*handler)(void);
} 
action_t;


action_t actions[] = {
  //  0, 10, moveDotHandler
  //  0, 10, tiltColourHandler
  
    // 0, 10, randomColourHandler
  //0, 50, tiltDotHandler,
  0, 200, cycleDotHandler,
  //0, 200, tiltTrailHandler,
};

const static byte ACTIONS = sizeof(actions) / sizeof(action_t);

byte x0 = 0, y0 = 0, z0 = 0;
byte x1 = 0, y1 = 0, z1 = 0;


/*
 * Setup starts here.
 */

void setup(void) {
  Serial.begin(38400);
  SerialX.begin(38400);

  // Assume these pins are all in consecutive order
  for (byte pin = PIN_LED_EN;  pin <= PIN_LED_LAT;  pin ++) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }

  digitalWrite(PIN_LED_EN, HIGH);  // Disable 74154

  pinMode(MOSI, OUTPUT);
  pinMode(SCK,  OUTPUT);

  // MY9262 Write Command
  // --------------------
  // 15:    Data format Mode0:        0
  // 14-13: Reserved:                 0  
  // 12:    APDM mode:                0
  // 11:    Counter reset:            0
  // 10:    Synchronization = manual: 1  The default is "auto" = 0
  //  9:    Global current HC:        1
  //  8-4:  Global current data:      01011
  //  3-2:  Reserved:                 00
  //  1:    Sleep mode = normal:      0
  //  0:    Reserved:                 0

  //my9262WriteCommand(0x0d60);  // Change synchronization to be "manual"
  my9262WriteCommand(0x0ff0);  // Highest current
  //my9262WriteCommand(0x0800);  // Lowest current

  fillCube(BLACK);
  //setLED(x0, y0, z0, RED);

  Timer1.initialize(700);
  Timer1.attachInterrupt(timer1Handler);
}

void timer1Handler(void) {
  //long timeStart = micros();

  nextColourPlaneZ();  // 656 microseconds

  //long timeEnd = micros();
  //Serial.println(timeEnd - timeStart);
}


void loop(void) {
  long timeNow = millis();

  for (byte index = 0;  index < ACTIONS;  index ++) {
    if (timeNow > actions[index].nextTime) {
      actions[index].nextTime = timeNow + actions[index].frequency;

      (* actions[index].handler)();
    }
  }
}


void fillCube(struct rgbType rgb) {
  for (byte z = 0;  z < CUBE_SIZE;  z ++) fillPlaneZ(z, rgb);
}


void fillPlaneZ(byte z, rgbType rgb) {
  for (byte y = 0;  y < CUBE_SIZE;  y++) {
    for (byte x = 0;  x < CUBE_SIZE;  x++) {
      setLED(x, y, z, rgb);
    }
  }
}


void setLED(byte x, byte y, byte z, rgbType rgb) {
  led[x][y][z].colour[COLOUR_RED]   = rgb.colour[COLOUR_RED];
  led[x][y][z].colour[COLOUR_GREEN] = rgb.colour[COLOUR_GREEN];
  led[x][y][z].colour[COLOUR_BLUE]  = rgb.colour[COLOUR_BLUE];
}

void loadColourPlaneZ(
byte colour,
byte PlaneZ) {

  SPI.begin();
  //SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);

  for (byte w = 0;  w < 15;  w ++) {
    byte value = led[w % CUBE_SIZE][w >> 2][PlaneZ].colour[colour];

    SPI.transfer(value);
    SPI.transfer(0x00);

    digitalWrite(PIN_LED_LAT, HIGH);  // MY9262 Data latch
    digitalWrite(SCK, HIGH);
    digitalWrite(SCK, LOW);
    digitalWrite(PIN_LED_LAT, LOW);
  }

  SPI.end();

  byte value = led[3][3][PlaneZ].colour[colour];

  for (byte b = 0;  b < 8;  b ++) {  // LSB first
    digitalWrite(MOSI, (value & 0x80) == 0x80);
    value <<= 1;
    digitalWrite(SCK, HIGH);
    digitalWrite(SCK, LOW);
  }

  digitalWrite(MOSI, LOW);

  for (byte b = 8;  b < 16;  b ++) {
    if (b == 14) digitalWrite(PIN_LED_LAT, HIGH);  // MY9262 Global latch
    digitalWrite(SCK, HIGH);
    digitalWrite(SCK, LOW);
  }

  digitalWrite(PIN_LED_EN, HIGH);  // Disable 74154
  digitalWrite(PIN_LED_LAT, LOW);  // MY9262 latch complete

  byte colourPlaneZSelect = colour + (PlaneZ * 3);

  // Assume these pins are all in consecutive order
  for (byte pin = PIN_LED_A0;  pin <= PIN_LED_A3;  pin ++) {
    digitalWrite(pin, colourPlaneZSelect & 1);
    colourPlaneZSelect >>= 1;
  }

  digitalWrite(PIN_LED_EN, LOW);   // Enable 74154
}


void nextColourPlaneZ(void) {
  if (++ currentColour > COLOUR_BLUE) {
    currentColour = COLOUR_RED;
    currentPlaneZ = (currentPlaneZ + 1) % CUBE_SIZE;
  }

  loadColourPlaneZ(currentColour, currentPlaneZ);
}


void my9262WriteCommand(unsigned int command) {

  digitalWrite(PIN_LED_LAT, LOW);

  for (byte b = 0;  b < 16;  b ++) {
    digitalWrite(MOSI, (command & 0x8000) == 0x8000);  // MSB first
    command <<= 1;

    if (b == 6) digitalWrite(PIN_LED_LAT, HIGH);  // MY9262 Write Command latch
    digitalWrite(SCK, HIGH);
    digitalWrite(SCK, LOW);
  }

  digitalWrite(PIN_LED_LAT, LOW);
}

