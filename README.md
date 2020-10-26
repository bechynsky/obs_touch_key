# OBS Studio touch control

Arduino with touch TFT display to control [OBS Studio](https://obsproject.com/) scenes is easy way how to add more comfort to your broadcasting or screen capture projects. Goal is to create cheap and portable device.

# Hardware

Arduino with [Keyboard library](https://www.arduino.cc/reference/en/language/functions/usb/keyboard/) support. I use [Arduino Leonardo](https://www.arduino.cc/en/Main/Arduino_BoardLeonardo) with ATmega32u4.

TFT Touch Shield for Arduino. I use [2.8" TFT Touch Shield for Arduino w/Capacitive Touch](https://www.adafruit.com/product/1947). You can use what is available for you. You will need different background picture if resolution will is not 240x320 pixels. I like this sice it is same size as Arduino so it fits nice.

SD card to store background bitmap. I use very old one just 1 GB formated with FAT file system. SD card reader is part of TFT shield.

# Software
Software jsut draw bitmap image from SD card. It listens for touch event. Based on coordinates of touch event is sends key strokes and highlight virtual button. Be aware there is no feedback from OBS. It just sends keyboard shortcuts.

# Disadvanteges
There is no feedback from touch screen compare to physical buttons like in this [OBS Control Rig](https://github.com/ridercz/ObsControl) project.
