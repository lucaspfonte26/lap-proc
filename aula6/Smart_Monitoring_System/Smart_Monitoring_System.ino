#include "firmware.h"
// #include "webserver.h" // Interface pendente Fialho

// Controle de tempo para a amostragem periódica da telemetria
unsigned long tempoUltimoEnvioTelemetria = 0;
const unsigned long INTERVALO_TELEMETRIA = 1000; // Frequência exata de 1Hz (1 leitura por segundo)

void setup() {
  // Inicialização do canal de comunicação Serial para rastreamento em Bancada
  Serial.begin(115200);
  Serial.println("\n[SISTEMA] Inicializando módulo de controle de hardware...");

  // Inicializa registradores, gpios e mapeamentos de interrupção externos (Foco Aluno 1)
  initFirmware();

  Serial.println("[SISTEMA] Firmware Base pronto.");
  Serial.println("[SISTEMA] Aguardando integração com módulo webserver (Aluno 2)...");
  // initWebserver(); // Chamada futura da inicialização do Wi-Fi
}

void loop() {
  // Processamento imediato das regras de transição de sinais e estados
  atualizarControleSinais();

  // Estrutura de Temporização não-bloqueante para atendimento do requisito de amostragem
  unsigned long tempoAtual = millis();
  if (tempoAtual - tempoUltimoEnvioTelemetria >= INTERVALO_TELEMETRIA) {
    tempoUltimoEnvioTelemetria = tempoAtual;

    // Captura os dados processados pelo firmware base
    int ldrValor = obterLeituraLDR();
    bool statusSOS = sistemaEmEmergencia();

    // Emissão estruturada de logs via serial para validação das métricas do pré-lab
    Serial.print(">> TELEMETRIA - LDR (ADC 12-bit): ");
    Serial.print(ldrValor);
    Serial.print(" | Alerta SOS: ");
    Serial.println(statusSOS ? "ATIVADO (Prioridade Máxima)" : "Inativo");

    // Interface de alimentação dos dados para o Webserver do Aluno 2
    // atualizarDadosServidorWeb(ldrValor, statusSOS);
  }
}