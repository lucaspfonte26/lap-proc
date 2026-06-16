#include <WiFi.h>
#include <WebServer.h>
#include "webserver.h"
#include "semaforo.h"

WebServer server(80);

// Controle de tempo para a amostragem periódica da telemetria (Requisito 1Hz)
unsigned long tempoUltimoEnvioTelemetria = 0;
const unsigned long INTERVALO_TELEMETRIA = 1000; 

int ldrValor = 0;
bool noiteAtiva = false;
bool pedestreAtivo = false;

void setup() {
  Serial.begin(115200);
  Serial.println("\n[SISTEMA] Inicializando Semáforo Inteligente...");

  // Inicializa periféricos e interrupções mapeados no módulo de controle
  initSemaforo();

  Serial.println("[SISTEMA] Configurações de Hardware prontas.");

  // Inicialização do Access Point do ESP32
  WiFi.softAP("Semaforo_Inteligente_Fialho"); 
  Serial.println("Access Point Iniciado. SSID: Semaforo_Inteligente_Fialho");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.softAPIP());

  // Rota raiz: Envia a interface HTML
  server.on("/", HTTP_GET, []() {
      server.send(200, "text/html", HTML_INTERFACE);
  });

  // Rota de dados: Envia as variáveis mapeadas estruturadas em JSON
  server.on("/dados", HTTP_GET, []() {
    String json = "{\"ldr\":" + String(ldrValor) + 
                  ",\"noite\":" + String(noiteAtiva ? "true" : "false") + 
                  ",\"pedestre\":" + String(pedestreAtivo ? "true" : "false") + "}";
    server.send(200, "application/json", json);
  });

  server.begin();
  Serial.println("Servidor HTTP rodando na porta 80.");
}

void loop() {
  server.handleClient();
  
  // Atualização em tempo real da máquina de estados do semáforo
  atualizarSemaforo();

  // Envio periódico de logs via serial (Amostragem de 1Hz)
  unsigned long tempoAtual = millis();
  if (tempoAtual - tempoUltimoEnvioTelemetria >= INTERVALO_TELEMETRIA) {
    tempoUltimoEnvioTelemetria = tempoAtual;

    // Coleta dados consolidados do módulo de controle
    ldrValor = obterValorLDR();
    noiteAtiva = verificarSeEhNoite();
    pedestreAtivo = verificarPedestreAtivo();

    // Impressão estruturada de debug no Serial Monitor
    Serial.print(">> MONITORAMENTO - LDR: ");
    Serial.print(ldrValor);
    Serial.print(" | Período: ");
    Serial.print(noiteAtiva ? "NOTURNO (Alerta)" : "DIURNO (Normal)");
    Serial.print(" | Solicitação Pedestre: ");
    Serial.println(pedestreAtivo ? "EM ATENDIMENTO" : "Nenhuma");
  }
}