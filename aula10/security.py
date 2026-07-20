#!/usr/bin/env python3
########################################################################
# Arquivo     : security.py
# Função      : Tratamento de Hashes Criptográficos e Logica de Cooldown
########################################################################
import hashlib
import time

# Hash SHA-256 correspondente à senha padrão "1234"
# Evita o armazenamento de credenciais em texto plano na memória flash.
HASH_SENHA_GRAVADA = "03ac674216f3e15c761ee1a5e255f067953623c8b388b4459e13f978d7c846f4"

MAX_TENTATIVAS = 3
TEMPO_COOLDOWN_SEG = 15 # Reduzido para facilitar testes em bancada de laboratório

tentativas_falhas = 0
bloqueado_ate = 0

def verificar_senha(senha_texto_plano):
    """
    Calcula o hash da string recebida e compara com o valor seguro em memória.
    Incrementa o contador de falhas em caso de divergência.
    """
    global tentativas_falhas, bloqueado_ate
    
    # Executa a cifragem da entrada via SHA-256
    hash_calculado = hashlib.sha256(senha_texto_plano.encode('utf-8')).hexdigest()
    
    if hash_calculado == HASH_SENHA_GRAVADA:
        tentativas_falhas = 0
        return True
    else:
        tentativas_falhas += 1
        print(f"[SEGURANÇA] Falha registrada. Contador: {tentativas_falhas}/{MAX_TENTATIVAS}")
        
        if tentativas_falhas >= MAX_TENTATIVAS:
            bloqueado_ate = time.time() + TEMPO_COOLDOWN_SEG
            print(f"[SEGURANÇA] Limite excedido! Sistema bloqueado por {TEMPO_COOLDOWN_SEG}s.")
        return False

def esta_bloqueado():
    """Retorna True se o sistema estiver no período de penalidade por tentativas falhas."""
    return time.time() < bloqueado_ate

def tempo_restante_cooldown():
    """Calcula e retorna o tempo restante de bloqueio em segundos inteiros."""
    restante = bloqueado_ate - time.time()
    return max(0, int(restante))

def resetar_falhas():
    """Força o reset do contador (utilizado em reinicializações seguras)."""
    global tentativas_falhas
    tentativas_falhas = 0