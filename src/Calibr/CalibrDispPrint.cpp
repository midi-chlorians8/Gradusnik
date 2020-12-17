#include <Arduino.h>

#include "CalibrDispPrint.h"

#include "Display.h"
#include "Calman.h"
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735

#include <Adafruit_MLX90614.h>

extern Adafruit_MLX90614 mlx;

void CalibrDispPrint() {          // Вывод на дисплей калибровочных данных (Исходн знач температуры + калибр значение с потенциометра)
if(TubmblerDebugDispClean == false){ // Разок очистить дисплей
  tft.fillScreen(ST77XX_BLACK);
  TubmblerDebugDispClean = true;
}
  // Если значение обновилось - разрешить вывод на экран [Калибр] режим
  if (OldValueTemp !=  filter(mlx.readObjectTempC() + PotVal)   ) {

    // Вывод на экран [Калибр] режим
    char string[10];  // Create a character array of 10 characters
    // Convert float to a string:
    dtostrf( filter(mlx.readObjectTempC() + PotVal), 3, 1, string); // (<variable>,<amount of digits we are going to use>,<amount of decimal digits>,<string name>)

    tft.setCursor(81 - 60, 67 - 45); // Set position (x,y)
    tft.setTextColor(ST7735_GREEN, ST7735_BLACK);  // Set color of text. First is the color of text and after is color of background
    tft.setTextSize(5);  // Set text size. Goes from 0 (the smallest) to 20 (very big)
    tft.println(string);  // Print a text or value

    tft.fillRect(0, 90, 55, 34, ST7735_BLACK);  // Draw filled rectangle (x,y,width,height,color)

    // Вывод на экран [Калибр] режим
  }

  OldValueTemp =  filter(mlx.readObjectTempC() + PotVal) ; // Ложим значение текущее в старое
  // Если значение обновилось - разрешить вывод на экран [Калибр] режим
}