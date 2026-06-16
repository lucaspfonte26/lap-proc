#include "semaforo.h"

// Variáveis voláteis da Interrupção (ISR)
volatile bool pedestreFlag = false;
volatile unsigned long tempoUltimaInterrupcao = 0;

// Estados da Máquina de Estados do Semáforo
enum EstadoSemaforo {
  MODO_NOTURNO,
  DIA_VERDE,
  DIA_AMARELO,
  DIA_VERMELHO
};

// Variáveis de controle interno
EstadoSemaforo estadoAtual = DIA_VERDE;
unsigned long tempoMudancaEstado = 0;
unsigned long tempoUltimoPisca = 0;
bool estadoPiscaAmarelo = false;
int valorLDR = 0;
bool solicitacaoPedestreAtiva = false;

// Rotina de Serviço de Interrupção (ISR) para o botão
void IRAM_ATTR isrBotaoPedestre() {
  unsigned long tempoAtual = millis();
  if (tempoAtual - tempoUltimaInterrupcao > DEBOUNCE_DELAY) {
    pedestreFlag = true;
    tempoUltimaInterrupcao = tempoAtual;
  }
}

void initSemaforo() {
  pinMode(BUTTON_PEDESTRE_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PEDESTRE_PIN), isrBotaoPedestre, FALLING);

  analogReadResolution(12); // Garante resolução de 12-bit

  // Inicializa o LED RGB nativo apagado (R=0, G=0, B=0)
  rgbLedWrite(LED_BUILTIN, 0, 0, 0);
}

int obterValorLDR() {
  return valorLDR;
}

bool verificarSeEhNoite() {
  return (valorLDR > LDR_THRESHOLD);
}

bool verificarPedestreAtivo() {
  return solicitacaoPedestreAtiva;
}

void atualizarSemaforo() {
  unsigned long tempoAtual = millis();
  
  // Realiza a leitura constante do sensor LDR
  valorLDR = analogRead(LDR_PIN);

  // --- LÓGICA DO MODO NOTURNO ---
  if (verificarSeEhNoite()) {
    estadoAtual = MODO_NOTURNO;
    pedestreFlag = false; // Ignora e limpa qualquer pressão no botão durante a noite
    solicitacaoPedestreAtiva = false;

    // Pisca Amarelo a 1Hz (Frequência de 1Hz = ciclo completo de 1s -> 500ms ON / 500ms OFF)
    if (tempoAtual - tempoUltimoPisca >= 500) {
      estadoPiscaAmarelo = !estadoPiscaAmarelo;
      tempoUltimoPisca = tempoAtual;
    }

    if (estadoPiscaAmarelo) {
      // Cor Amarela no LED RGB (R=G, B=0)
      rgbLedWrite(LED_BUILTIN, LED_BRILHO, LED_BRILHO, 0);
    } else {
      rgbLedWrite(LED_BUILTIN, 0, 0, 0);
    }
    return; // Sai da função, ignorando a lógica diurna
  }

  // --- LÓGICA DO MODO DIURNO (MÁQUINA DE ESTADOS) ---
  
  // Se acabou de amanhecer e estava no modo noturno, volta para o Verde
  if (estadoAtual == MODO_NOTURNO) {
    estadoAtual = DIA_VERDE;
  }

  // Captura o disparo assíncrono do botão de pedestre via interrupção
  if (pedestreFlag) {
    pedestreFlag = false; // Consome o evento
    if (estadoAtual == DIA_VERDE) {
      solicitacaoPedestreAtiva = true;
      estadoAtual = DIA_AMARELO;
      tempoMudancaEstado = tempoAtual;
    }
  }

  // Gerenciamento dos estados do semáforo diurno
  switch (estadoAtual) {
    case DIA_VERDE:
      // Carros passam: LED apagado (sem cor verde, conforme solicitado)
      rgbLedWrite(LED_BUILTIN, 0, 0, 0);
      break;

    case DIA_AMARELO:
      // Transição segura: LED Amarelo (R=G, B=0)
      rgbLedWrite(LED_BUILTIN, LED_BRILHO, LED_BRILHO, 0);
      
      // Verifica se o tempo de permanência no amarelo esgotou
      if (tempoAtual - tempoMudancaEstado >= TEMPO_AMARELO) {
        estadoAtual = DIA_VERMELHO;
        tempoMudancaEstado = tempoAtual;
      }
      break;

    case DIA_VERMELHO:
      // Carros param / Pedestre atravessa: LED Vermelho ligado
      rgbLedWrite(LED_BUILTIN, LED_BRILHO, 0, 0);
      
      // Verifica se o tempo de travessia do pedestre terminou
      if (tempoAtual - tempoMudancaEstado >= TEMPO_VERMELHO) {
        solicitacaoPedestreAtiva = false; // Conclui o atendimento ao pedestre
        estadoAtual = DIA_VERDE;
      }
      break;
      
    default:
      estadoAtual = DIA_VERDE;
      break;
  }
}