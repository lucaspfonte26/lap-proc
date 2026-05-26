const unsigned long T_GREEN  = 3000; 
const unsigned long T_YELLOW = 1000; 
const unsigned long T_RED    = 4000; 

void setup() {
}

void loop() {
  rgbLedWrite(LED_BUILTIN, 0, 64, 0); 
  delay(T_GREEN);
  rgbLedWrite(LED_BUILTIN, 64, 64, 0); 
  delay(T_YELLOW);
  rgbLedWrite(LED_BUILTIN, 64, 0, 0);
  delay(T_RED);
}