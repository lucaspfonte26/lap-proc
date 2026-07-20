#!/usr/bin/env python3
########################################################################
# Arquivo     : lock-main.py
# Função      : Orquestrador da Fechadura - Máquina de Estados com Cooldown
########################################################################
import Keypad
from LCD1602 import CharLCD1602
from gpiozero import DistanceSensor, TonalBuzzer
from gpiozero.tones import Tone
import time
import warnings
import security # Importa o nosso módulo de segurança da Fase 3

warnings.filterwarnings("ignore")

# CONFIGURAÇÕES DE HARDWARE (Mantendo mapeamento Freenove)
ROWS, COLS = 4, 4
keys = ['1','2','3','A', '4','5','6','B', '7','8','9','C', '*','0','#','D']
rowsPins = [16, 20, 21, 26]
colsPins = [19, 13, 6, 5]

keypad = Keypad.Keypad(keys, rowsPins, colsPins, ROWS, COLS)
keypad.setDebounceTime(50)

lcd = CharLCD1602()
buzzer = TonalBuzzer(4)
sensor = DistanceSensor(echo=15, trigger=14, max_distance=3)
LIMIAR_PORTA_FECHADA_CM = 5.0
LIMITE_DIGITOS = 6

# ESTADOS: IDLE, INPUT, SUCESSO, FALHA, ALARME, COOLDOWN
estado_atual = "IDLE"
senha_buffer = ""

tempo_transicao_estado = 0
tempo_ultima_atualizacao_lcd = 0
buzzer_desliga_em = 0
buzzer_estado = False

def formatar_linha(texto):
    return texto.ljust(16)

def atualizar_display(linha0, linha1):
    lcd.write(0, 0, formatar_linha(linha0))
    lcd.write(0, 1, formatar_linha(linha1))

def acionar_buzzer(frequencia, duracao_segundos):
    global buzzer_desliga_em, buzzer_estado
    buzzer.play(Tone(frequencia))
    buzzer_desliga_em = time.time() + duracao_segundos
    buzzer_estado = True

def setup():
    lcd.init_lcd()
    atualizar_display("SISTEMA SEGURO", "STATUS: TRANCADA")
    time.sleep(1)

def loop_principal():
    global estado_atual, senha_buffer, tempo_transicao_estado, tempo_ultima_atualizacao_lcd, buzzer_estado
    
    while True:
        agora = time.time()
        
        # Gerenciamento de Áudio Não-Bloqueante
        if buzzer_estado and agora >= buzzer_desliga_em:
            buzzer.stop()
            buzzer_estado = False
            
        # POLLING DO SENSOR (RF3): Executado SEMPRE, inclusive em Cooldown
        distancia = sensor.distance * 100
        porta_fechada = distancia <= LIMIAR_PORTA_FECHADA_CM
        
        if not porta_fechada and estado_atual not in ["SUCESSO", "ALARME"]:
            estado_atual = "ALARME"
            atualizar_display("ALERTA VIOLACAO!", "PORTA ARROMBADA!")
            acionar_buzzer(220.0, 5.0)
            
        # MÁQUINA DE ESTADOS REFINADA
        if estado_atual == "IDLE":
            key = keypad.getKey()
            if key != keypad.NULL:
                estado_atual = "INPUT"
                senha_buffer = ""
                processar_tecla(key)

        elif estado_atual == "INPUT":
            key = keypad.getKey()
            if key != keypad.NULL:
                processar_tecla(key)

        elif estado_atual == "SUCESSO" or estado_atual == "FALHA":
            if agora >= tempo_transicao_estado:
                if security.esta_bloqueado(): # Se a falha gerou bloqueio
                    estado_atual = "COOLDOWN"
                elif porta_fechada:
                    estado_atual = "IDLE"
                    atualizar_display("STATUS: TRANCADA", "Aguardando...")
                else:
                    atualizar_display("STATUS: ABERTA", "Feche a porta...")

        elif estado_atual == "COOLDOWN":
            # Verifica se o tempo de penalidade expirou
            if not security.esta_bloqueado():
                estado_atual = "IDLE"
                atualizar_display("SISTEMA REATIVADO", "STATUS: TRANCADA")
                print("[SISTEMA] Cooldown finalizado. Teclado liberado.")
            else:
                # Atualiza o contador regressivo na tela a cada 1 segundo (Não-bloqueante)
                if agora - tempo_ultima_atualizacao_lcd >= 1.0:
                    tempo_restante = security.tempo_restante_cooldown()
                    atualizar_display("SISTEMA BLOQUEADO", f"Aguarde: {tempo_restante}s")
                    tempo_ultima_atualizacao_lcd = agora
                
                # Ignora deliberadamente qualquer entrada do teclado durante o Cooldown
                _ = keypad.getKey()

def processar_tecla(key):
    global estado_atual, senha_buffer, tempo_transicao_estado
    agora = time.time()
    
    if key == '#': 
        # Utiliza a validação criptográfica modularizada
        if security.verificar_senha(senha_buffer):
            estado_atual = "SUCESSO"
            atualizar_display("ACESSO LIBERADO", "Bem-vindo!")
            acionar_buzzer(440.0, 0.2)
            tempo_transicao_estado = agora + 3.0
        else:
            estado_atual = "FALHA"
            atualizar_display("ACESSO NEGADO!", "Senha Incorreta")
            acionar_buzzer(220.0, 0.5)
            tempo_transicao_estado = agora + 2.0
            
    elif key == '*': 
        if len(senha_buffer) > 0:
            senha_buffer = senha_buffer[:-1]
            exibir_ofuscacao()
    else: 
        if len(senha_buffer) < LIMITE_DIGITOS:
            senha_buffer += key
            exibir_ofuscacao()

def exibir_ofuscacao():
    atualizar_display("DIGITE A SENHA", "Senha: " + "*" * len(senha_buffer))

if __name__ == '__main__':
    setup()
    try:
        loop_principal()
    except KeyboardInterrupt:
        lcd.clear()
        buzzer.close()
        sensor.close()