#include <WiFi.h>
#include <WebServer.h>
#include "webserver.h"
#include "firmware.h"

WebServer server(80);

// Controle de tempo para a amostragem periódica da telemetria
unsigned long tempoUltimoEnvioTelemetria = 0;
const unsigned long INTERVALO_TELEMETRIA = 1000; // Frequência exata de 1Hz (1 leitura por segundo)

int ldrValor = 0;
bool statusSOS = false;

void setup() {
  // Inicialização do canal de comunicação Serial para rastreamento em Bancada
  Serial.begin(115200);
  Serial.println("\n[SISTEMA] Inicializando módulo de controle de hardware...");

  // Inicializa registradores, gpios e mapeamentos de interrupção externos (Foco Aluno 1)
  initFirmware();

  Serial.println("[SISTEMA] Firmware Base pronto.");

  // Setup Webserver (WiFi Hotspot AP) sem internet
    WiFi.softAP("Calculadora_ESP32_Fialho"); 
    Serial.println("Access Point Iniciado. SSID: Calculadora_ESP32_Fialho");
    Serial.print("Endereço IP: ");
    Serial.println(WiFi.softAPIP());

    // Rota raiz: Envia a interface HTML
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", HTML_INTERFACE);
    });

    server.on("/dados", HTTP_GET, []() {
      // Monta um JSON simples com os valores salvos
      String json = "{\"ldr\":" + String(ldrValor) + ", \"sos\":" + String(statusSOS ? "true" : "false") + "}";
      server.send(200, "application/json", json);
  });

    server.begin();
    Serial.println("Servidor HTTP rodando na porta 80.");
}

void loop() {
  server.handleClient();
  
  // Processamento imediato das regras de transição de sinais e estados
  atualizarControleSinais();

  // Estrutura de Temporização não-bloqueante para atendimento do requisito de amostragem
  unsigned long tempoAtual = millis();
  if (tempoAtual - tempoUltimoEnvioTelemetria >= INTERVALO_TELEMETRIA) {
    tempoUltimoEnvioTelemetria = tempoAtual;

    // Captura os dados processados pelo firmware base
    ldrValor = obterLeituraLDR();
    statusSOS = sistemaEmEmergencia();

    // Emissão estruturada de logs via serial para validação das métricas do pré-lab
    Serial.print(">> TELEMETRIA - LDR (ADC 12-bit): ");
    Serial.print(ldrValor);
    Serial.print(" | Alerta SOS: ");
    Serial.println(statusSOS ? "ATIVADO (Prioridade Máxima)" : "Inativo");
  }
}