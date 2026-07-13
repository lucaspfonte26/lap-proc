#include "metronomo.h"
#include <pigpio.h>
#include <atomic>
#include <algorithm>
#include <chrono>
#include <thread>
#include <cstdio>
#include <cstdint>

// =============================================================================
// Estado compartilhado (a ISR roda em uma thread separada do pigpio)
// =============================================================================
static std::atomic<int> g_bpm{BPM_INICIAL};
static uint32_t g_ultimaBordaMais = 0;
static uint32_t g_ultimaBordaMenos = 0;
static bool g_tique = true;   // alterna a posicao do servo a cada batida

// =============================================================================
// Rotinas de Servico de Interrupcao dos botoes
// Debounce em dois niveis: glitch filter no pino (us) + janela de 200 ms aqui.
// =============================================================================
static void ajustarBPM(int delta) {
    int novo = g_bpm.load() + delta;
    novo = std::max(BPM_MIN, std::min(BPM_MAX, novo));
    g_bpm.store(novo);
    std::printf("BPM alterado para %d\n", novo);
}

static void isrBotaoMais(int gpio, int level, uint32_t tick) {
    (void)gpio; (void)level;
    if (tick - g_ultimaBordaMais < DEBOUNCE_US) return;  // subtracao unsigned trata wrap
    g_ultimaBordaMais = tick;
    ajustarBPM(+BPM_PASSO);
}

static void isrBotaoMenos(int gpio, int level, uint32_t tick) {
    (void)gpio; (void)level;
    if (tick - g_ultimaBordaMenos < DEBOUNCE_US) return;
    g_ultimaBordaMenos = tick;
    ajustarBPM(-BPM_PASSO);
}

// =============================================================================
// Inicializacao e encerramento
// =============================================================================
bool initMetronomo() {
    if (gpioInitialise() < 0) {
        std::fprintf(stderr, "ERRO: pigpio nao inicializou. Rode 'sudo pigpiod' e execute com sudo.\n");
        return false;
    }

    gpioSetMode(PIN_BUZZER, PI_OUTPUT);
    gpioWrite(PIN_BUZZER, 0);

    gpioSetMode(PIN_BOTAO_MAIS, PI_INPUT);
    gpioSetMode(PIN_BOTAO_MENOS, PI_INPUT);
    gpioSetPullUpDown(PIN_BOTAO_MAIS, PI_PUD_UP);
    gpioSetPullUpDown(PIN_BOTAO_MENOS, PI_PUD_UP);
    gpioGlitchFilter(PIN_BOTAO_MAIS, GLITCH_US);
    gpioGlitchFilter(PIN_BOTAO_MENOS, GLITCH_US);
    gpioSetISRFunc(PIN_BOTAO_MAIS, FALLING_EDGE, 0, isrBotaoMais);
    gpioSetISRFunc(PIN_BOTAO_MENOS, FALLING_EDGE, 0, isrBotaoMenos);

    gpioHardwarePWM(PIN_LED, LED_PWM_HZ, 0);  // LED apagado
    gpioServo(PIN_SERVO, SERVO_MID_US);       // servo centralizado
    return true;
}

void encerrarMetronomo() {
    gpioHardwarePWM(PIN_LED, LED_PWM_HZ, 0);
    gpioWrite(PIN_BUZZER, 0);
    gpioServo(PIN_SERVO, 0);   // pulso 0 = para de acionar o servo (relaxa)
    gpioTerminate();
}

int obterBPM() { return g_bpm.load(); }

// =============================================================================
// Uma iteracao do metronomo: batida sincronizada + espera anti-drift
// =============================================================================
void passoMetronomo() {
    const auto t_ini = std::chrono::steady_clock::now();
    const long periodo_us = 60000000L / g_bpm.load();

    // --- Batida: servo (tique-taque), LED aceso e buzzer sincronizados ---
    gpioServo(PIN_SERVO, g_tique ? SERVO_ESQ_US : SERVO_DIR_US);
    g_tique = !g_tique;
    gpioHardwarePWM(PIN_LED, LED_PWM_HZ, 1000000);  // duty 100%
#if BUZZER_HABILITADO
    gpioWrite(PIN_BUZZER, 1);
#endif
    std::this_thread::sleep_for(std::chrono::milliseconds(BEEP_MS));
    gpioWrite(PIN_BUZZER, 0);
    gpioHardwarePWM(PIN_LED, LED_PWM_HZ, 0);        // LED apagado

    // --- Espera compensatoria (anti-drift): dorme o grosso, busy-wait no fim ---
    auto gasto = [&] {
        return std::chrono::duration_cast<std::chrono::microseconds>(
                   std::chrono::steady_clock::now() - t_ini).count();
    };
    const long restante = periodo_us - static_cast<long>(gasto());
    if (restante <= 0) {
        std::fprintf(stderr, "AVISO: laco estourou o periodo (BPM alto demais?).\n");
        return;   // nunca dorme tempo negativo
    }
    if (restante > 1000) {
        std::this_thread::sleep_for(std::chrono::microseconds(restante - 1000));
    }
    while (gasto() < periodo_us) { /* espera ativa final para jitter < 5 ms */ }
}
