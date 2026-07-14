#ifndef METRONOMO_H
#define METRONOMO_H

// Pinos fixos da Freenove Projects Board (BCM). PWM de hardware so existe nos
// GPIOs 12/13/18/19, entao LED e buzzer usam PWM por software.
#define PIN_LED          17
#define PIN_SERVO        18
#define PIN_BUZZER       4    // buzzer passivo: so apita com onda quadrada
#define PIN_BOTAO_MAIS   21
#define PIN_BOTAO_MENOS  20

#define BPM_INICIAL      60
#define BPM_MIN          40
#define BPM_MAX          240
#define BPM_PASSO        5

#define SERVO_MIN_US     1000    // 0 graus
#define SERVO_MID_US     1500    // 90 graus
#define SERVO_MAX_US     2000    // 180 graus
#define SERVO_ESQ_US     1200    // "tique"
#define SERVO_DIR_US     1800    // "taque"

#define BEEP_MS          30
#define BUZZER_TOM_HZ    2000    // ressonancia do buzzer do kit
#define BUZZER_HABILITADO 1      // 0 = silencioso; LED e servo seguem batendo
#define LED_PWM_HZ       1000
#define LED_PWM_RANGE    1000
#define DEBOUNCE_US      200000
#define GLITCH_US        300

bool initMetronomo();
void passoMetronomo();
void encerrarMetronomo();
int  obterBPM();

#endif
