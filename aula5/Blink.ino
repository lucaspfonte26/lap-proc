// Bancada 1: teste cego do LED externo (sem PWM/web)
// GPIO7 + resistor 220-330 ohm -> anodo(longo) | catodo(curto) -> GND
const int LED_PIN = 3;

void setup() {
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_PIN, LOW);
    delay(1000);
}
