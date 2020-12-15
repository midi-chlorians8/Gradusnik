
#include "Calman.h"

// переменные для калмана (Температура конечная)
float varVolt = 0.25;  // среднее отклонение (ищем в excel)
float varProcess = 0.005; // скорость реакции на изменение (подбирается вручную)
float Pc = 0.0;
float G = 0.0;
float P = 1.0;
float Xp = 0.0;
float Zp = 0.0;
float Xe = 0.0;
// переменные для калмана(Температура конечная)

float filter(float val) {  //функция фильтрации
  Pc = P + varProcess;
  G = Pc / (Pc + varVolt);
  P = (1 - G) * Pc;
  Xp = Xe;
  Zp = Xp;
  Xe = G * (val - Zp) + Xp; // "фильтрованное" значение
  return (Xe);
}