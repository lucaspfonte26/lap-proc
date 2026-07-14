// Teste isolado do buzzer ATIVO da placa (sinal digital liga/desliga): padrao de beeps.
// Buzzer ativo tem oscilador interno -> nao precisa de PWM, so de nivel alto.
// Compilar: g++ -std=c++17 teste_buzzer.cpp -o teste_buzzer -lpigpio -lrt -pthread
// Executar: sudo ./teste_buzzer
#include "metronomo.h"
#include <pigpio.h>
#include <chrono>
#include <thread>
#include <cstdio>

static void beep(int ms) {
    gpioWrite(PIN_BUZZER, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    gpioWrite(PIN_BUZZER, 0);
}

int main() {
    if (gpioInitialise() < 0) {
        std::fprintf(stderr, "ERRO: rode 'sudo pigpiod' e execute com sudo.\n");
        return 1;
    }
    gpioSetMode(PIN_BUZZER, PI_OUTPUT);

    std::printf("3 beeps curtos (%d ms)...\n", BEEP_MS);
    for (int i = 0; i < 3; i++) {
        beep(BEEP_MS);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    std::printf("1 beep longo (500 ms)...\n");
    beep(500);

    gpioWrite(PIN_BUZZER, 0);
    gpioTerminate();
    return 0;
}
