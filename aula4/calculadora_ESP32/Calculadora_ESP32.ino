#include <WiFi.h>
#include <WebServer.h>
#include "interface.h"
#include "aritmetica.h"

WebServer server(80);

void setup() {
    Serial.begin(115200);
    inicializarHardware();

    // Setup Webserver (WiFi Hotspot AP) sem internet
    WiFi.softAP("Calculadora_ESP32_Fialho"); 
    Serial.println("Access Point Iniciado. SSID: Calculadora_ESP32_Fialho");
    Serial.print("Endereço IP: ");
    Serial.println(WiFi.softAPIP());

    // Rota raiz: Envia a interface HTML
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", HTML_INTERFACE);
    });

    // Rota da API de cálculo
    server.on("/calc", HTTP_GET, processarCalculo);

    server.begin();
    Serial.println("Servidor HTTP rodando na porta 80.");
}

void loop() {
    server.handleClient(); // Escuta requisições continuamente
}

void processarCalculo() {
    // 1. Recebe os parâmetros do Front-End
    String paramA = server.arg("a");
    String paramB = server.arg("b");
    String op = server.arg("op");

    // Identifica o tamanho do operando com base na quantidade de caracteres digitados
    int tamanhoBits = paramA.length(); 

    // 2. Transforma strings binárias em inteiros com sinal (C nativo e dinâmico)
    int valA = parseBinarioDinamico(paramA);
    int valB = parseBinarioDinamico(paramB);
    
    // 3. Processamento Matemático e HARNESS DE TEMPO (Responsabilidade do Aluno C)
    int resultado = 0;
    bool implementado = true;
    bool erroDivisao = false;
    float mediaUs = 0.0f;
    float desvioUs = 0.0f;

    if (op == "add") {
        resultado = valA + valB;
    } else if (op == "sub") {
        resultado = valA - valB;
    } else if (op == "mul") {
        resultado = multiply(valA, valB);
    } else if (op == "fat") {
        resultado = factorial(valA);
    } else if (op == "div") {
        resultado = divideSucessiva(valA, valB, erroDivisao); 
    } else {
        implementado = false;
    }

    // Tratamento de Erros Críticos antes de enviar ao usuário
    if (!implementado) {
        server.send(200, "application/json", "{\"erro\":\"operacao nao implementada\"}");
        return;
    }
    if (erroDivisao) {
        server.send(200, "application/json", "{\"erro\":\"divisao por zero\"}");
        return;
    }

    // Benchmark de todas as operações: média + desvio (vence a resolução de 1 us)
    int repeticoes = (tamanhoBits <= 4) ? 2000 : (tamanhoBits <= 8 ? 500 : 20);
    medirOperacao(op, valA, valB, repeticoes, 30, mediaUs, desvioUs);

    // 4. Detecção de Overflow Dinâmica (Limites mudam conforme o número de bits)
    int limiteMin = -(1 << (tamanhoBits - 1));
    int limiteMax = (1 << (tamanhoBits - 1)) - 1;
    bool overflow = (resultado > limiteMax || resultado < limiteMin);

    // 5. Mascaramento e Saída de Hardware
    int mascaraBits = (1 << tamanhoBits) - 1;
    int resultadoMascarado = resultado & mascaraBits;
    atualizarLEDs(resultadoMascarado & 0x0F); // Garante segurança física enviando apenas os 4 pinos mapeados

    // 6. Monta a resposta JSON incluindo a métrica de tempo
    String jsonResposta = "{";
    jsonResposta += "\"decimal\":" + String(resultado) + ",";
    jsonResposta += "\"binario\":\"" + formatarBinarioDinamico(resultadoMascarado, tamanhoBits) + "\",";
    jsonResposta += "\"overflow\":" + String(overflow ? "true" : "false") + ",";
    jsonResposta += "\"tempo_us\":" + String(mediaUs, 3) + ",";
    jsonResposta += "\"desvio_us\":" + String(desvioUs, 3); // Desvio padrão amostral do benchmark
    jsonResposta += "}";

    server.send(200, "application/json", jsonResposta);
}