
int counter = 0;

void randomColourHandler(void) {
  Serial.println(counter ++);
  Serial.print("Vcc: ");
  Serial.println(readVcc());

  for (byte count = 0;  count < 3;  count ++) pulseCube();

  fillCube(BLACK);
  for (byte z = 0;  z < CUBE_SIZE;  z++) {
    fillPlaneZ(z, RED);    delay(300);
    fillPlaneZ(z, GREEN);  delay(300);
    fillPlaneZ(z, BLUE);   delay(300);
    fillPlaneZ(z, BLACK);
  }
/*
   checkEveryLED(RED);
   checkEveryLED(GREEN);
   checkEveryLED(BLUE);
 */
    randomLED(300);
}

void pulseCube(void) {
  for (byte value = 0;  value < 128;  value ++) {
    rgbType rgb = { value, value, value };
    fillCube(rgb);
    delay(5);
  }

  for (byte value = 128;  value > 0;  value --) {
    rgbType rgb = { value, value, value };
    fillCube(rgb);
    delay(5);
  }
}

void checkEveryLED(
  struct rgbType rgb) {

  for (byte z = 0;  z < CUBE_SIZE;  z++) {
    for (byte y = 0;  y < CUBE_SIZE;  y++) {
      for (byte x = 0;  x < CUBE_SIZE;  x++) {
        setLED(x, y, z, rgb);
        delay(200);
        setLED(x, y, z, BLACK);
      }
    }
  }
}

void randomLED(
  int count) {

  for (int index = 0;  index < count;  index ++) {
    byte x = random(CUBE_SIZE);
    byte y = random(CUBE_SIZE);
    byte z = random(CUBE_SIZE);
    setLED(x, y, z, RGB(random(250) + 6, random(250) + 6, random(250) + 6));
  }
}
