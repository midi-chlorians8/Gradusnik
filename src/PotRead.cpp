#include <Arduino.h>
#include "PotRead.h"
void PotRead() {
  // Потенциометр чтение и обработка. Для подстройки под температуру тела
  int sensorValue = analogRead(A0); // Чтение потенциометра
  sensorValue = map(sensorValue, 0, 1023, 0, 44);
  PotVal = float(sensorValue) / 10;
  //PotVal=filter2(PotVal);
  // Потенциометр чтение и обработка. Для подстройки под температуру тела
}