#!/usr/bin/env python3
########################################################################
# Módulo de Validação: Display LCD1602 via I2C
# Objetivo: Validar endereçamento I2C e simular estados da fechadura (RF2)
########################################################################
import smbus
from time import sleep
from LCD1602 import CharLCD1602

# Instancia o objeto do LCD a partir da biblioteca fornecida
lcd1602 = CharLCD1602()    

def testar_estados_fechadura():
    lcd1602.init_lcd()
    
    print("==================================================")
    print("      TESTE ISOLADO: DISPLAY LCD (I2C)            ")
    print("==================================================")
    print("-> Simulando as transições de estado na tela...")
    
    while(True):
        # Estado 1: Idle (Aguardando entrada)
        lcd1602.clear()
        lcd1602.write(0, 0, 'STATUS: TRANCADA') # Linha 0 (Superior)
        lcd1602.write(0, 1, 'Insira a Senha: ') # Linha 1 (Inferior)
        print("[VISUAL] Exibindo Estado Idle")
        sleep(2)
        
        # Estado 2: Evento de Entrada (Ofuscando caracteres)
        lcd1602.clear()
        lcd1602.write(0, 0, 'STATUS: TRANCADA')
        lcd1602.write(0, 1, 'Senha: ****     ')
        print("[VISUAL] Exibindo ofuscação de senha (*)")
        sleep(1.5)
        
        # Estado 3: Sucesso (Desbloqueado)
        lcd1602.clear()
        lcd1602.write(0, 0, 'STATUS: ABERTO  ')
        lcd1602.write(0, 1, 'Bem-vindo!      ')
        print("[VISUAL] Exibindo Sucesso (Transição para Aberto)")
        sleep(2)
        
        # Estado 4: Falha (Acesso Negado)
        lcd1602.clear()
        lcd1602.write(0, 0, 'ACESSO NEGADO!  ')
        lcd1602.write(0, 1, 'Tente Novamente ')
        print("[VISUAL] Exibindo Falha de processamento")
        sleep(2)

def destroy():
    # Limpa a tela antes de encerrar o programa para não deixar lixo visual
    lcd1602.clear()
    print("\n[INFO] Display limpo. Encerrando teste.")

if __name__ == '__main__':
    print ('Iniciando programa de teste do LCD...')
    try:
        # Nota sobre Depuração (Nível 2): 
        # Certifique-se de rodar 'i2cdetect -y 1' no terminal da RPi 
        # antes de rodar este script para confirmar o endereço do hardware.
        testar_estados_fechadura()
    except KeyboardInterrupt:
        destroy()