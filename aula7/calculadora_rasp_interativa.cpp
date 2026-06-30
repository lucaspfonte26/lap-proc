#include <iostream>
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>
#include <iomanip>
#include <time.h>

// Função auxiliar para pegar o tempo do sistema em microssegundos (us)
int64_t obter_tempo_us() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t)ts.tv_sec * 1000000LL + (int64_t)ts.tv_nsec / 1000LL;
}

// ---- Lógica Aritmética Portada do ESP32 ----

int multiply(int a, int b) {
    int resultado = 0;
    int passos = (b < 0) ? -b : b;
    for (int i = 0; i < passos; i++) {
        resultado += a;
        asm volatile("" : "+r"(resultado)); // Impede final-value replacement
    }
    return (b < 0) ? -resultado : resultado;
}

int factorial(int n, bool &overflow) {
    overflow = false;
    if (n < 0) { overflow = true; return 0; }
    if (n <= 1) return 1;
    long long resultado = 1;
    for (int i = 2; i <= n; i++) {
        resultado *= i;
        if (resultado > INT32_MAX || resultado < INT32_MIN) overflow = true;
        resultado = (int32_t)resultado; // Wraparound de 32 bits
    }
    return (int)resultado;
}

int factorial(int n) { 
    bool descartado; 
    return factorial(n, descartado); 
}

int divideSucessiva(int a, int b, bool &erroDivisao) {
    if (b == 0) {
        erroDivisao = true;
        return 0;
    }
    erroDivisao = false;

    bool resultadoNegativo = (a < 0) ^ (b < 0);
    int absA = (a < 0) ? -a : a;
    int absB = (b < 0) ? -b : b;
    
    int quociente = 0;
    while (absA >= absB) {
        absA -= absB;
        quociente++;
        asm volatile("" : "+r"(absA), "+r"(quociente)); // Impede strength reduction
    }

    return resultadoNegativo ? -quociente : quociente;
}

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

// ---- Motor de Benchmark com Média e Desvio Padrão ----

void medirOperacao(std::string op, int a, int b, int repeticoes, int amostras,
                   float &mediaUs, float &desvioUs) {
    static volatile int sink; 
    int opcode = 0;
    if (op == "add") opcode = 1;
    else if (op == "sub") opcode = 2;
    else if (op == "mul") opcode = 3;
    else if (op == "fat") opcode = 4;
    else if (op == "div") opcode = 5;

    if (amostras > 60) amostras = 60;
    std::vector<float> medias(amostras);
    float soma = 0.0f;

    for (int s = 0; s < amostras; s++) {
        int64_t ini = obter_tempo_us();
        for (int r = 0; r < repeticoes; r++) {
            sink = executarOp(opcode, a, b);
        }
        int64_t fim = obter_tempo_us();
        
        // Tempo médio desta amostra específica por repetição
        medias[s] = (float)(fim - ini) / repeticoes;
        soma += medias[s];
    }

    mediaUs = soma / amostras;

    float somaQuad = 0.0f;
    for (int s = 0; s < amostras; s++) {
        float d = medias[s] - mediaUs;
        somaQuad += d * d;
    }
    desvioUs = (amostras > 1) ? std::sqrt(somaQuad / (amostras - 1)) : 0.0f;
}

int main() {
    std::cout << std::fixed << std::setprecision(5);
    std::cout << "====================================================\n";
    std::cout << "   BENCHMARK DA CALCULADORA BINARIA - RASPBERRY PI  \n";
    std::cout << "====================================================\n\n";

    // Configurações do teste
    int repeticoes = 10000;
    int amostras = 30;
    float media, desvio;
    
    std::string op;
    int a, b;

    // Loop interativo de entrada do usuário
    while (true) {
        std::cout << "\nDigite a operacao (add, sub, mul, fat, div) ou 'sair' para encerrar: ";
        std::cin >> op;

        if (op == "sair") {
            std::cout << "Encerrando o programa...\n";
            break;
        }

        if (op != "add" && op != "sub" && op != "mul" && op != "fat" && op != "div") {
            std::cout << "Operacao invalida. Tente novamente.\n";
            continue;
        }

        std::cout << "Digite o primeiro valor (A): ";
        std::cin >> a;

        if (op == "fat") {
            b = 0; // O fatorial ignora o segundo operando
        } else {
            std::cout << "Digite o segundo valor (B): ";
            std::cin >> b;
        }

        // Tratamento da divisão por zero antes de iniciar o benchmark para evitar loops infinitos
        if (op == "div" && b == 0) {
            std::cout << "Erro: Divisao por zero detectada. Operacao cancelada.\n";
            continue;
        }

        // Converter a string da operação em opcode para obter o resultado real
        int opcode = 0;
        if (op == "add") opcode = 1;
        else if (op == "sub") opcode = 2;
        else if (op == "mul") opcode = 3;
        else if (op == "fat") opcode = 4;
        else if (op == "div") opcode = 5;

        int resultado_matematico = executarOp(opcode, a, b);
        
        std::cout << "\n-> Resultado da operacao: " << resultado_matematico << "\n";
        std::cout << "-> Executando benchmark (" << amostras << " amostras, " << repeticoes << " repeticoes)...\n";
        
        // Chamada da função de benchmark
        medirOperacao(op, a, b, repeticoes, amostras, media, desvio);
        
        std::cout << "[" << op << "] Media de tempo: " << media << " us | Desvio Padrao: " << desvio << " us\n";
        std::cout << "----------------------------------------------------\n";
    }

    return 0;
}