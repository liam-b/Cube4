

void tiltColourHandler(void) {
  int ax = analogRead(0);
  int ay = analogRead(1);
  int az = analogRead(2);

//Serial.println(ax);
//Serial.println(ay);
//Serial.println(az);

  int red   = acceleration2colour(ax);
  int green = acceleration2colour(ay);
  int blue  = acceleration2colour(az);

  fillCube(RGB(red, green, blue));
}

int acceleration2colour(int acceleration) {
  int colour = (acceleration - 200.0) * 255.0 / 300.0;
  if (colour < 0) colour = 0;
  if (colour > 255) colour = 255;
  return(colour);
}
