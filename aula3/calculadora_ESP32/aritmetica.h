// Arquivo: aritmetica.h

// Mapeamento GPIO (Ajustem conforme as conexões físicas na protoboard)
#define LED_BIT0 19 // Bit menos significativo (LSB)
#define LED_BIT1 18
#define LED_BIT2 5
#define LED_BIT3 17 // Bit mais significativo (Sinal/MSB)

void inicializarHardware() {
    pinMode(LED_BIT0, OUTPUT);
    pinMode(LED_BIT1, OUTPUT);
    pinMode(LED_BIT2, OUTPUT);
    pinMode(LED_BIT3, OUTPUT);
    
    // Teste inicial (Blink) para garantir integridade
    digitalWrite(LED_BIT0, HIGH); delay(200); digitalWrite(LED_BIT0, LOW);
}

// Função para enviar o pulso elétrico aos pinos
void atualizarLEDs(int resultado) {
    digitalWrite(LED_BIT0, resultado & 0x01); //
    digitalWrite(LED_BIT1, (resultado >> 1) & 0x01);
    digitalWrite(LED_BIT2, (resultado >> 2) & 0x01);
    digitalWrite(LED_BIT3, (resultado >> 3) & 0x01);
}

// Converte a string (ex: "1110") para um inteiro com sinal correto no comp. de dois
int parse4Bit(String bin) {
    int val = strtol(bin.c_str(), NULL, 2); //
    // Se o bit mais significativo for 1 (valores >= 8), é negativo.
    if (val >= 8) {
        val = val - 16; 
    }
    return val;
}

// Retorna uma string binária formatada em 4 bits para enviar ao front-end
String formatarBinario(int val) {
    String bin = "";
    for(int i = 3; i >= 0; i--) {
        bin += ((val >> i) & 0x01) ? "1" : "0";
    }
    return bin;
}