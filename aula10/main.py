import time
import warnings

try:
    from . import security
    from .drivers.display import DisplayDriver
    from .drivers.keypad import KeypadDriver
    from .drivers.sensor import SensorDriver
    from .drivers.buzzer import BuzzerDriver
    from .states import LockState
except ImportError:
    import security
    from drivers.display import DisplayDriver
    from drivers.keypad import KeypadDriver
    from drivers.sensor import SensorDriver
    from drivers.buzzer import BuzzerDriver
    from states import LockState

warnings.filterwarnings("ignore")

LIMITE_DIGITOS = 6


def main():
    controller = ElectronicLockController()
    try:
        controller.run()
    except KeyboardInterrupt:
        controller.cleanup_hardware()
        print("\nSistema encerrado com segurança.")


class ElectronicLockController:
    def __init__(self):
        self.estado_atual = LockState.IDLE
        self.senha_buffer = ""
        self.tempo_transicao_estado = 0
        self.tempo_ultima_atualizacao_lcd = 0

        self.display = DisplayDriver()
        self.keypad = KeypadDriver()
        self.sensor = SensorDriver()
        self.buzzer = BuzzerDriver()

    def run(self):
        self.setup_hardware()

        while True:
            agora = time.time()

            self.buzzer.update()

            porta_fechada = self.sensor.is_closed()

            if not porta_fechada and self.estado_atual not in [
                LockState.SUCCESS,
                LockState.ALARM,
            ]:
                self.estado_atual = LockState.ALARM
                self.display.atualizar("ALERTA VIOLACAO!", "PORTA ARROMBADA!")
                self.buzzer.play(220.0, 5.0)

            if self.estado_atual == LockState.IDLE:
                key = self.keypad.get_key()
                if key != self.keypad.NULL:
                    self.estado_atual = LockState.INPUT
                    self.senha_buffer = ""
                    self.processar_tecla(key)

            elif self.estado_atual == LockState.INPUT:
                key = self.keypad.get_key()
                if key != self.keypad.NULL:
                    self.processar_tecla(key)

            elif self.estado_atual in [LockState.SUCCESS, LockState.FAILURE]:
                if agora >= self.tempo_transicao_estado:
                    if security.esta_bloqueado():
                        self.estado_atual = LockState.COOLDOWN
                    elif porta_fechada:
                        self.estado_atual = LockState.IDLE
                        self.display.atualizar("STATUS: TRANCADA", "Aguardando...")
                    else:
                        self.display.atualizar("STATUS: ABERTA", "Feche a porta...")

            elif self.estado_atual == LockState.COOLDOWN:
                if not security.esta_bloqueado():
                    self.estado_atual = LockState.IDLE
                    self.display.atualizar("SISTEMA REATIVADO", "STATUS: TRANCADA")
                    print("[SISTEMA] Cooldown finalizado. Teclado liberado.")
                else:
                    if agora - self.tempo_ultima_atualizacao_lcd >= 1.0:
                        tempo_restante = security.tempo_restante_cooldown()
                        self.display.atualizar(
                            "SISTEMA BLOQUEADO", f"Aguarde: {tempo_restante}s"
                        )
                        self.tempo_ultima_atualizacao_lcd = agora

                    _ = self.keypad.get_key()

    def setup_hardware(self):
        """Inicializa o display e apresenta a tela de boot."""
        self.display.init()
        self.display.atualizar("SISTEMA SEGURO", "STATUS: TRANCADA")
        time.sleep(1)

    def cleanup_hardware(self):
        """Limpa o display e libera as portas GPIO na saída."""
        self.display.clear()
        self.buzzer.close()
        self.sensor.close()

    def exibir_ofuscacao(self):
        self.display.atualizar(
            "DIGITE A SENHA", "Senha: " + "*" * len(self.senha_buffer)
        )

    def processar_tecla(self, key):
        agora = time.time()

        if key == "#":
            if security.verificar_senha(self.senha_buffer):
                self.estado_atual = LockState.SUCCESS
                self.display.atualizar("ACESSO LIBERADO", "Bem-vindo!")
                self.buzzer.play(440.0, 0.2)
                self.tempo_transicao_estado = agora + 3.0
            else:
                self.estado_atual = LockState.FAILURE
                self.display.atualizar("ACESSO NEGADO!", "Senha Incorreta")
                self.buzzer.play(220.0, 0.5)
                self.tempo_transicao_estado = agora + 2.0

        elif key == "*":
            if len(self.senha_buffer) > 0:
                self.senha_buffer = self.senha_buffer[:-1]
                self.exibir_ofuscacao()
        else:
            if len(self.senha_buffer) < LIMITE_DIGITOS:
                self.senha_buffer += key
                self.exibir_ofuscacao()
