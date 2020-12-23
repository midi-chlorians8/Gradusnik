#include <Arduino.h>
#include "CalibrPcPrint.h"
#include "Calman.h"
#include <Adafruit_MLX90614.h>

extern Adafruit_MLX90614 mlx;
void CalibrPcPrint() { //Вывод на дисплей калибровочных данных (Исходн знач температуры + калибр значение с потенциометра)
  // Чтение данных с датчика. Прибавка к ним калибр знач. .Отправка на пк температуры [Калибр] режим
   Serial.print("D");Serial.print(   filter(mlx.readObjectTempC() + PotVal)   ); Serial.print(" "); Serial.println();
  // Чтение данных с датчика. Прибавка к ним калибр знач. .Отправка на пк температуры [Калибр] режим
}