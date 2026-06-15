#include "firmware.h"

// =============================================================================
// Variáveis Voláteis (Compartilhadas com o contexto da ISR)
// =============================================================================
volatile bool sosFlag = false;
volatile unsigned long tempoUltimaInterrupcao = 0;

// =============================================================================
// Variáveis de Controle de Estado Interno
// =============================================================================
unsigned long tempoInicioSOS = 0;
unsigned long tempoUltimoPisca = 0;
bool estadoLedAmarelo = false;
bool modoSOSAtivo = false;
int valorLDRAtual = 0;

// =============================================================================
// Rotina de Serviço de Interrupção (ISR)
// =============================================================================
/**
 * @brief ISR disparada pelo evento elétrico no pino do Botão SOS.
 * Alocada na IRAM (Instrução RAM) do ESP32 para garantir latência mínima.
 */
void IRAM_ATTR isrBotaoSOS() {
  unsigned long tempoAtual = millis();
  
  // Algoritmo de Debounce por Software (Ignora repiques mecânicos dentro do intervalo)
  if (tempoAtual - tempoUltimaInterrupcao > DEBOUNCE_DELAY) {
    sosFlag = true;
    tempoUltimaInterrupcao = tempoAtual;
  }
}

// =============================================================================
// Implementação das Funções do Firmware
// =============================================================================

void initFirmware() {
  // Configuração das saídas de sinalização visual
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  digitalWrite(LED_RED_PIN, LOW);
  digitalWrite(LED_GREEN_PIN, LOW);

  // Configuração da entrada digital do botão utilizando resistor pull-up interno
  pinMode(BUTTON_SOS_PIN, INPUT_PULLUP);
  
  // Vincula o pino à ISR na transição de nível ALTO para BAIXO (Borda de Descida)
  attachInterrupt(digitalPinToInterrupt(BUTTON_SOS_PIN), isrBotaoSOS, FALLING);

  // Força explicitamente a resolução nativa do hardware do ADC para 12 bits
  analogReadResolution(12);
}

int obterLeituraLDR() {
  valorLDRAtual = analogRead(LDR_PIN);
  return valorLDRAtual;
}

bool sistemaEmEmergencia() {
  return modoSOSAtivo;
}

void atualizarControleSinais() {
  unsigned long tempoAtual = millis();

  // 1. Tratamento Prévio de Prioridade Máxima: Captura do flag assíncrono da ISR
  if (sosFlag) {
    sosFlag = false;         // Consome o evento da interrupção
    modoSOSAtivo = true;
    tempoInicioSOS = tempoAtual;
    
    // Transição imediata de Estado Visual para Vermelho Puro
    digitalWrite(LED_RED_PIN, HIGH);
    digitalWrite(LED_GREEN_PIN, LOW);
  }

  // 2. Execução Determinística da Máquina de Estados
  if (modoSOSAtivo) {
    // Estado de Bloqueio SOS por Tempo (Garante a permanência em vermelho por 3s)
    if (tempoAtual - tempoInicioSOS >= SOS_DURATION) {
      modoSOSAtivo = false;  // Libera a prioridade do sistema
      digitalWrite(LED_RED_PIN, LOW);
      digitalWrite(LED_GREEN_PIN, LOW);
    }
  } 
  else {
    // Estado de Monitoramento Padrão de Luminosidade (Baixa Prioridade)
    obterLeituraLDR();

    // Se a leitura analógica quantizada estiver abaixo do limiar operacional
    if (valorLDRAtual < LDR_THRESHOLD) {
      // Temporização não-bloqueante para alternância do estado do LED a cada 2000ms
      if (tempoAtual - tempoUltimoPisca >= 2000) {
        estadoLedAmarelo = !estadoLedAmarelo;
        tempoUltimoPisca = tempoAtual;
      }

      if (estadoLedAmarelo) {
        // Ativação combinada para gerar a cor Amarela
        digitalWrite(LED_RED_PIN, HIGH);
        digitalWrite(LED_GREEN_PIN, HIGH);
      } else {
        digitalWrite(LED_RED_PIN, LOW);
        digitalWrite(LED_GREEN_PIN, LOW);
      }
    } 
    else {
      // Condição de Luminosidade Adequada (LEDs Apagados)
      digitalWrite(LED_RED_PIN, LOW);
      digitalWrite(LED_GREEN_PIN, LOW);
      estadoLedAmarelo = false;
    }
  }
}