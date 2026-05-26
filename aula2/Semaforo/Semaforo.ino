const unsigned long T = 10000;  // ms

void setup() {
}

void loop() {
  rgbLedWrite(LED_BUILTIN, 64, 64, 0);  // ESTADO 01: amarelo aceso
  delay(T);
  rgbLedWrite(LED_BUILTIN, 0, 0, 0);    // ESTADO 02: apagado
  delay(T);
}