#ifndef FIRMWARE_CORE_H
#define FIRMWARE_CORE_H

#include <Arduino.h>

// =============================================================================
// Definições de Hardware (Mapeamento de Pinos GPIO)
// =============================================================================
#define LDR_PIN         34    // Pino ADC1_CH6 do ESP32 (Leitura analógica do LDR)
#define BUTTON_SOS_PIN  25    // Pino GPIO com suporte a interrupção externa (Botão SOS)
#define LED_RED_PIN     2     // Componente Vermelha do LED (Usado para SOS e Amarelo)
#define LED_GREEN_PIN   4     // Componente Verde do LED (Combinado com Vermelho = Amarelo)

// =============================================================================
// Constantes de Configuração e Temporização
// =============================================================================
#define DEBOUNCE_DELAY  50    // Janela de debounce por software (50 milissegundos)
#define SOS_DURATION    3000  // Duração de retenção do Alerta SOS (3 segundos)
#define LDR_THRESHOLD   1500  // Limiar digital de baixa luminosidade (Ajustável de 0 a 4095)

/**
 * @brief Inicializa e configura os periféricos digitais, analógicos e interrupções.
 */
void initFirmware();

/**
 * @brief Maquina de estados executada continuamente para gerenciar os alertas visuais.
 */
void atualizarControleSinais();

/**
 * @brief Realiza a leitura instantânea do registrador do conversor ADC.
 * @return Inteiro contendo o valor quantizado entre 0 e 4095.
 */
int obterLeituraLDR();

/**
 * @brief Retorna o estado atual do alarme de emergência SOS.
 * @return true se o sistema estiver retido no processamento do SOS, false caso contrário.
 */
bool sistemaEmEmergencia();

#endif