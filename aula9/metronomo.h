#ifndef METRONOMO_H
#define METRONOMO_H

// =============================================================================
// Mapeamento de pinos GPIO (numeracao BCM)
// =============================================================================
#define PIN_LED          18   // LED de status: PWM por hardware 1 kHz (+ resistor 330 ohm)
#define PIN_SERVO        17   // Servo SG90: gpioServo, pulso 50 Hz
#define PIN_BUZZER       27   // Buzzer ativo: liga/desliga
#define PIN_BOTAO_MAIS   23   // Botao incrementa BPM
#define PIN_BOTAO_MENOS  24   // Botao decrementa BPM

// =============================================================================
// Constantes de configuracao
// =============================================================================
#define BPM_INICIAL      60      // Base "1 segundo" do enunciado
#define BPM_MIN          40
#define BPM_MAX          240
#define BPM_PASSO        5       // Incremento/decremento por clique

#define SERVO_MIN_US     1000    // 0 graus (SG90)
#define SERVO_MID_US     1500    // 90 graus
#define SERVO_MAX_US     2000    // 180 graus
#define SERVO_ESQ_US     1200    // Posicao "tique" do braco
#define SERVO_DIR_US     1800    // Posicao "taque" do braco

#define BEEP_MS          30      // Duracao do beep por batida
#define DEBOUNCE_US      200000  // Janela de debounce mecanico (200 ms em us)
#define GLITCH_US        300     // Filtro de ruido eletrico (us)
#define LED_PWM_HZ       1000    // Frequencia do PWM do LED (persistencia da visao)
#define BUZZER_HABILITADO 1      // 1 = som ligado; 0 = silencioso (LED/servo seguem)

// =============================================================================
// Interface publica
// =============================================================================
bool initMetronomo();     // Inicia pigpio, configura pinos e ISRs. false se falhar.
void passoMetronomo();    // Uma batida + espera compensada (uma iteracao do loop).
void encerrarMetronomo(); // Relaxa servo, desliga buzzer, encerra pigpio.
int  obterBPM();          // BPM atual (para log).

#endif // METRONOMO_H
