# esp32ble-controller
## What is this?
This is my Personal Project to make me lazier while watching Light Novel or Manga.
With this you dont have to swipe/touch your stupidly big screen/keyboard anymore. 
## Required for the project

* ESP32
* 3 Buttons
* 2 10k resistor
* Battery
* Breadboard or PCB

## Default ESP32 pin used 
Right Button pin **34**

Left Button pin **25**

Modes Button pin **23**

IF you change any of the Left Right Button you need to **change** BUTTON_PIN_BITMASK to have deep sleep working.
## Features
* Works with almost all operating system with Bluetooth
* Can Change preset on the fly with serial monitor when plugged in with USB 

## Help
### Why doesn’t it deep sleep and wake up instantly?
This is most likely you used **INPUT_PULLUP** instead of external pullup. **INPUT_PULLUP** is disable when deepsleep so you are required to use deepsleep.
