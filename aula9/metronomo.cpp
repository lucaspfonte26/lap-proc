#include "metronomo.h"
#include <pigpio.h>
#include <atomic>
#include <algorithm>
#include <chrono>
#include <thread>
#include <cstdio>
#include <cstdint>

// A ISR do pigpio roda em outra thread, por isso o BPM e atomico.
static std::atomic<int> g_bpm{BPM_INICIAL};
static uint32_t g_ultimaBordaMais = 0;
static uint32_t g_ultimaBordaMenos = 0;
static bool g_tique = true;

static void buzzerLigar()    { gpioPWM(PIN_BUZZER, 50); }   // 50% de duty
static void buzzerDesligar() { gpioPWM(PIN_BUZZER, 0); }

static void ajustarBPM(int delta) {
    int novo = std::max(BPM_MIN, std::min(BPM_MAX, g_bpm.load() + delta));
    g_bpm.store(novo);
    std::printf("BPM alterado para %d\n", novo);
}

// Debounce em dois niveis: glitch filter no pino + janela de 200 ms aqui.
static void isrBotaoMais(int, int, uint32_t tick) {
    if (tick - g_ultimaBordaMais < DEBOUNCE_US) return;   // subtracao unsigned trata wrap
    g_ultimaBordaMais = tick;
    ajustarBPM(+BPM_PASSO);
}

static void isrBotaoMenos(int, int, uint32_t tick) {
    if (tick - g_ultimaBordaMenos < DEBOUNCE_US) return;
    g_ultimaBordaMenos = tick;
    ajustarBPM(-BPM_PASSO);
}

bool initMetronomo() {
    if (gpioInitialise() < 0) {
        std::fprintf(stderr, "ERRO: pare o pigpiod (sudo killall pigpiod) e execute com sudo.\n");
        return false;
    }

    gpioSetMode(PIN_BUZZER, PI_OUTPUT);
    gpioSetPWMfrequency(PIN_BUZZER, BUZZER_TOM_HZ);
    gpioSetPWMrange(PIN_BUZZER, 100);
    buzzerDesligar();

    gpioSetMode(PIN_LED, PI_OUTPUT);
    gpioSetPWMfrequency(PIN_LED, LED_PWM_HZ);
    gpioSetPWMrange(PIN_LED, LED_PWM_RANGE);
    gpioPWM(PIN_LED, 0);

    gpioServo(PIN_SERVO, SERVO_MID_US);

    gpioSetMode(PIN_BOTAO_MAIS, PI_INPUT);
    gpioSetMode(PIN_BOTAO_MENOS, PI_INPUT);
    gpioSetPullUpDown(PIN_BOTAO_MAIS, PI_PUD_UP);
    gpioSetPullUpDown(PIN_BOTAO_MENOS, PI_PUD_UP);
    gpioGlitchFilter(PIN_BOTAO_MAIS, GLITCH_US);
    gpioGlitchFilter(PIN_BOTAO_MENOS, GLITCH_US);
    gpioSetISRFunc(PIN_BOTAO_MAIS, FALLING_EDGE, 0, isrBotaoMais);
    gpioSetISRFunc(PIN_BOTAO_MENOS, FALLING_EDGE, 0, isrBotaoMenos);
    return true;
}

void encerrarMetronomo() {
    gpioPWM(PIN_LED, 0);
    buzzerDesligar();
    gpioServo(PIN_SERVO, 0);   // pulso 0 relaxa o servo
    gpioTerminate();
}

int obterBPM() { return g_bpm.load(); }

void passoMetronomo() {
    const auto t_ini = std::chrono::steady_clock::now();
    const long periodo_us = 60000000L / g_bpm.load();

    gpioServo(PIN_SERVO, g_tique ? SERVO_ESQ_US : SERVO_DIR_US);
    g_tique = !g_tique;
    gpioPWM(PIN_LED, LED_PWM_RANGE);
#if BUZZER_HABILITADO
    buzzerLigar();
#endif
    std::this_thread::sleep_for(std::chrono::milliseconds(BEEP_MS));
    buzzerDesligar();
    gpioPWM(PIN_LED, 0);

    // Anti-drift: desconta o tempo da batida e dorme so o restante.
    auto gasto = [&] {
        return std::chrono::duration_cast<std::chrono::microseconds>(
                   std::chrono::steady_clock::now() - t_ini).count();
    };
    const long restante = periodo_us - static_cast<long>(gasto());
    if (restante <= 0) {
        std::fprintf(stderr, "AVISO: laco estourou o periodo (BPM alto demais?).\n");
        return;
    }
    if (restante > 1000) {
        std::this_thread::sleep_for(std::chrono::microseconds(restante - 1000));
    }
    while (gasto() < periodo_us) { /* sleep_for nao e preciso; ultimo ms em espera ativa */ }
}
