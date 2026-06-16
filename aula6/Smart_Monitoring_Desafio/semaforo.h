#ifndef SEMAFORO_H
#define SEMAFORO_H

#include <Arduino.h>

// =============================================================================
// Definições de Hardware (Mapeamento de Pinos GPIO)
// =============================================================================
#define LDR_PIN             34    // Leitura analógica do LDR
#define BUTTON_PEDESTRE_PIN 25    // Botão de Pedestres (Antigo SOS)
#define LED_RED_PIN         2     // Componente Vermelha do LED
#define LED_GREEN_PIN       4     // Componente Verde do LED

// =============================================================================
// Constantes de Configuração e Temporização
// =============================================================================
#define DEBOUNCE_DELAY      50    // Janela de debounce (ms)
#define LDR_THRESHOLD       1500  // Limiar de transição Dia/Noite (0 a 4095)
#define TEMPO_AMARELO       2000  // Duração do sinal Amarelo no Dia (2 segundos)
#define TEMPO_VERMELHO      5000  // Duração do sinal Vermelho para pedestre (5 segundos)

// =============================================================================
// Funções de Interface do Módulo
// =============================================================================
void initSemaforo();
void atualizarSemaforo();
int obterValorLDR();
bool verificarSeEhNoite();
bool verificarPedestreAtivo();

#endif