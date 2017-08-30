from machine import Pin


class led(object):
    LED_DICT = {0: 0, 1: 2, 2: 16}

    def __init__(self, id):
        try:
            self.pin = self.LED_DICT[id]
        except KeyError:
            raise "This LED does not exist"
        self.led = Pin(self.pin, Pin.OUT, 1)

    def on(self):
        self.led.value(0)

    def off(self):
        self.led.value(1)
