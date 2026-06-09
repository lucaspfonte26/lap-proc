#include <WiFi.h>
#include <WebServer.h>
#include "interface.h"

// Hardware: GPIO3 --[220-330 ohm]--|>|-- GND  (LED externo)
WebServer server(80);

const int LED_PIN  = 3;
const int LED_RES  = 13;              // resolucao do duty: 0..8191
const int MAX_DUTY = (1 << LED_RES) - 1;

int ledFreq  = 5000;                  // Hz, configuravel pela interface
int ledNivel = 0;                     // brilho em % (0..100)

void aplicarBrilho() {
    int duty = map(ledNivel, 0, 100, 0, MAX_DUTY);
    ledcWrite(LED_PIN, duty);
}

void setup() {
    Serial.begin(115200);

    ledcAttach(LED_PIN, ledFreq, LED_RES);   // LEDC: timer + canal em hardware
    aplicarBrilho();

    WiFi.softAP("LED_PWM_ESP32_Fialho");
    Serial.print("Access Point iniciado. IP: ");
    Serial.println(WiFi.softAPIP());

    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", HTML_INTERFACE);
    });
    server.on("/led", HTTP_GET, handleLed);
    server.on("/freq", HTTP_GET, handleFreq);

    server.begin();
    Serial.println("Servidor HTTP rodando na porta 80.");
}

void loop() {
    server.handleClient();
}

// /led?val=0..100  -> ajusta o brilho (duty cycle)
void handleLed() {
    ledNivel = constrain(server.arg("val").toInt(), 0, 100);
    aplicarBrilho();
    Serial.printf("Brilho: %d%% (duty %d/%d) @ %d Hz\n",
                  ledNivel, map(ledNivel, 0, 100, 0, MAX_DUTY), MAX_DUTY, ledFreq);
    server.send(200, "text/plain", "ok");
}

// /freq?hz=...  -> reconfigura a frequencia do PWM (teste de flicker)
void handleFreq() {
    ledFreq = constrain(server.arg("hz").toInt(), 10, 8000);  // 13 bits limita ~9.7 kHz
    ledcChangeFrequency(LED_PIN, ledFreq, LED_RES);
    aplicarBrilho();                  // reaplica o duty apos trocar a frequencia
    Serial.printf("Frequencia PWM: %d Hz\n", ledFreq);
    server.send(200, "text/plain", String(ledFreq));
}
