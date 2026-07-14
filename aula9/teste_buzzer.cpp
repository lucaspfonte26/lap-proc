// Buzzer passivo (GPIO 4): so apita com onda quadrada; nivel digital nao gera som.
// sudo ./teste_buzzer
#include "metronomo.h"
#include <pigpio.h>
#include <chrono>
#include <initializer_list>
#include <thread>
#include <cstdio>

static void tom(int hz, int ms) {
    gpioSetPWMfrequency(PIN_BUZZER, hz);
    gpioPWM(PIN_BUZZER, 50);
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    gpioPWM(PIN_BUZZER, 0);
}

int main() {
    if (gpioInitialise() < 0) {
        std::fprintf(stderr, "ERRO: pare o pigpiod (sudo killall pigpiod) e execute com sudo.\n");
        return 1;
    }
    gpioSetMode(PIN_BUZZER, PI_OUTPUT);
    gpioSetPWMrange(PIN_BUZZER, 100);

    std::printf("3 beeps curtos + 1 longo a %d Hz\n", BUZZER_TOM_HZ);
    for (int i = 0; i < 3; i++) {
        tom(BUZZER_TOM_HZ, BEEP_MS);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
    tom(BUZZER_TOM_HZ, 500);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // O passivo muda de tom com a frequencia. Anotar em qual soa mais alto.
    for (int hz : {500, 1000, 2000, 4000}) {
        std::printf("Tom pedido = %d Hz | aplicado = %d Hz\n", hz, gpioSetPWMfrequency(PIN_BUZZER, hz));
        tom(hz, 400);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    gpioTerminate();
    return 0;
}
