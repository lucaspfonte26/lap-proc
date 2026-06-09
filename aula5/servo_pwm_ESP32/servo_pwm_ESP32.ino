#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include "interface.h"

// Servo: SINAL -> GPIO4 | VCC -> 5V (ideal: fonte externa) | GND comum
WebServer server(80);
Servo servo;

const int SERVO_PIN = 3;
int servoAng = 90;

void setup() {
    Serial.begin(115200);

    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    servo.setPeriodHertz(50);
    servo.attach(SERVO_PIN, 1000, 2000);   // 1.0 ms -> 0 graus | 2.0 ms -> 180 graus
    servo.write(servoAng);

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
    servo.write(servoAng);
    Serial.printf("Servo: %d graus\n", servoAng);
    server.send(200, "text/plain", "ok");
}
