/*
  Blink

  Turns the on-board RGB LED yellow for one second, then off for one second,
  repeatedly.

  On the ESP32-C3 SuperMini the on-board LED is an RGB WS2812 on GPIO 8 (macro
  LED_BUILTIN). It is driven via rgbLedWrite(pin, R, G, B), available natively
  in the arduino-esp32 core >= 3.0 (no external library needed).
  If you want to know what pin the on-board LED is connected to on your board,
  check the Technical Specs of your board at:
  https://docs.arduino.cc/hardware/

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman
  adapted 26 May 2026 for ESP32-C3 SuperMini RGB LED (yellow)

  This example code is in the public domain.

  https://docs.arduino.cc/built-in-examples/basics/Blink/
*/

// the setup function runs once when you press reset or power the board
void setup() {
  // no pinMode needed: rgbLedWrite() initializes LED_BUILTIN on first call.
}

// the loop function runs over and over again forever
void loop() {
  rgbLedWrite(LED_BUILTIN, 64, 64, 0);  // turn the LED yellow (R=G, B=0)
  delay(1000);                          // wait for a second
  rgbLedWrite(LED_BUILTIN, 0, 0, 0);    // turn the LED off
  delay(1000);                          // wait for a second
}
