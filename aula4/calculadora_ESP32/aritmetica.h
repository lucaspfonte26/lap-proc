#include <math.h>

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
    digitalWrite(LED_BIT0, HIGH); delay(100); digitalWrite(LED_BIT0, LOW);
    digitalWrite(LED_BIT1, HIGH); delay(100); digitalWrite(LED_BIT1, LOW);
    digitalWrite(LED_BIT2, HIGH); delay(100); digitalWrite(LED_BIT2, LOW);
    digitalWrite(LED_BIT3, HIGH); delay(100); digitalWrite(LED_BIT3, LOW);
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

// Multiplicação por somas repetidas (Mantida do Aluno A)
int multiply(int a, int b) {
    int resultado = 0;
    int passos = (b < 0) ? -b : b;
    for (int i = 0; i < passos; i++) {
        resultado += a;
    }
    return (b < 0) ? -resultado : resultado;
}

// IMPLEMENTAÇÃO DO ALUNO B: Fatorial em C (Iterativo)
int factorial(int n) {
    if (n < 0) return 0; // Tratamento básico para erro matemático
    if (n <= 1) return 1;
    int resultado = 1;
    for (int i = 2; i <= n; i++) {
        resultado *= i;
    }
    return resultado;
}

// DESAFIO DO ALUNO C: Divisão por Subtrações Sucessivas com tratamento de Erro
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

// Dispatch barato por código numérico (evita comparar String dentro do laço)
int executarOp(int opcode, int a, int b) {
    switch (opcode) {
        case 1: return a + b;
        case 2: return a - b;
        case 3: return multiply(a, b);
        case 4: return factorial(a);
        case 5: { bool e; return divideSucessiva(a, b, e); }
    }
    return 0;
}

// Mede qualquer operação em lotes: repeticoes por lote vence a resolução de 1 us;
// amostras geram média e desvio padrão amostral (sqrtf).
void medirOperacao(String op, int a, int b, int repeticoes, int amostras,
                   float &mediaUs, float &desvioUs) {
    static volatile int sink; // impede o compilador de descartar o cálculo
    int opcode = 0;
    if (op == "add") opcode = 1;
    else if (op == "sub") opcode = 2;
    else if (op == "mul") opcode = 3;
    else if (op == "fat") opcode = 4;
    else if (op == "div") opcode = 5;

    if (amostras > 60) amostras = 60;
    float medias[60];
    float soma = 0.0f;

    for (int s = 0; s < amostras; s++) {
        int64_t ini = esp_timer_get_time();
        for (int r = 0; r < repeticoes; r++) {
            sink = executarOp(opcode, a, b);
        }
        int64_t fim = esp_timer_get_time();
        medias[s] = (float)(fim - ini) / repeticoes;
        soma += medias[s];
    }

    mediaUs = soma / amostras;

    float somaQuad = 0.0f;
    for (int s = 0; s < amostras; s++) {
        float d = medias[s] - mediaUs;
        somaQuad += d * d;
    }
    desvioUs = (amostras > 1) ? sqrtf(somaQuad / (amostras - 1)) : 0.0f;
}

// EVOLUÇÃO: Converte um inteiro para String binária com tamanho de bits configurável
String formatarBinarioDinamico(int val, int bits) {
    String bin = "";
    for(int i = bits - 1; i >= 0; i--) {
        bin += ((val >> i) & 0x01) ? "1" : "0";
    }
    return bin;
}