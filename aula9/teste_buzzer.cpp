// Teste isolado do buzzer PASSIVO da placa (GPIO 4).
// Passivo nao tem oscilador interno: so apita com PWM. O nivel digital nao produz som.
// Compilar: g++ -std=c++17 teste_buzzer.cpp -o teste_buzzer -lpigpio -lrt -pthread
// Executar: sudo ./teste_buzzer
#include "metronomo.h"
#include <pigpio.h>
#include <chrono>
#include <thread>
#include <cstdio>

static void tom(int hz, int ms) {
    gpioSetPWMfrequency(PIN_BUZZER, hz);
    gpioPWM(PIN_BUZZER, 50);   // 50% de duty sobre range 100
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

    std::printf("3 beeps curtos (%d ms) na ressonancia (%d Hz)...\n", BEEP_MS, BUZZER_TOM_HZ);
    for (int i = 0; i < 3; i++) {
        tom(BUZZER_TOM_HZ, BEEP_MS);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    std::printf("1 beep longo (500 ms)...\n");
    tom(BUZZER_TOM_HZ, 500);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Varias frequencias -> o passivo muda de tom, ao contrario do ativo.
    // Anotar em qual delas o som e mais alto (deve ser perto de 2 kHz).
    const int tons[] = {500, 1000, 2000, 4000};
    for (int hz : tons) {
        int real = gpioSetPWMfrequency(PIN_BUZZER, hz);
        std::printf("Tom pedido = %d Hz | aplicado = %d Hz\n", hz, real);
        tom(hz, 400);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    gpioPWM(PIN_BUZZER, 0);
    gpioTerminate();
    return 0;
}
