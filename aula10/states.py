from enum import Enum, auto

class LockState(Enum):
    IDLE = auto()
    INPUT = auto()
    SUCCESS = auto()
    FAILURE = auto()
    ALARM = auto()
    COOLDOWN = auto()