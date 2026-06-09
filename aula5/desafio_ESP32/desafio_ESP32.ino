#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include "interface.h"

// LED: GPIO3 + resistor 220-330 ohm -> GND | Servo: sinal GPIO4, VCC 5V, GND comum
WebServer server(80);
Servo servo;

// ---- LED: LEDC direto (kHz), como no led_pwm_ESP32 ----
const int LED_PIN      = 3;
const int LED_RES      = 11;
const int LED_MAX_DUTY = (1 << LED_RES) - 1;
int ledFreq  = 5000;
int ledNivel = 0;

// ---- Servo: biblioteca ESP32Servo (50 Hz), como no servo_pwm_ESP32 ----
const int SERVO_PIN = 4;
int servoAng = 90;

void aplicarBrilho() {
    int duty = map(ledNivel, 0, 100, 0, LED_MAX_DUTY);
    ledcWrite(LED_PIN, duty);
}

void setup() {
    Serial.begin(115200);

    if (!ledcAttach(LED_PIN, ledFreq, LED_RES))
        Serial.println("FALHA no ledcAttach do LED (reduza freq/resolucao)");
    aplicarBrilho();

    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    servo.setPeriodHertz(50);
    servo.attach(SERVO_PIN, 1000, 2000);
    servo.write(servoAng);

    WiFi.softAP("Painel_ESP32_Fialho");
    Serial.print("Access Point iniciado. IP: ");
    Serial.println(WiFi.softAPIP());

    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", HTML_INTERFACE);
    });
    server.on("/led",   HTTP_GET, handleLed);
    server.on("/freq",  HTTP_GET, handleFreq);
    server.on("/servo", HTTP_GET, handleServo);

    server.begin();
    Serial.println("Servidor HTTP rodando na porta 80.");
}

void loop() {
    server.handleClient();
}

void handleLed() {
    ledNivel = constrain(server.arg("val").toInt(), 0, 100);
    aplicarBrilho();
    Serial.printf("LED: %d%% @ %d Hz\n", ledNivel, ledFreq);
    server.send(200, "text/plain", "ok");
}

void handleFreq() {
    ledFreq = constrain(server.arg("hz").toInt(), 10, 8000);
    ledcChangeFrequency(LED_PIN, ledFreq, LED_RES);
    aplicarBrilho();
    Serial.printf("LED freq: %d Hz\n", ledFreq);
    server.send(200, "text/plain", String(ledFreq));
}

void handleServo() {
    servoAng = constrain(server.arg("ang").toInt(), 0, 180);
    servo.write(servoAng);
    Serial.printf("Servo: %d graus\n", servoAng);
    server.send(200, "text/plain", "ok");
}
