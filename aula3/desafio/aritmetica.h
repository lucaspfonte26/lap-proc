// Pinos dos LEDs (mesmos da calculadora de complemento de 2)
#define LED_BIT0 19 // LSB
#define LED_BIT1 18
#define LED_BIT2 5
#define LED_BIT3 7 // MSB / bit de sinal

void inicializarHardware() {
    pinMode(LED_BIT0, OUTPUT);
    pinMode(LED_BIT1, OUTPUT);
    pinMode(LED_BIT2, OUTPUT);
    pinMode(LED_BIT3, OUTPUT);

    // Blink de teste no boot: confirma a integridade das ligacoes
    digitalWrite(LED_BIT0, HIGH); delay(100); digitalWrite(LED_BIT0, LOW);
    digitalWrite(LED_BIT1, HIGH); delay(100); digitalWrite(LED_BIT1, LOW);
    digitalWrite(LED_BIT2, HIGH); delay(100); digitalWrite(LED_BIT2, LOW);
    digitalWrite(LED_BIT3, HIGH); delay(100); digitalWrite(LED_BIT3, LOW);
}

void atualizarLEDs(int bits) {
    digitalWrite(LED_BIT0, bits & 0x01);
    digitalWrite(LED_BIT1, (bits >> 1) & 0x01);
    digitalWrite(LED_BIT2, (bits >> 2) & 0x01);
    digitalWrite(LED_BIT3, (bits >> 3) & 0x01);
}

int parseBits(String bin) {
    return strtol(bin.c_str(), NULL, 2) & 0x0F;
}

// Negar em complemento de 1 = inverter os bits, sem somar 1 (ao contrario do comp. de 2)
int complemento1(int bits) {
    return (~bits) & 0x0F;
}

int somarComplemento1(int bitsX, int bitsY) {
    int soma = bitsX + bitsY;
    int carryOut = (soma >> 4) & 0x01;
    soma = soma & 0x0F;
    if (carryOut) {
        soma = (soma + 1) & 0x0F; // end-around carry: vai-um do MSB volta ao LSB
    }
    return soma;
}

// Converte 4 bits para decimal com sinal; 1111 e -0, tratado como 0
int interpretarComplemento1(int bits) {
    if (bits & 0x08) {
        return -((~bits) & 0x0F);
    }
    return bits;
}

// Overflow: operandos de mesmo sinal produzem resultado de sinal diferente
// (na subtracao, bitsY ja e o complemento de 1 de B)
bool detectarOverflow(int bitsX, int bitsY, int bitsRes) {
    int sinalX = (bitsX >> 3) & 0x01;
    int sinalY = (bitsY >> 3) & 0x01;
    int sinalRes = (bitsRes >> 3) & 0x01;
    return (sinalX == sinalY) && (sinalRes != sinalX);
}

String formatarBinario(int val) {
    String bin = "";
    for (int i = 3; i >= 0; i--) {
        bin += ((val >> i) & 0x01) ? "1" : "0";
    }
    return bin;
}
