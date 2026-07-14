#ifndef METRONOMO_H
#define METRONOMO_H

// =============================================================================
// Mapeamento de pinos GPIO (numeracao BCM)
// =============================================================================
// Pinos fixos da Freenove Projects Board (componentes ja soldados).
#define PIN_LED          17   // LED indicador: PWM por SOFTWARE (GPIO 17 nao tem PWM de hardware)
#define PIN_SERVO        18   // Servo: gpioServo, pulso 50 Hz (nao usar junto com o WS2812)
#define PIN_BUZZER       4    // Buzzer da placa (ver BUZZER_PASSIVO abaixo)
#define PIN_BOTAO_MAIS   21   // Botao incrementa BPM
#define PIN_BOTAO_MENOS  20   // Botao decrementa BPM

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
#define BUZZER_PASSIVO   1       // 1 = passivo (PWM na ressonancia); 0 = ativo (nivel digital)
#define BUZZER_TOM_HZ    2000    // Ressonancia do buzzer passivo do kit
#define DEBOUNCE_US      200000  // Janela de debounce mecanico (200 ms em us)
#define GLITCH_US        300     // Filtro de ruido eletrico (us)
#define LED_PWM_HZ       1000    // Frequencia do PWM do LED (persistencia da visao)
#define LED_PWM_RANGE    1000    // Resolucao do duty: 0 = apagado, LED_PWM_RANGE = 100%
#define BUZZER_HABILITADO 1      // 1 = som ligado; 0 = silencioso (LED/servo seguem)

// =============================================================================
// Interface publica
// =============================================================================
bool initMetronomo();     // Inicia pigpio, configura pinos e ISRs. false se falhar.
void passoMetronomo();    // Uma batida + espera compensada (uma iteracao do loop).
void encerrarMetronomo(); // Relaxa servo, desliga buzzer, encerra pigpio.
int  obterBPM();          // BPM atual (para log).

#endif // METRONOMO_H
