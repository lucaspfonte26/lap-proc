#!/usr/bin/env python3
########################################################################
# Módulo de Validação: Buzzer
# Objetivo: Validar acionamento sonoro para feedback de sucesso e erro
########################################################################
from gpiozero import TonalBuzzer
from gpiozero.tones import Tone
import time

# Configuração do pino do Buzzer (usando o pino 4 conforme o exemplo da Freenove)
# A interface requer um pino GPIO com suporte a Sinal PWM ou Digital
buzzer = TonalBuzzer(4)

def bipe_sucesso():
    print("[ÁUDIO] Simulando (Sucesso) -> Relé/Tranca abre -> Buzzer (bipe curto)")
    buzzer.play(Tone(440.0)) # Frequência mais aguda/agradável (ex: Lá)
    time.sleep(0.2)          # Duração curta
    buzzer.stop()

def alarme_falha():
    print("[ÁUDIO] Simulando (Falha) -> Acesso Negado -> Buzzer (aviso longo/múltiplo)")
    # Loop para criar um som de alerta iterativo
    for _ in range(3):
        buzzer.play(Tone(220.0)) # Frequência mais grave para denotar erro
        time.sleep(0.3)
        buzzer.stop()
        time.sleep(0.1)

def testar_buzzer():
    print("==================================================")
    print("      TESTE ISOLADO: BUZZER (FEEDBACK SONORO)     ")
    print("==================================================")
    print("-> Validando saturação do transistor e perfis de áudio...")
    
    while True:
        # Dispara o evento de sucesso
        bipe_sucesso()
        time.sleep(2) # Pausa visual/auditiva entre os estados
        
        # Dispara o evento de falha
        alarme_falha()
        time.sleep(2) # Pausa visual/auditiva entre os estados

def destroy():
    buzzer.close() 
    print("\n[INFO] Recursos do Buzzer liberados. Encerrando teste.")

if __name__ == '__main__':
    print ('Program is starting...')
    try:
        testar_buzzer()
    except KeyboardInterrupt: # Pressione ctrl-c para finalizar o teste
        print("\nEnding program")
        destroy()