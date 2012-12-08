#define TRAIL_DEPTH  16

#define MIN(x,y) (x) > (y) ? (x) : (y)

typedef struct POINT { 
  byte x;
  byte y;
  byte z; 
} POINT;

#define MAKE_POINT(x, y, z) (POINT) { x, y, z }

typedef struct TRAIL_STRUCT { 
  POINT point[TRAIL_DEPTH];
  POINT last; 
} TRAIL;


void cycleDotHandler(void);

void tiltDotHandler(void);

void heavyDot(rgbType colour, int ax, int ay, int az, struct TRAIL_STRUCT *trail);

void lightDot(rgbType colour, int ax, int ay, int az, struct TRAIL_STRUCT *trail);

void lateDot(rgbType colour, int lag, struct TRAIL_STRUCT *inTrail, struct TRAIL_STRUCT *outTrail);

void cycleDot(rgbType colour, struct TRAIL_STRUCT *trail);

void doDot(rgbType colour, struct TRAIL_STRUCT *trail);


void tiltTrailHandler(void);

void doTrail(rgbType colour, struct TRAIL_STRUCT *trail);


