# HeadLED

This is the Arduino program for the HeadLed box.

The program has three important parts that are separated

- check buttons --> void checkButtons()
- update state --> void updateState()
- run animations --> executed in main loop, void loop()
The code is divided into two parts that are clearly marked, and can be found in the following order

Controll code
Led animation code

Check list for adding new animations:

Increase the value of int modesChoiceMax
Add mode in void loop()
Use pause() instead of delay() as pause() checks buttons while pausing.

For an in depth toutorial on how to use and program the adressable LEDs, see adafruits guide: https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library
