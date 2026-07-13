# =============================================================================
# Metronomo no Raspberry Pi 3 (PCS3732 - Aula 9)
#
# Fiacao (numeracao BCM):
#   LED status   -> GPIO18  (resistor 330 ohm em serie ao anodo; catodo no GND)
#   Servo SG90   -> sinal GPIO17 ; VCC 5V (fonte externa recomendada) ; GND comum
#   Buzzer ativo -> GPIO27  (+ no pino, - no GND)
#   Botao +BPM   -> GPIO23 ao GND (usa pull-up interno)
#   Botao -BPM   -> GPIO24 ao GND (usa pull-up interno)
#
# Como usar no laboratorio:
#   1) sudo pigpiod       (inicia o daemon, se ainda nao estiver rodando)
#   2) make               (compila os 4 binarios)
#   3) sudo ./metronomo   (ou ./teste_led, ./teste_servo, ./teste_buzzer)
# =============================================================================
CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDLIBS   = -lpigpio -lrt -pthread

BINS = metronomo teste_led teste_servo teste_buzzer

all: $(BINS)

metronomo: main.cpp metronomo.cpp metronomo.h
	$(CXX) $(CXXFLAGS) main.cpp metronomo.cpp -o $@ $(LDLIBS)

teste_led: teste_led.cpp metronomo.h
	$(CXX) $(CXXFLAGS) teste_led.cpp -o $@ $(LDLIBS)

teste_servo: teste_servo.cpp metronomo.h
	$(CXX) $(CXXFLAGS) teste_servo.cpp -o $@ $(LDLIBS)

teste_buzzer: teste_buzzer.cpp metronomo.h
	$(CXX) $(CXXFLAGS) teste_buzzer.cpp -o $@ $(LDLIBS)

clean:
	rm -f $(BINS)

.PHONY: all clean
