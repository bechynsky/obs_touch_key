// Adafruit_ImageReader test for Adafruit ILI9341 TFT Shield for Arduino.
// Demonstrates loading images from SD card or flash memory to the screen,
// to RAM, and how to query image file dimensions.
// Requires three BMP files in root directory of SD card:
// purple.bmp, parrot.bmp and wales.bmp.
// As written, this uses the microcontroller's SPI interface for the screen
// (not 'bitbang') and must be wired to specific pins (e.g. for Arduino Uno,
// MOSI = pin 11, MISO = 12, SCK = 13). Other pins are configurable below.

#include <Adafruit_GFX.h>         // Core graphics library
#include <Adafruit_ILI9341.h>     // Hardware-specific library
#include <SdFat.h>                // SD card & FAT filesystem library
#include <Adafruit_SPIFlash.h>    // SPI / QSPI flash library
#include <Adafruit_ImageReader.h> // Image-reading functions

// Touch
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_FT6206.h>

// Keyboard simulator
#include "Keyboard.h"

// The FT6206 uses hardware I2C (SCL/SDA)
Adafruit_FT6206 ts = Adafruit_FT6206();

// TFT display and SD card share the hardware SPI interface, using
// 'select' pins for each to identify the active device on the bus.
#define SD_CS   4 // SD card select pin
#define TFT_CS 10 // TFT select pin
#define TFT_DC  9 // TFT display/command pin

SdFat                SD;         // SD card filesystem
Adafruit_ImageReader reader(SD); // Image-reader object, pass in SD filesys

Adafruit_ILI9341     tft    = Adafruit_ILI9341(TFT_CS, TFT_DC);

// buttons
#define THICKNESS 14

// record butons
#define BUTTON_RECORD_X 0
#define BUTTON_RECORD_W 119

#define BUTTON_RECORD_1_Y 0
#define BUTTON_RECORD_1_H 158

#define BUTTON_RECORD_2_Y 162
#define BUTTON_RECORD_2_H 158

// scene buttons
#define BUTTON_SCENE_X 121
#define BUTTON_SCENE_W 119

#define BUTTON_1_Y 0
#define BUTTON_1_H 106

#define BUTTON_2_Y 108
#define BUTTON_2_H 104

#define BUTTON_3_Y 214
#define BUTTON_3_H 106

// colors
// http://www.barth-dev.de/online/rgb565-color-picker/
#define BACKGROUND              ILI9341_BLACK
#define ACTIVE_SCENE            ILI9341_GREEN

// recording buttons
#define BUTTON_RECORD_1_ACTIVE  0x3E60
#define BUTTON_RECORD_2_ACTIVE  0xC800

// scene buttons
#define BUTTON_1_ACTIVE         0xA014
#define BUTTON_2_ACTIVE         0x045F
#define BUTTON_3_ACTIVE         0xFF20

bool is_recording = false;

// x, y - top left corner
// w, h - size
// c - color
// t - thickness of border
void drawBox(int x, int y, int w, int h, uint16_t c, byte t) {
  for (byte i = 0; i < t; i++) {
       tft.drawRect(x + i,  y + i, w - (i * 2), h - (i * 2), c);
  }
}

void clearBoxes() {
  drawBox(BUTTON_SCENE_X, BUTTON_1_Y, BUTTON_SCENE_W, BUTTON_1_H, BACKGROUND, THICKNESS);
  drawBox(BUTTON_SCENE_X, BUTTON_2_Y, BUTTON_SCENE_W, BUTTON_2_H, BACKGROUND, THICKNESS);
  drawBox(BUTTON_SCENE_X, BUTTON_3_Y, BUTTON_SCENE_W, BUTTON_3_H, BACKGROUND, THICKNESS);
}

void setup(void) {
  ImageReturnCode stat; // Status from image-reading functions

  tft.begin();          // Initialize screen

  if (!ts.begin(40)) { 
    for(;;); 
    //Serial.println("Unable to start touchscreen.");
  } 
  else { 
    //Serial.println("Touchscreen started."); 
  }

  // The Adafruit_ImageReader constructor call (above, before setup())
  // accepts an uninitialized SdFat or FatFileSystem object. This MUST
  // BE INITIALIZED before using any of the image reader functions!
  if(!SD.begin(SD_CS)) { // ESP32 requires 25 MHz limit
    //Serial.println(F("SD begin() failed"));
    for(;;); // Fatal error, do not continue
  }
  
  // Load full-screen BMP file 'purple.bmp' at position (0,0) (top left).
  // Notice the 'reader' object performs this, with 'tft' as an argument.
  stat = reader.drawBMP("/obs_inactive.bmp", tft, 0, 0);
  reader.printStatus(stat);   // How'd we do?

  Keyboard.begin();
  
  delay(2000); // Pause 2 seconds before moving on to loop()
}

void loop() {
  if (ts.touched())
  {   
    // Retrieve a point  
    TS_Point p = ts.getPoint(); 
    
    p.x = map(p.x, 0, 240, 240, 0);
    p.y = map(p.y, 0, 320, 320, 0);

    if (p.x > BUTTON_SCENE_X)
    {
      if (p.y < BUTTON_2_Y)
      {
        clearBoxes();
        drawBox(BUTTON_SCENE_X, BUTTON_1_Y, BUTTON_SCENE_W, BUTTON_1_H, BUTTON_1_ACTIVE, THICKNESS);
        //Keyboard.println("A");
      }
      else if (p.y > BUTTON_2_Y && p.y < BUTTON_3_Y)
      {
        clearBoxes();
        drawBox(BUTTON_SCENE_X, BUTTON_2_Y, BUTTON_SCENE_W, BUTTON_2_H, BUTTON_2_ACTIVE, THICKNESS);
        //Keyboard.println("B");
      }
      else if (p.y > BUTTON_3_Y)
      {
        clearBoxes();
        drawBox(BUTTON_SCENE_X, BUTTON_3_Y, BUTTON_SCENE_W, BUTTON_3_H, BUTTON_3_ACTIVE, THICKNESS);
        //Keyboard.println("C");
      }
    }
    else
    {
      if (p.y < BUTTON_RECORD_2_Y)
      {
        if (is_recording) {
          drawBox(BUTTON_RECORD_X, BUTTON_RECORD_1_Y, BUTTON_RECORD_W, BUTTON_RECORD_1_H, BACKGROUND, THICKNESS);
          //Keyboard.println("R");
        } else {
          drawBox(BUTTON_RECORD_X, BUTTON_RECORD_1_Y, BUTTON_RECORD_W, BUTTON_RECORD_1_H, BUTTON_RECORD_1_ACTIVE, THICKNESS);
          //Keyboard.println("R");
        }
      }
      else
      {
        if (is_recording) {
          drawBox(BUTTON_RECORD_X, BUTTON_RECORD_2_Y, BUTTON_RECORD_W, BUTTON_RECORD_2_H, BACKGROUND, THICKNESS);
          //Keyboard.println("R");
        } else {
          drawBox(BUTTON_RECORD_X, BUTTON_RECORD_2_Y, BUTTON_RECORD_W, BUTTON_RECORD_2_H, BUTTON_RECORD_2_ACTIVE, THICKNESS);
          //Keyboard.println("R");
        }
      }
      is_recording = !is_recording;
    }
    // just one touch
    while(ts.touched()) {delay(10);};
  }
  
}
