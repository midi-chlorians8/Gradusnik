
#pragma once
// Дисплей 0.96 st7735
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
//#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#define TFT_CS        10
#define TFT_RST        9 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         8


extern Adafruit_ST7735 tft;// = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
//int Variable1;  // Create a variable to have something dynamic to show on the display

// Дисплей 0.96 st7735