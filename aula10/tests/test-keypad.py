import time
from libs_freenove import Keypad

ROWS = 4
COLS = 4
keys =  [   '1','2','3','A',
            '4','5','6','B',
            '7','8','9','C',
            '*','0','#','D'     ]
rowsPins = [16, 20, 21, 26]
colsPins = [19, 13, 6, 5]

def validar_teclado():
    keypad = Keypad.Keypad(keys, rowsPins, colsPins, ROWS, COLS)
    
    keypad.setDebounceTime(50)
    
    senha_buffer = ""
    LIMITE_DIGITOS = 6
    
    print("==================================================")
    print("      TESTE ISOLADO: TECLADO MATRICIAL            ")
    print("==================================================")
    print(f"-> Digite a senha (Máximo de {LIMITE_DIGITOS} dígitos).")
    print("-> Pressione '*' para apagar (Backspace).")
    print("-> Pressione '#' para submeter a senha.")
    print("==================================================\n")
    
    while(True):
        key = keypad.getKey()
        
        if(key != keypad.NULL):
            if key == '#':
                print(f"[EVENTO] Senha submetida para processamento: {senha_buffer}")
                senha_buffer = ""
                
            elif key == '*':
                if len(senha_buffer) > 0:
                    senha_buffer = senha_buffer[:-1]
                    print(f"[EVENTO] Backspace acionado. Buffer atual: {senha_buffer if senha_buffer else '(vazio)'}")
                else:
                    print("[AVISO] O buffer já está vazio.")
                    
            else:
                if len(senha_buffer) < LIMITE_DIGITOS:
                    senha_buffer += key
                    print(f"[EVENTO] Tecla '{key}' registrada. Buffer atual: {senha_buffer}")
                else:
                    print(f"[AVISO] Limite de {LIMITE_DIGITOS} dígitos atingido! Submeta (#) ou apague (*).")

if __name__ == '__main__':
    try:
        validar_teclado()
    except KeyboardInterrupt:
        print("\nEncerramento forçado. Finalizando teste do teclado.")