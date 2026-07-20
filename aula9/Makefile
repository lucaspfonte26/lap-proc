# Metronomo no Raspberry Pi 3 (PCS3732 - Aula 9)
# Hardware: Freenove Projects Board. Pinos BCM fixos da placa:
#   LED 17 (PWM software) | Servo 18 | Buzzer passivo 4 (PWM 2 kHz) | Botoes 21 (+) e 20 (-)
#
# ANTES DE RODAR: ligar a chave geral de energia da placa E as chaves seletoras
# do buzzer e do servo. Sem isso so o LED funciona.
#
#   sudo killall pigpiod   (o daemon conflita com gpioInitialise)
#   make
#   sudo ./metronomo       (ou ./teste_led, ./teste_servo, ./teste_buzzer)
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
