# Desafio da Aula 9: BPM configuravel pelos botoes fisicos da placa.
# Codigo autonomo — nao depende de aula9/ e nao altera nada la.
#
# Hardware: Freenove Projects Board. Pinos BCM:
#   LED 17 | Servo 18 | Buzzer passivo 4 (PWM 2 kHz)
#   Botoes: 21 (+5 BPM) | 20 (-5 BPM) | 26 (reset 60 BPM) | 16 (liga/desliga som)
#
# ANTES DE RODAR: ligar a chave geral de energia da placa E as chaves seletoras
# do buzzer, do servo e dos botoes. Sem isso so o LED funciona.
#
#   sudo killall pigpiod   (o daemon conflita com gpioInitialise)
#   make
#   sudo ./metronomo_desafio
CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDLIBS   = -lpigpio -lrt -pthread

metronomo_desafio: main.cpp metronomo.cpp metronomo.h
	$(CXX) $(CXXFLAGS) main.cpp metronomo.cpp -o $@ $(LDLIBS)

clean:
	rm -f metronomo_desafio

.PHONY: clean
