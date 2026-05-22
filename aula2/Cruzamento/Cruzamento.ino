const uint8_t PIN_RED    = 2;
const uint8_t PIN_YELLOW = 3;
const uint8_t PIN_GREEN  = 4;

const unsigned long T_GREEN  = 3000;
const unsigned long T_YELLOW = 1000;
const unsigned long T_RED    = 4000;

void setup() {
  pinMode(PIN_RED,    OUTPUT);
  pinMode(PIN_YELLOW, OUTPUT);
  pinMode(PIN_GREEN,  OUTPUT);
}

void loop() {
  digitalWrite(PIN_GREEN, HIGH);
  delay(T_GREEN);
  digitalWrite(PIN_GREEN, LOW);

  digitalWrite(PIN_YELLOW, HIGH);
  delay(T_YELLOW);
  digitalWrite(PIN_YELLOW, LOW);

  digitalWrite(PIN_RED, HIGH);
  delay(T_RED);
  digitalWrite(PIN_RED, LOW);
}
