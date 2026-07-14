#include "metronomo.h"
#include <pigpio.h>
#include <atomic>
#include <algorithm>
#include <chrono>
#include <thread>
#include <cstdio>
#include <cstdint>

// As ISRs do pigpio rodam em outra thread, por isso o estado compartilhado e atomico.
static std::atomic<int>  g_bpm{BPM_INICIAL};
static std::atomic<bool> g_som{SOM_INICIAL};
static bool g_tique = true;

static uint32_t g_ultimaMais  = 0;
static uint32_t g_ultimaMenos = 0;
static uint32_t g_ultimaReset = 0;
static uint32_t g_ultimaSom   = 0;

static void buzzerLigar()    { gpioPWM(PIN_BUZZER, 50); }   // 50% de duty
static void buzzerDesligar() { gpioPWM(PIN_BUZZER, 0); }

// Debounce de software: ignora bordas dentro da janela. A subtracao unsigned
// trata o wrap do tick do pigpio. Complementa o glitch filter do pino.
static bool aceitaBorda(uint32_t& ultima, uint32_t tick) {
    if (tick - ultima < DEBOUNCE_US) return false;
    ultima = tick;
    return true;
}

static void ajustarBPM(int delta) {
    int novo = std::max(BPM_MIN, std::min(BPM_MAX, g_bpm.load() + delta));
    g_bpm.store(novo);
    std::printf("BPM = %d\n", novo);
}

static void isrMais(int, int, uint32_t tick) {
    if (aceitaBorda(g_ultimaMais, tick)) ajustarBPM(+BPM_PASSO);
}

static void isrMenos(int, int, uint32_t tick) {
    if (aceitaBorda(g_ultimaMenos, tick)) ajustarBPM(-BPM_PASSO);
}

static void isrReset(int, int, uint32_t tick) {
    if (!aceitaBorda(g_ultimaReset, tick)) return;
    g_bpm.store(BPM_INICIAL);
    std::printf("BPM = %d (reset)\n", BPM_INICIAL);
}

static void isrSom(int, int, uint32_t tick) {
    if (!aceitaBorda(g_ultimaSom, tick)) return;
    bool novo = !g_som.load();
    g_som.store(novo);
    if (!novo) buzzerDesligar();   // corta um beep em andamento
    std::printf("Som %s\n", novo ? "ligado" : "desligado");
}

static void configurarBotao(int pin, gpioISRFunc_t isr) {
    gpioSetMode(pin, PI_INPUT);
    gpioSetPullUpDown(pin, PI_PUD_UP);   // botao fecha para o GND
    gpioGlitchFilter(pin, GLITCH_US);
    gpioSetISRFunc(pin, FALLING_EDGE, 0, isr);
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

    configurarBotao(PIN_BOTAO_MAIS,  isrMais);
    configurarBotao(PIN_BOTAO_MENOS, isrMenos);
    configurarBotao(PIN_BOTAO_RESET, isrReset);
    configurarBotao(PIN_BOTAO_SOM,   isrSom);
    return true;
}

void encerrarMetronomo() {
    gpioPWM(PIN_LED, 0);
    buzzerDesligar();
    gpioServo(PIN_SERVO, 0);   // pulso 0 relaxa o servo
    gpioTerminate();
}

int  obterBPM()  { return g_bpm.load(); }
bool somLigado() { return g_som.load(); }

void passoMetronomo() {
    const auto t_ini = std::chrono::steady_clock::now();
    const long periodo_us = 60000000L / g_bpm.load();   // le o BPM no inicio da batida

    gpioServo(PIN_SERVO, g_tique ? SERVO_ESQ_US : SERVO_DIR_US);
    g_tique = !g_tique;
    gpioPWM(PIN_LED, LED_PWM_RANGE);
    if (g_som.load()) buzzerLigar();
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
