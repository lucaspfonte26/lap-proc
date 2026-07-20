try:
    from ..libs_freenove import Keypad
except ImportError:
    from libs_freenove import Keypad


class KeypadDriver:
    def __init__(self):
        ROWS, COLS = 4, 4
        keys = [
            "1",
            "2",
            "3",
            "A",
            "4",
            "5",
            "6",
            "B",
            "7",
            "8",
            "9",
            "C",
            "*",
            "0",
            "#",
            "D",
        ]
        rowsPins = [16, 20, 21, 26]
        colsPins = [19, 13, 6, 5]

        self.keypad = Keypad.Keypad(keys, rowsPins, colsPins, ROWS, COLS)
        self.keypad.setDebounceTime(50)
        self.NULL = self.keypad.NULL

    def get_key(self):
        return self.keypad.getKey()
