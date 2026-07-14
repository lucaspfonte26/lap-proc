#ifndef METRONOMO_H
#define METRONOMO_H

// Desafio: BPM configuravel pelos 4 botoes da placa, em tempo de execucao.
// Copia autonoma do metronomo de aula9/ — nao compartilha arquivo com ele.

// Pinos fixos da Freenove Projects Board (BCM). PWM de hardware so existe nos
// GPIOs 12/13/18/19, entao LED e buzzer usam PWM por software.
#define PIN_LED          17
#define PIN_SERVO        18
#define PIN_BUZZER       4    // buzzer passivo: so apita com onda quadrada

#define PIN_BOTAO_MAIS   21   // +5 BPM
#define PIN_BOTAO_MENOS  20   // -5 BPM
#define PIN_BOTAO_RESET  26   // volta ao BPM inicial
#define PIN_BOTAO_SOM    16   // liga/desliga o buzzer

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
#define SOM_INICIAL      1       // estado inicial; o botao 16 alterna depois
#define LED_PWM_HZ       1000
#define LED_PWM_RANGE    1000
#define DEBOUNCE_US      200000
#define GLITCH_US        300

bool initMetronomo();
void passoMetronomo();
void encerrarMetronomo();
int  obterBPM();
bool somLigado();

#endif
