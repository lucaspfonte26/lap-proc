# =============================================================================
# Metronomo no Raspberry Pi 3 (PCS3732 - Aula 9)
#
# Hardware: Freenove Projects Board for Raspberry Pi (componentes ja soldados,
# nada de protoboard). Pinos fixos da placa, numeracao BCM:
#   LED indicador -> GPIO17  (sem PWM de hardware -> usamos gpioPWM, por software)
#   Servo         -> GPIO18  (conector de servo da placa)
#   Buzzer ativo  -> GPIO12  (sinal digital on/off; compartilha o GPIO com o rele)
#   Botao +BPM    -> GPIO21
#   Botao -BPM    -> GPIO20
#
# Como usar no laboratorio:
#   1) sudo killall pigpiod   (o daemon conflita com gpioInitialise(); precisa estar parado)
#   2) make                   (compila os 4 binarios)
#   3) sudo ./metronomo       (ou ./teste_led, ./teste_servo, ./teste_buzzer)
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
