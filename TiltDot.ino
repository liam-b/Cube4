
#include "TiltDot.h"

#define REFRESH_RATE 50

#define FLAT_X       341
#define FLAT_Y       371
#define FLAT_Z       303
#define FLAT_MARGIN  10

TRAIL trails[10];

void tiltDotHandler(void) { 
  int ax = analogRead(0); int ay = analogRead(1); int az = analogRead(2);
  
  heavyDot(GREEN, ax, ay, az, &trails[0]);
  lateDot(ORANGE, 8, &trails[0], &trails[2]);
//  lightDot(RED, ax, ay, az, &trails[1]);
//  lateDot(YELLOW, 8, &trails[1], &trails[3]);
}


void tiltTrailHandler(void) {
  cycleDot(YELLOW, &trails[4]);
  
  doTrail(PURPLE, &trails[0]);
  doTrail(DARK_PURPLE, &trails[2]);
//  doTrail(BLUE, &trails[1]);
//  doTrail(DARK_BLUE, &trails[3]);
}


void cycleDotHandler(void) { 
  cycleDot(YELLOW, &trails[4]);
}


void heavyDot(rgbType colour, int ax, int ay, int az, struct TRAIL_STRUCT *trail) {  
  trail->point[0] = MAKE_POINT(limitRange(trail->point[0].x + calcStep(ax, FLAT_X, FLAT_MARGIN), 3),
                              limitRange(trail->point[0].y + calcStep(ay, FLAT_Y, FLAT_MARGIN), 3), 
                              limitRange(trail->point[0].z + calcStep(az, FLAT_Z, FLAT_MARGIN), 3));
  
  doDot(colour, trail);
}


void lightDot(rgbType colour, int ax, int ay, int az, struct TRAIL_STRUCT *trail) {    
  trail->point[0] = MAKE_POINT(limitRange(trail->point[0].x - calcStep(ax, FLAT_X, FLAT_MARGIN), 3),
                              limitRange(trail->point[0].y - calcStep(ay, FLAT_Y, FLAT_MARGIN), 3), 
                              limitRange(trail->point[0].z - calcStep(az, FLAT_Z, FLAT_MARGIN), 3));
  
  doDot(colour, trail);
}


void lateDot(rgbType colour, int lag, struct TRAIL_STRUCT *inTrail, struct TRAIL_STRUCT *outTrail) {    
  if (pointsDiffer(inTrail->point[lag], inTrail->point[0])) {
    outTrail->point[0] = inTrail->point[lag];
  }
  
  doDot(colour, outTrail);
}


void cycleDot(rgbType colour, struct TRAIL_STRUCT *trail) {
  int MIN = 0;
  int MAX = 3;
  
  int xOn = 1;
  
 if (xOn == 1 && trail->point[0].y == MIN){
  trail->point[0].x++;
  if (trail->point[0].x > MAX)
     trail->point[0].y = MIN;
     trail->point[0].y++;
 } 
     
    
     
  doDot(colour, trail);
}


void doDot(rgbType colour, struct TRAIL_STRUCT *trail) {
  // set led to leading dot
  setLEDFromPoint(trail->point[0], colour);
  
  // turn the last dot in the trail off, unless it's the leading dot
  if (pointsDiffer(trail->last, trail->point[0])) {
    setLEDFromPoint(trail->last, BLACK);
  }
  trail->last = trail->point[0];
  
  // advance the trail
  for (int i=TRAIL_DEPTH-1; i > 0; --i) {
    trail->point[i] = trail->point[i-1];
  }
}


void doTrail(rgbType rgb, struct TRAIL_STRUCT *trail) {
  // set colour of first point in the trail
  if (pointsDiffer(trail->point[1], trail->point[0])) {
    setLEDFromPoint(trail->point[1], rgb);
  }

  // fade the trail
  for (int i=TRAIL_DEPTH-1; i > 0; --i) {
    if (pointsDiffer(trail->point[i], trail->point[0])) { 
      setLEDFromPoint(trail->point[i], RGB((rgb.colour[0] >> i),  (rgb.colour[1] >> i), (rgb.colour[2] >> i)));
    }
  }
}


int calcStep(int acc, int flat, int margin) {
  if (acc > flat + margin) {
    return -1;
  } else if (acc < flat - margin) {
    return 1;
  }
  return 0;
}


int limitRange(int n, int top) {
  if (n < 0) return 0;
  if (n > top) return top;
  return n;
}


boolean pointsDiffer(struct POINT point1, struct POINT point2) {
  return point1.x != point2.x || point1.y != point2.y || point1.z != point2.z;
}


void setLEDFromPoint(struct POINT point, rgbType colour) {
  setLED(point.x, point.y, point.z, colour);
}


void printXYZ(int x, int y, int z) {
  globalString.begin();
  globalString  = x;
  globalString += " ";
  globalString += y;
  globalString += " ";
  globalString += z;
  if (Serial) Serial.println(globalBuffer);
}


