// Mapeamento GPIO - Ajuste conforme as conexões físicas na protoboard
#define LED_BIT0 19 // Bit menos significativo (LSB)
#define LED_BIT1 18
#define LED_BIT2 5
#define LED_BIT3 7 // Bit mais significativo (Sinal/MSB)

void inicializarHardware() {
    pinMode(LED_BIT0, OUTPUT);
    pinMode(LED_BIT1, OUTPUT);
    pinMode(LED_BIT2, OUTPUT);
    pinMode(LED_BIT3, OUTPUT);
    
    // Teste inicial (Blink rápido) para confirmar inicialização do sistema
    digitalWrite(LED_BIT0, HIGH); delay(1000); digitalWrite(LED_BIT0, LOW);
    digitalWrite(LED_BIT1, HIGH); delay(1000); digitalWrite(LED_BIT1, LOW);
    digitalWrite(LED_BIT2, HIGH); delay(1000); digitalWrite(LED_BIT2, LOW);
    digitalWrite(LED_BIT3, HIGH); delay(1000); digitalWrite(LED_BIT3, LOW);
}

// Direciona o resultado truncado para os pinos físicos
void atualizarLEDs(int resultado) {
    digitalWrite(LED_BIT0, resultado & 0x01);
    digitalWrite(LED_BIT1, (resultado >> 1) & 0x01);
    digitalWrite(LED_BIT2, (resultado >> 2) & 0x01);
    digitalWrite(LED_BIT3, (resultado >> 3) & 0x01);
}

// Converte String binária para inteiro garantindo o sinal (Complemento de Dois de 4 bits)
int parse4Bit(String bin) {
    int val = strtol(bin.c_str(), NULL, 2);
    // Se o bit mais significativo for 1 (valor unsigned >= 8), o número é negativo
    if (val >= 8) {
        val = val - 16; 
    }
    return val;
}

// Converte um inteiro para String binária de 4 bits exatos
String formatarBinario(int val) {
    String bin = "";
    for(int i = 3; i >= 0; i--) {
        bin += ((val >> i) & 0x01) ? "1" : "0";
    }
    return bin;
}