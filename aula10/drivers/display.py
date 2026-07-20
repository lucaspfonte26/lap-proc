try:
    from ..libs_freenove.LCD1602 import CharLCD1602
except ImportError:
    from libs_freenove.LCD1602 import CharLCD1602


class DisplayDriver:
    def __init__(self):
        self.lcd = CharLCD1602()

    def init(self):
        self.lcd.init_lcd()

    def formatar_linha(self, texto):
        """Garante que a linha tenha exatos 16 caracteres para limpar resíduos no display."""
        return texto.ljust(16)

    def atualizar(self, linha0, linha1):
        self.lcd.write(0, 0, self.formatar_linha(linha0))
        self.lcd.write(0, 1, self.formatar_linha(linha1))

    def clear(self):
        self.lcd.clear()
