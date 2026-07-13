// Teste isolado do servo SG90 (gpioServo): posicoes fixas + varredura suave.
// Compilar: g++ -std=c++17 teste_servo.cpp -o teste_servo -lpigpio -lrt -pthread
// Executar: sudo ./teste_servo
#include "metronomo.h"
#include <pigpio.h>
#include <chrono>
#include <thread>
#include <cstdio>

static void ir(int us, int ms) {
    gpioServo(PIN_SERVO, us);
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

int main() {
    if (gpioInitialise() < 0) {
        std::fprintf(stderr, "ERRO: rode 'sudo pigpiod' e execute com sudo.\n");
        return 1;
    }

    std::printf("Posicoes: 0, 90 e 180 graus\n");
    ir(SERVO_MIN_US, 800);
    ir(SERVO_MID_US, 800);
    ir(SERVO_MAX_US, 800);

    std::printf("Varredura suave 0 -> 180 -> 0\n");
    for (int us = SERVO_MIN_US; us <= SERVO_MAX_US; us += 25) ir(us, 20);
    for (int us = SERVO_MAX_US; us >= SERVO_MIN_US; us -= 25) ir(us, 20);

    gpioServo(PIN_SERVO, 0);   // relaxa o servo
    gpioTerminate();
    return 0;
}
