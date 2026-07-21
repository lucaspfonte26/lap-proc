#!/usr/bin/env python3
########################################################################
# Módulo de Validação: Sensor Ultrassônico (Monitoramento de Estado)
# Objetivo: Validar nível lógico de abertura/fechamento da porta (RF3)
########################################################################
from gpiozero import DistanceSensor
from time import sleep
import warnings  
warnings.filterwarnings("ignore")  
  
# Definição dos pinos conforme o padrão da placa Freenove
trigPin = 14
echoPin = 15

sensor = DistanceSensor(echo=echoPin, trigger=trigPin, max_distance=3)

def validar_sensor_distancia():
    DISTANCIA_PORTA_FECHADA_CM = 5.0
    
    print("==================================================")
    print("      TESTE ISOLADO: SENSOR ULTRASSÔNICO          ")
    print("==================================================")
    print("-> Monitorando a distância física do batente...")
    print(f"-> Porta considerada FECHADA se menor que {DISTANCIA_PORTA_FECHADA_CM}cm.\n")
    
    while True:
        distancia_atual = sensor.distance * 100
        
        if distancia_atual <= DISTANCIA_PORTA_FECHADA_CM:
            estado_porta = "FECHADA (Segura)"
            sinal_logico = "0V / LOW (Trancada)"
        else:
            estado_porta = "ABERTA / VIOLADA"
            sinal_logico = "3.3V / HIGH (Aberta)"
            
        print(f"[LEITURA] Distância: {distancia_atual:.2f} cm | Estado Lógico: {estado_porta} -> Sinal: {sinal_logico}")
        
        sleep(1)
        
if __name__ == '__main__':
    print('Iniciando programa de teste do sensor...')
    try:
        validar_sensor_distancia()
    except KeyboardInterrupt:
        print("\n[INFO] Finalizando teste do sensor ultrassônico.")
    finally:
        sensor.close()