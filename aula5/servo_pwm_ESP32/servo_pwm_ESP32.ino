#include <WiFi.h>
#include <WebServer.h>
#include "interface.h"

// Hardware: servo SINAL -> GPIO4 | VCC -> 5V (ideal: fonte externa) | GND comum
WebServer server(80);

const int SERVO_PIN    = 4;
const int SERVO_FREQ   = 50;               // Hz -> periodo de 20 ms
const int SERVO_RES    = 12;               // resolucao do duty: 0..4095
const int PERIODO_US   = 20000;            // 20 ms em microssegundos
const int PULSO_MIN_US = 1000;             // 1.0 ms  -> 0 graus
const int PULSO_MAX_US = 2000;             // 2.0 ms  -> 180 graus
const int MAX_DUTY     = (1 << SERVO_RES) - 1;

int servoAng = 90;                         // posicao atual (graus)

// converte angulo -> largura de pulso (us) -> duty cycle e atua no LEDC
void aplicarAngulo() {
    int us   = map(servoAng, 0, 180, PULSO_MIN_US, PULSO_MAX_US); // regra de 3
    int duty = map(us, 0, PERIODO_US, 0, MAX_DUTY);
    ledcWrite(SERVO_PIN, duty);
}

void setup() {
    Serial.begin(115200);

    ledcAttach(SERVO_PIN, SERVO_FREQ, SERVO_RES);   // LEDC: timer 50 Hz + canal
    aplicarAngulo();                                // centraliza em 90 graus

    WiFi.softAP("Servo_ESP32_Fialho");
    Serial.print("Access Point iniciado. IP: ");
    Serial.println(WiFi.softAPIP());

    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", HTML_INTERFACE);
    });
    server.on("/servo", HTTP_GET, handleServo);

    server.begin();
    Serial.println("Servidor HTTP rodando na porta 80.");
}

void loop() {
    server.handleClient();
}

// /servo?ang=0..180  -> posiciona o servo
void handleServo() {
    servoAng = constrain(server.arg("ang").toInt(), 0, 180);
    aplicarAngulo();
    int us = map(servoAng, 0, 180, PULSO_MIN_US, PULSO_MAX_US);
    Serial.printf("Servo: %d graus (pulso %d us) @ %d Hz\n", servoAng, us, SERVO_FREQ);
    server.send(200, "text/plain", "ok");
}
