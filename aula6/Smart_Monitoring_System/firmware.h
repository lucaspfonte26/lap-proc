#ifndef FIRMWARE_CORE_H
#define FIRMWARE_CORE_H

#include <Arduino.h>

// =============================================================================
// Definições de Hardware (Mapeamento de Pinos GPIO)
// =============================================================================
#define LDR_PIN         9    // Pino ADC1_CH6 do ESP32 (Leitura analógica do LDR)
#define BUTTON_SOS_PIN  7    // Pino GPIO com suporte a interrupção externa (Botão SOS)

// =============================================================================
// Constantes de Configuração e Temporização
// =============================================================================
#define DEBOUNCE_DELAY  50    // Janela de debounce por software (50 milissegundos)
#define SOS_DURATION    3000  // Duração de retenção do Alerta SOS (3 segundos)
#define LDR_THRESHOLD   1500  // Limiar digital de baixa luminosidade (0 a 4095)

// =============================================================================
// Interface de Funções Públicas
// =============================================================================
void initFirmware();
void atualizarControleSinais();
int obterLeituraLDR();
bool sistemaEmEmergencia();

#endif // FIRMWARE_CORE_H