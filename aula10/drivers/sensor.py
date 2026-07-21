from gpiozero import DistanceSensor

class SensorDriver:
    def __init__(self, echo=15, trigger=14, max_distance=3):
        self.sensor = DistanceSensor(echo=echo, trigger=trigger, max_distance=max_distance)
        self.LIMIAR_PORTA_FECHADA_CM = 15.0

    @property
    def distance_cm(self):
        """Retorna a distância lida em centímetros."""
        return self.sensor.distance * 100

    def is_closed(self):
        return self.distance_cm <= self.LIMIAR_PORTA_FECHADA_CM

    def close(self):
        self.sensor.close()