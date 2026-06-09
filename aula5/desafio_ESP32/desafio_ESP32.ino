#include <WiFi.h>
#include <WebServer.h>
#include "interface.h"

// LED: GPIO3 + resistor 220-330 ohm -> GND | Servo: sinal GPIO4, VCC 5V, GND comum
WebServer server(80);

const int LED_PIN      = 3;
const int LED_RES      = 13;
const int LED_MAX_DUTY = (1 << LED_RES) - 1;
int ledFreq  = 5000;
int ledNivel = 0;

const int SERVO_PIN      = 4;
const int SERVO_FREQ     = 50;
const int SERVO_RES      = 12;
const int SERVO_MAX_DUTY = (1 << SERVO_RES) - 1;
const int PERIODO_US     = 20000;
const int PULSO_MIN_US   = 1000;
const int PULSO_MAX_US   = 2000;
int servoAng = 90;

void aplicarBrilho() {
    int duty = map(ledNivel, 0, 100, 0, LED_MAX_DUTY);
    ledcWrite(LED_PIN, duty);
}

void aplicarAngulo() {
    int us   = map(servoAng, 0, 180, PULSO_MIN_US, PULSO_MAX_US);
    int duty = map(us, 0, PERIODO_US, 0, SERVO_MAX_DUTY);
    ledcWrite(SERVO_PIN, duty);
}

void setup() {
    Serial.begin(115200);

    ledcAttach(LED_PIN, ledFreq, LED_RES);         // timer LEDC do LED (kHz)
    ledcAttach(SERVO_PIN, SERVO_FREQ, SERVO_RES);  // timer LEDC do servo (50 Hz)
    aplicarBrilho();
    aplicarAngulo();

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
    aplicarAngulo();
    int us = map(servoAng, 0, 180, PULSO_MIN_US, PULSO_MAX_US);
    Serial.printf("Servo: %d graus (pulso %d us)\n", servoAng, us);
    server.send(200, "text/plain", "ok");
}
