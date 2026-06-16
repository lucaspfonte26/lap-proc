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
void IRAM_ATTR isrBotaoSOS() {
  unsigned long tempoAtual = millis();
  
  if (tempoAtual - tempoUltimaInterrupcao > DEBOUNCE_DELAY) {
    sosFlag = true;
    tempoUltimaInterrupcao = tempoAtual;
  }
}

// =============================================================================
// Implementação das Funções do Firmware
// =============================================================================

void initFirmware() {
  // Configuração da entrada digital do botão utilizando resistor pull-up interno
  pinMode(BUTTON_SOS_PIN, INPUT_PULLUP);
  
  // Vincula o pino à ISR na transição de nível ALTO para BAIXO (Borda de Descida)
  attachInterrupt(digitalPinToInterrupt(BUTTON_SOS_PIN), isrBotaoSOS, FALLING);

  // Força explicitamente a resolução nativa do hardware do ADC para 12 bits
  analogReadResolution(12);

  // Inicializa o LED RGB nativo apagado (R=0, G=0, B=0)
  rgbLedWrite(LED_BUILTIN, 0, 0, 0);
}

int obterLeituraLDR() {
  return valorLDRAtual;
}

bool sistemaEmEmergencia() {
  return modoSOSAtivo;
}

void atualizarControleSinais() {
  unsigned long tempoAtual = millis();

  // 1. Tratamento Prévio de Prioridade Máxima (SOS)
  if (sosFlag) {
    sosFlag = false;
    modoSOSAtivo = true;
    tempoInicioSOS = tempoAtual;
    
    // Transição imediata para Vermelho Fixo (Usando intensidade 64 para não ofuscar)
    rgbLedWrite(LED_BUILTIN, 64, 0, 0);
  }

  // 2. Execução Determinística da Máquina de Estados
  if (modoSOSAtivo) {
    // Mantém estado de bloqueio vermelho por 3s
    if (tempoAtual - tempoInicioSOS >= SOS_DURATION) {
      modoSOSAtivo = false;  
      rgbLedWrite(LED_BUILTIN, 0, 0, 0); // Apaga ao sair do SOS
    }
  } 
  else {
    // Monitoramento padrão (Baixa Prioridade)
    valorLDRAtual = analogRead(LDR_PIN);

    if (valorLDRAtual < LDR_THRESHOLD) {
      // Temporização para alternar LED amarelo a cada 2000ms
      if (tempoAtual - tempoUltimoPisca >= 2000) {
        estadoLedAmarelo = !estadoLedAmarelo;
        tempoUltimoPisca = tempoAtual;
      }

      if (estadoLedAmarelo) {
        // Gera cor Amarela no LED RGB built-in (R=G, B=0)
        rgbLedWrite(LED_BUILTIN, 64, 64, 0);
      } else {
        // Apaga o LED no intervalo do pisca
        rgbLedWrite(LED_BUILTIN, 0, 0, 0);
      }
    } 
    else {
      // Luminosidade Normal: Garante que o LED fique apagado
      rgbLedWrite(LED_BUILTIN, 0, 0, 0);
      estadoLedAmarelo = false;
    }
  }
}