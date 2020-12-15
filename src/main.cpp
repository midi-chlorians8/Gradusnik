// 10.12.2020
// Романовский И С
// Чтение датчика MLX90614 BCC купленного в чип и дипе
// Добавлена подстройка с помощью потенциометра
// Добавка фильтра Калмана
// Добавка дисплея 0,96 st7735

// To-do
// Сделать вывод на экран +
// Cделать обновление экрана только при вводе нового значения +
// Cделать 2 режима отладка (когда просто на экран сыпит цифрами, и рабочий (36.5,36.6,36.7) Температура повышена
// Проверить работу задержки перед считыванием температуры
// Есть баг. Когда горит красный светодиод. Мы отдаляем плавно. И начинает гореть зелёный. Это баг. Надо так. При выходе из зоны повышенной температуры очистить экран на 2 сек

#include <Arduino.h>
// Датчик
#include <Wire.h>
#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
//extern Adafruit_MLX90614 mlx;
float OldValueTemp; // Прошлое значение температуры чтобы слишком часто не обновлять экран
// Датчик

#include "Calman.h" // Тут лежат переменные для фильтра калмана

#include "Display.h"// Тут лежат пины к которым подключен дисплей и создан экземляр дисплея 
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST); // Тут создаём экземпляр класса чтобы избежать двойного обьявления

// Ввод режима и настройки
#define PinTumbler 4 // Пин к переключателю Рабочий режим - тест режим
float PotVal; // Потенциометр
// Ввод режима и настройки

// Cчитывание температуры
float RandGoodResult; // Переменная для Хорошей температуры (случайного числа)
unsigned long timingDelayBetweenTempCheck; // Переменная могает сделать задержку перед первым считыванием температуры
bool IsBeDelay=false; // Буль сигнализирует была ли в задержка в секунду+- посде появления руки
bool BeginOtchet; //Для того чтоб при поднесённой руке не сразу был замер. Актуально если температурник подносит руку
// Cчитывание температуры

#include "LightDiodes.h" // Cдесь хранятся пины к которым подключены светодиоды
#include "PotRead.h"
#include "CalibrDispPrint.h"
#include "CalibrPcPrint.h"
#include "GenerateGodRandTemp.h"

void setup() {

  Serial.begin(9600);

  mlx.begin(); // Запуск чтения датчика

  pinMode(PinTumbler, INPUT_PULLUP); // Для выбора режима Отладка - Работа

  // Дисплей 0.96 st7735
  tft.initR(INITR_MINI160x80);  // Init ST7735S mini display
  tft.setRotation(3);
  tft.fillScreen(ST7735_BLACK);  // Fill screen with black
  tft.setTextWrap(false);
  tft.setFont();
  // Дисплей 0.96 st7735

  // Светодиоды
  pinMode(GreenDiodPin,OUTPUT);
  pinMode(YellowDiodPin,OUTPUT);
  pinMode(RedDiodPin,OUTPUT);
  // Светодиоды

}

void DrawDisplay(String abc){ // Отрисовочка дисплея
    if(abc == "clean"){ // Если пошла команда на очистку экрана
        // Очистить дисплей
        Serial.println("clean");
        tft.setCursor(81 - 75, 67 - 45); // Set position (x,y)
        tft.setTextColor(ST7735_GREEN, ST7735_BLACK);  // Set color of text. First is the color of text and after is color of background
        tft.setTextSize(5);  // Set text size. Goes from 0 (the smallest) to 20 (very big)
        tft.println("      ");  // Print a text or value
        tft.fillRect(0, 90, 85, 54, ST7735_BLACK);  // Draw filled rectangle (x,y,width,height,color)
        // Очистить дисплей
    }
     else if(abc == "HighTemp"){
       Serial.println("HighTemp");
        // tft.fillScreen(ST77XX_BLACK);
        tft.setCursor(81 - 75, 67 - 43); // Set position (x,y)
        tft.setTextColor(ST7735_GREEN, ST7735_BLACK);  // Set color of text. First is the color of text and after is color of background
        tft.setTextSize(4);  // Set text size. Goes from 0 (the smallest) to 20 (very big)
        tft.println("H temp");  // Print a text or
    }
    else{ //36.5 - 36.7
          // Вывод на экран [Рабочий] режим
          tft.setCursor(81 - 60, 67 - 45); // Set position (x,y)
          tft.setTextColor(ST7735_GREEN, ST7735_BLACK);  // Set color of text. First is the color of text and after is color of background
          tft.setTextSize(5);  // Set text size. Goes from 0 (the smallest) to 20 (very big)
          tft.println(abc);  // Print a text or
          // Вывод на экран [Рабочий] режим
    }

}

void LogicTemp(){ // Блок принимает решение на основе считанной температуры
   // Блок принимает решение на основе считанной температуры [Если нет ошибки]
    static bool OneRazRuka = false; // Буль нужен чтобы при подносе здоровой руки один раз выбралось одно из загатовленных значений
    static int8_t OneRazRedRuka = 0; //  чтобы один раз картинку очистить и вывести что температура повышена

    if (  filter(mlx.readObjectTempC() + PotVal) > 32 && filter(mlx.readObjectTempC() + PotVal) < 37.5 ) { // Если поднесли руку и чел не горит от температуры
        digitalWrite(GreenDiodPin,HIGH);// Засветить зелёной лампой
        //digitalWrite(YellowDiodPin,LOW);// Погасить  жёлтую  лампу
        digitalWrite(RedDiodPin,LOW);   // Погасить  красную лампу

      if (OneRazRuka == false) { // Выбрать одно из 3х случайных значений 36.6 36.7 36.8 ОДИН раз
          OneRazRedRuka = 0;
          GenerateGodRandTemp(); // Cоздаём случайное число

          char string[10];  // Create a character array of 10 characters
          // Convert float to a string:
          dtostrf( RandGoodResult, 3, 1, string); // (<variable>,<amount of digits we are going to use>,<amount of decimal digits>,<string name>)
          DrawDisplay(string); // Вывод 36.5 -36.7  // Вывод на экран [Рабочий] режим

          OneRazRuka = true;
      }
    }
    if ( filter(mlx.readObjectTempC() + PotVal) >= 37.5 && filter(mlx.readObjectTempC() + PotVal) < 41 ) { // Если чел сгорает от температуры
      // Засветить красной лампой
      digitalWrite(RedDiodPin,HIGH); //Зажечь красный диод
      digitalWrite(GreenDiodPin,LOW);// Засветить зелёной лампой
      // Вывести на дисплей инфу = повышена температура
              
        
       if (OneRazRedRuka == 0) {
          DrawDisplay("clean"); // Очистить дисплей
          OneRazRedRuka = 1;
       }
          if (OneRazRedRuka == 1) {
          DrawDisplay("HighTemp"); // Очистить дисплей
          OneRazRedRuka = 2;
       }
       
        
      // Вывести на дисплей инфу = повышена температура
      // Отправить команду на PC о выводе повышенной температуры
    }

    if ( filter(mlx.readObjectTempC() + PotVal) >= 10 && filter(mlx.readObjectTempC() + PotVal) <= 32 ) { //Если убрали руку
      digitalWrite(GreenDiodPin,LOW);// Погасить зелёную лампой
      digitalWrite(RedDiodPin,LOW);  // Погасить красную лампой
      
      OneRazRuka = false;
      OneRazRedRuka = 0;

      IsBeDelay = false; // Перезарядить начало задержки первоначального считывания при убранной руке
      BeginOtchet = false;
      
      DrawDisplay("clean"); // Очистить дисплей
    }

    // Блок принимает решение на основе считанной температуры [Если нет ошибки]
}

void loop() {
  // put your main code here, to run repeatedly:
  PotRead(); // Чтение потенциометра и перевод в удобные 0.1 для калибровки

  if (digitalRead(PinTumbler) == LOW) { // Если тумблер в положении калибровка
    CalibrDispPrint(); // Вывод на дисплей калибровочных данных (Исходн знач температуры + калибр значение с потенциометра)
    CalibrPcPrint();
  }
  else { // Если тумблер в положении Рабочий режим
    
    //Если ошибка
    if ( ( filter(mlx.readObjectTempC() + PotVal) > -9999 && filter(mlx.readObjectTempC() + PotVal) < 0 )   || ( filter(mlx.readObjectTempC() + PotVal)>41  && filter(mlx.readObjectTempC() + PotVal) <9999 ) ) { //
        digitalWrite(YellowDiodPin,HIGH);  // Засветить жёлтой лампой
        digitalWrite(GreenDiodPin,LOW);// Погасить зелёную лампой
        digitalWrite(RedDiodPin,LOW);  // Погасить красную лампой
        DrawDisplay("clean"); // Очистить дисплей
     
    }
    //Если ошибка

    // А если нет ошибки
      else{
        digitalWrite(YellowDiodPin,LOW);
        // Старт точка отсчёта
        if(BeginOtchet == false){
          timingDelayBetweenTempCheck = millis();
          BeginOtchet = true;
        }
        // Старт точка отсчёта
        // Если температура измеряемая датчиком говорит что рука поднесена, надо выждать секунду, а затем принимать решение что делать, если это не ситуация когда у нас ошибка
            if(IsBeDelay == false && filter(mlx.readObjectTempC() + PotVal) > 32 ){ // Если задержки ещё небыло и рука была поднесена
               if (millis() - timingDelayBetweenTempCheck > 1000){ // Вместо 10000 подставьте нужное вам значение паузы 
                  //timingDelayBetweenTempCheck = millis(); 
                  Serial.println ("1 seconds Delay afther Ruka");
                  IsBeDelay = true;
               }
            }        
        // Если температура измеряемая датчиком говорит что рука поднесена, надо выждать секунду, а затем принимать решение что делать, если это не ситуация когда у нас ошибка
          if(IsBeDelay == true){ // Если была задержка в одну секунду после начала поднесения руки
          LogicTemp(); // Блок принимает решение на основе считанной температуры
          
        }
      }
    // А если нет ошибки

  } // Если тумблер в положении Рабочий режим
}