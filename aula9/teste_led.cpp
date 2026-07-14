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

    gpioSetMode(PIN_LED, PI_OUTPUT);
    gpioSetPWMrange(PIN_LED, LED_PWM_RANGE);

    // 1) Rampa de brilho (duty 0 -> 100 -> 0 %) a 1 kHz -> observar variacao suave
    std::printf("Rampa de brilho a %d Hz...\n", LED_PWM_HZ);
    gpioSetPWMfrequency(PIN_LED, LED_PWM_HZ);
    for (int duty = 0; duty <= LED_PWM_RANGE; duty += 50) {
        gpioPWM(PIN_LED, duty);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    for (int duty = LED_PWM_RANGE; duty >= 0; duty -= 50) {
        gpioPWM(PIN_LED, duty);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    // 2) Varias frequencias a 50% de duty -> anotar a partir de qual some o flicker.
    // Valores validos do PWM por software do pigpio (amostragem de 5 us).
    const int freqs[] = {10, 20, 50, 100, 500, 1000};
    for (int f : freqs) {
        int real = gpioSetPWMfrequency(PIN_LED, f);
        gpioPWM(PIN_LED, LED_PWM_RANGE / 2);
        std::printf("Frequencia pedida = %d Hz | aplicada = %d Hz (50%% duty)\n", f, real);
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    gpioPWM(PIN_LED, 0);
    gpioTerminate();
    return 0;
}
