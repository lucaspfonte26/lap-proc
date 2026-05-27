#include <WiFi.h>
#include <WebServer.h>
#include "interface.h"
#include "aritmetica.h"

WebServer server(80);

void setup() {
    Serial.begin(115200);
    inicializarHardware();

    // Setup Webserver (WiFi Hotspot AP) sem internet
    WiFi.softAP("Calculadora_ESP32"); 
    Serial.println("Access Point Iniciado. SSID: Calculadora_ESP32");
    Serial.print("Endereço IP: ");
    Serial.println(WiFi.softAPIP());

    // Rota raiz: Envia a interface HTML
    server.on("/", []() {
        server.send(200, "text/html", HTML_INTERFACE);
    });

    // Rota da API de cálculo
    server.on("/calc", processarCalculo);

    server.begin();
    Serial.println("Servidor HTTP rodando.");
}

void loop() {
    server.handleClient(); // Escuta requisições continuamente
}

void processarCalculo() {
    // 1. Recebe os parâmetros do Front-End
    String paramA = server.arg("a");
    String paramB = server.arg("b");
    String op = server.arg("op");

    // 2. Transforma strings binárias em inteiros reais (C nativo)
    int valA = parse4Bit(paramA);
    int valB = parse4Bit(paramB);
    
    // 3. Processamento Matemático
    int resultado = (op == "add") ? (valA + valB) : (valA - valB); //
    
    // 4. Detecção de Overflow
    // Em complemento de dois de 4 bits, o intervalo válido é -8 a +7
    bool overflow = (resultado > 7 || resultado < -8); 

    // 5. Mascaramento e Saída de Hardware
    int resultadoMascarado = resultado & 0x0F; //
    atualizarLEDs(resultadoMascarado);

    // 6. Monta a resposta JSON para devolver ao navegador
    String jsonResposta = "{";
    jsonResposta += "\"decimal\":" + String(resultado) + ",";
    jsonResposta += "\"binario\":\"" + formatarBinario(resultadoMascarado) + "\",";
    jsonResposta += "\"overflow\":" + String(overflow ? "true" : "false");
    jsonResposta += "}";

    server.send(200, "application/json", jsonResposta);
}