// Teste isolado do LED (PWM por hardware): rampa de brilho + varias frequencias.
// Compilar: g++ -std=c++17 teste_led.cpp -o teste_led -lpigpio -lrt -pthread
// Executar: sudo ./teste_led
#include "metronomo.h"
#include <pigpio.h>
#include <chrono>
#include <thread>
#include <cstdio>

int main() {
    if (gpioInitialise() < 0) {
        std::fprintf(stderr, "ERRO: rode 'sudo pigpiod' e execute com sudo.\n");
        return 1;
    }

    // 1) Rampa de brilho (duty 0 -> 100 -> 0 %) a 1 kHz -> observar variacao suave
    std::printf("Rampa de brilho a %d Hz...\n", LED_PWM_HZ);
    for (int duty = 0; duty <= 1000000; duty += 50000) {
        gpioHardwarePWM(PIN_LED, LED_PWM_HZ, duty);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    for (int duty = 1000000; duty >= 0; duty -= 50000) {
        gpioHardwarePWM(PIN_LED, LED_PWM_HZ, duty);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    // 2) Varias frequencias a 50% de duty -> anotar a partir de qual some o flicker
    const int freqs[] = {5, 20, 50, 100, 500, 1000};
    for (int f : freqs) {
        std::printf("Frequencia = %d Hz (50%% duty)\n", f);
        gpioHardwarePWM(PIN_LED, f, 500000);
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    gpioHardwarePWM(PIN_LED, LED_PWM_HZ, 0);
    gpioTerminate();
    return 0;
}
