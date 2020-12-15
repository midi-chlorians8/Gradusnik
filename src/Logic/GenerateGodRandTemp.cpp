#include <Arduino.h>
#include "GenerateGodRandTemp.h"
void GenerateGodRandTemp() {
  // RandGoodResult = random(36.6, 36.8);
  // Делаем случ знач 36.5-36.7
  int IntRandGoodResult = analogRead(A1) & 3;//random(1, 4);
  switch (IntRandGoodResult ) {
    case 0:
      RandGoodResult = 36.5;
      break;
    case 1:
      RandGoodResult = 36.6;
      break;
    case 2:
      RandGoodResult = 36.6;
      break;
    case 3:
      RandGoodResult = 36.7;
      break;

  }

  // Serial.print("IntRandGoodResult:");Serial.println(IntRandGoodResult); //Del!
}