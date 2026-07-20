from gpiozero import TonalBuzzer
from gpiozero.tones import Tone
import time

class BuzzerDriver:
    def __init__(self, pin=4):
        self.buzzer = TonalBuzzer(pin)
        self.desliga_em = 0
        self.estado = False

    def play(self, frequencia, duracao_segundos):
        self.buzzer.play(Tone(frequencia))
        self.desliga_em = time.time() + duracao_segundos
        self.estado = True

    def update(self):
        if self.estado and time.time() >= self.desliga_em:
            self.buzzer.stop()
            self.estado = False

    def close(self):
        self.buzzer.close()