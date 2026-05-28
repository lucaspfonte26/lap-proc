#include "aritmetica.h"

void setup() {
    Serial.begin(115200);
    delay(1000); // aguarda o Serial Monitor conectar
    inicializarHardware();

    Serial.println();
    Serial.println("==================================================");
    Serial.println(" Calculadora 4 bits - COMPLEMENTO DE UM (via Serial)");
    Serial.println(" Faixa representavel: -7 a +7");
    Serial.println("==================================================");
}

void loop() {
    String a = lerEntrada("Operando A (4 bits, ex: 0110): ");
    if (!validar4Bits(a)) {
        Serial.println("[ERRO] Entrada invalida! Use exatamente 4 bits (0/1).\n");
        return;
    }

    String b = lerEntrada("Operando B (4 bits, ex: 0010): ");
    if (!validar4Bits(b)) {
        Serial.println("[ERRO] Entrada invalida! Use exatamente 4 bits (0/1).\n");
        return;
    }

    String op = lerEntrada("Operacao (+ ou -): ");
    if (op != "+" && op != "-") {
        Serial.println("[ERRO] Operacao invalida! Use + ou -.\n");
        return;
    }

    processarCalculo(a, b, op);
}

void processarCalculo(String a, String b, String op) {
    int bitsA = parseBits(a);
    int bitsB = parseBits(b);

    // Subtracao em complemento de 1: A - B = A + (~B)
    int bitsY = (op == "+") ? bitsB : complemento1(bitsB);

    int bitsRes = somarComplemento1(bitsA, bitsY);
    bool overflow = detectarOverflow(bitsA, bitsY, bitsRes);

    atualizarLEDs(bitsRes);

    Serial.println("--------------------------------------------------");
    Serial.print("A = ");   Serial.print(a); Serial.print("  ("); Serial.print(interpretarComplemento1(bitsA)); Serial.println(")");
    Serial.print("B = ");   Serial.print(b); Serial.print("  ("); Serial.print(interpretarComplemento1(bitsB)); Serial.println(")");
    Serial.print("Operacao: "); Serial.println(op == "+" ? "soma" : "subtracao");
    Serial.print("Resultado (bits): "); Serial.println(formatarBinario(bitsRes));

    if (overflow) {
        Serial.println(">>> OVERFLOW! Resultado fora da faixa -7 a +7 <<<");
    } else {
        Serial.print("Resultado (decimal): ");
        Serial.println(interpretarComplemento1(bitsRes));
    }
    Serial.println("--------------------------------------------------");
    Serial.println();
}

String lerEntrada(const char* prompt) {
    Serial.print(prompt);
    while (Serial.available() == 0) {
        delay(10);
    }
    String entrada = Serial.readString();
    entrada.trim();
    Serial.println(entrada); // eco no Serial Monitor do que foi digitado
    return entrada;
}

bool validar4Bits(String s) {
    if (s.length() != 4) return false;
    for (int i = 0; i < 4; i++) {
        if (s[i] != '0' && s[i] != '1') return false;
    }
    return true;
}
