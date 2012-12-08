void moveDotHandler(void) {
  if (Serial.available()) {
    byte ch = Serial.read();

    if (ch == 'd') x1 = (byte) (x0 - 1) % CUBE_SIZE;
    if (ch == 'a') x1 = (byte) (x0 + 1) % CUBE_SIZE;
    if (ch == 'w') y1 = (byte) (y0 - 1) % CUBE_SIZE;
    if (ch == 's') y1 = (byte) (y0 + 1) % CUBE_SIZE;
    if (ch == 'l') z1 = (byte) (z0 - 1) % CUBE_SIZE;
    if (ch == 'o') z1 = (byte) (z0 + 1) % CUBE_SIZE;

    if (x0 != x1  ||  y0 != y1  ||  z0 != z1) {
      setLED(x0, y0, z0, BLACK);
      x0 = x1;  y0 = y1;  z0 = z1;
      setLED(x0, y0, z0, BLUE);
    }
  }
 
  globalString.begin();
  globalString  = analogRead(0);
  globalString += " ";
  globalString += analogRead(1);
  globalString += " ";
  globalString += analogRead(2);
  
  if (Serial) Serial.println(globalBuffer);
 
}
