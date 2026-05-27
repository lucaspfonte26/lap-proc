// Definir os pinos
#define LED_BIT0 19 
#define LED_BIT1 18 
#define LED_BIT2 5 
#define LED_BIT3 17  

void setup() {
    // Configura todos os 4 pinos como saída
    pinMode(LED_BIT0, OUTPUT);
    pinMode(LED_BIT1, OUTPUT);
    pinMode(LED_BIT2, OUTPUT);
    pinMode(LED_BIT3, OUTPUT);
}

void loop() {
    // 1. Pisca em sequência (Efeito para testar a ordem dos bits)
    digitalWrite(LED_BIT0, HIGH); delay(150); digitalWrite(LED_BIT0, LOW);
    digitalWrite(LED_BIT1, HIGH); delay(150); digitalWrite(LED_BIT1, LOW);
    digitalWrite(LED_BIT2, HIGH); delay(150); digitalWrite(LED_BIT2, LOW);
    digitalWrite(LED_BIT3, HIGH); delay(150); digitalWrite(LED_BIT3, LOW);

    delay(1000); // Espera 1 segundo antes de reiniciar o teste
}