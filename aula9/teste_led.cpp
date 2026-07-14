// LED (GPIO 17): rampa de brilho + varredura de frequencias.
// sudo ./teste_led
#include "metronomo.h"
#include <pigpio.h>
#include <chrono>
#include <thread>
#include <cstdio>

int main() {
    if (gpioInitialise() < 0) {
        std::fprintf(stderr, "ERRO: pare o pigpiod (sudo killall pigpiod) e execute com sudo.\n");
        return 1;
    }
    gpioSetMode(PIN_LED, PI_OUTPUT);
    gpioSetPWMrange(PIN_LED, LED_PWM_RANGE);
    gpioSetPWMfrequency(PIN_LED, LED_PWM_HZ);

    std::printf("Rampa de brilho a %d Hz\n", LED_PWM_HZ);
    for (int duty = 0; duty <= LED_PWM_RANGE; duty += 50) {
        gpioPWM(PIN_LED, duty);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    for (int duty = LED_PWM_RANGE; duty >= 0; duty -= 50) {
        gpioPWM(PIN_LED, duty);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    // Anotar a partir de qual frequencia o flicker some (persistencia da visao).
    for (int f : {10, 20, 50, 100, 500, 1000}) {
        std::printf("Frequencia pedida = %d Hz | aplicada = %d Hz\n", f, gpioSetPWMfrequency(PIN_LED, f));
        gpioPWM(PIN_LED, LED_PWM_RANGE / 2);
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    gpioPWM(PIN_LED, 0);
    gpioTerminate();
    return 0;
}
