const uint8_t PIN_YELLOW = 3;
const unsigned long T = 10000  // ms

void setup() {
  pinMode(PIN_YELLOW, OUTPUT);
}

void loop() {
  digitalWrite(PIN_YELLOW, HIGH);  // ESTADO 01
  delay(T);
  digitalWrite(PIN_YELLOW, LOW);   // ESTADO 02
  delay(T);
}
