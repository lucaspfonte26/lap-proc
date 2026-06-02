// Mapeamento GPIO - Ajuste conforme as conexões físicas na protoboard
#define LED_BIT0 19 // Bit menos significativo (LSB)
#define LED_BIT1 18
#define LED_BIT2 5
#define LED_BIT3 7 // Bit mais significativo (Sinal/MSB para 4 bits)

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

// Direciona o resultado truncado para os pinos físicos (exibe os 4 bits menos significativos)
void atualizarLEDs(int resultado) {
    digitalWrite(LED_BIT0, resultado & 0x01);
    digitalWrite(LED_BIT1, (resultado >> 1) & 0x01);
    digitalWrite(LED_BIT2, (resultado >> 2) & 0x01);
    digitalWrite(LED_BIT3, (resultado >> 3) & 0x01);
}

// EVOLUÇÃO: Converte String binária para inteiro garantindo o sinal de forma DINÂMICA
int parseBinarioDinamico(String bin) {
    int bits = bin.length();
    int val = strtol(bin.c_str(), NULL, 2);
    
    // Cálculo do MSB dinâmico para Complemento de Dois (ex: para 8 bits, limite é 128)
    int limiteMSB = 1 << (bits - 1);
    if (val >= limiteMSB) {
        val = val - (1 << bits); 
    }
    return val;
}

int multiply(int a, int b) {
    int resultado = 0;
    int passos = (b < 0) ? -b : b;
    for (int i = 0; i < passos; i++) {
        resultado += a;
    }
    return (b < 0) ? -resultado : resultado;
}

int factorial(int n) {
    if (n < 0) return 0; // Tratamento básico para erro matemático
    if (n <= 1) return 1;
    int resultado = 1;
    for (int i = 2; i <= n; i++) {
        resultado *= i;
    }
    return resultado;
}

int divideSucessiva(int a, int b, bool &erroDivisao) {
    if (b == 0) {
        erroDivisao = true; // Sinaliza divisão por zero
        return 0;
    }
    erroDivisao = false;

    // Determina o sinal do resultado usando XOR lógico
    bool resultadoNegativo = (a < 0) ^ (b < 0);

    // Obtém os valores absolutos para a subtração sucessiva
    int absA = (a < 0) ? -a : a;
    int absB = (b < 0) ? -b : b;
    
    int quociente = 0;
    while (absA >= absB) {
        absA -= absB;
        quociente++;
    }

    return resultadoNegativo ? -quociente : quociente;
}

String formatarBinarioDinamico(int val, int bits) {
    String bin = "";
    for(int i = bits - 1; i >= 0; i--) {
        bin += ((val >> i) & 0x01) ? "1" : "0";
    }
    return bin;
}