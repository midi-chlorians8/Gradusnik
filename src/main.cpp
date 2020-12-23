// 10.12.2020 - 18.12.2020
// Романовский И С
// Чтение датчика MLX90614 BCC купленного в чип и дипе(Беларусь).

// Df Player
// mp3_play (1); // Температура в норме
// mp3_play (2); // Температура повышена
// mp3_play (3); // Ошибка

// To-do
// Cделать так чтобы не только звук от зелёного диода был, но и от красного и от жёлтого. +
// Добавить выход под наушнички +
// Допаять транзистор светодиоды +

// В данный момент мы подводим к горячей кружке = H Temp - красный; Отдаляем = 36.6 - зелёный; Приближаем = H Temp - красный; Отдаляем = _пустой дисплей_ - зелёный;   
// А надо:  H Temp - красный; отдаляем до зелёного и нет никакого зелёного и надписей на экране
// И для этого нужно сделать: Из красного режима, переход в зелёный или жёлтый, только при попадании в зону [рука убрана] 



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

bool TubmblerDebugDispClean = false;

float BodyTemp = 0; // Температура тела (датчик + потенциометр)

bool RedActivate = false; // Если активирован красный уровень
bool YellowActivate = false; // Если активирован жёлтый уровень


// DF Player
#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>
SoftwareSerial mySerial(9, 7); // RX, TX
// DF Player


//#define PodstavkaTemp // Раскомментировать для отладки . Замена датчика на ввод с клавиатуры
bool OneRazYellowDangerSay=false; // Чтоб только один раз воспроизвёлося звук ошибка
#ifdef PodstavkaTemp
String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
float InptuF=0;

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
 if (stringComplete) {
    Serial.println(inputString);
    
    if(inputString=="x\n"){
      Serial.print("BodyTemp:");Serial.println(BodyTemp);
      Serial.print("BeginOtchet:");Serial.println(BeginOtchet);
      Serial.print("IsBeDelay:");Serial.println(IsBeDelay);
      Serial.print("timingDelayBetweenTempCheck:");Serial.println(timingDelayBetweenTempCheck);
    }
    else{
      InptuF = inputString.toFloat();
      Serial.print("InptuF:");Serial.println(InptuF);
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
  
}
#endif

#define ComputerVersion

void setup() {

  Serial.begin(9600);

    // DF Player
  mySerial.begin (9600);
	mp3_set_serial (mySerial);	//set softwareSerial for DFPlayer-mini mp3 module 
	mp3_set_volume (30);
  // DF Player
  /*
  delay(500);
  mp3_play (1);
  delay(500);
  */

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
        //Serial.println("clean");
        tft.setCursor(81 - 75, 67 - 45); // Set position (x,y)
        tft.setTextColor(ST7735_GREEN, ST7735_BLACK);  // Set color of text. First is the color of text and after is color of background
        tft.setTextSize(5);  // Set text size. Goes from 0 (the smallest) to 20 (very big)
        tft.println("      ");  // Print a text or value
        tft.fillRect(0, 90, 85, 54, ST7735_BLACK);  // Draw filled rectangle (x,y,width,height,color)
        tft.fillScreen(ST77XX_BLACK);
        // Очистить дисплей
    }
     else if(abc == "HighTemp"){
       //Serial.println("HighTemp");
        // tft.fillScreen(ST77XX_BLACK);



        //tft.setCursor(81 - 75, 67 - 43); // Set position (x,y)
         tft.setCursor(81 - 60, 67 - 45); // Set position (x,y)
        tft.setTextColor(ST7735_BLUE, ST7735_BLACK);  // Set color of text. First is the color of text and after is color of background
        //tft.setTextSize(4);  // Set text size. Goes from 0 (the smallest) to 20 (very big)
        tft.setTextSize(5);
        //tft.println("H temp");  // Print a text or
        //Serial.println(String(BodyTemp));Serial.println(" ");
        tft.println(String(BodyTemp,1));
        Serial.println(String(BodyTemp));Serial.println(" ");
    }
    else{ //36.5 - 36.7
          // Вывод на экран [Рабочий] режим
          tft.setCursor(81 - 60, 67 - 45); // Set position (x,y)
          tft.setTextColor(ST7735_GREEN, ST7735_BLACK);  // Set color of text. First is the color of text and after is color of background
          tft.setTextSize(5);  // Set text size. Goes from 0 (the smallest) to 20 (very big)
          tft.println(abc);  // Print a text or
          Serial.println(String(abc));Serial.println(" "); // Посыклка 36.5 36.6 36.7
          
          //Serial.println("Ataka");Serial.println(" ");//Serial.println(String(abc));Serial.println("Ataka");
          // Вывод на экран [Рабочий] режим
    }

}
int8_t OneRazRedRuka = 0; //  чтобы один раз картинку очистить и вывести что температура повышена
bool OneRazRuka = false; // Буль нужен чтобы при подносе здоровой руки один раз выбралось одно из загатовленных значений
void LogicTemp(){ // Блок принимает решение на основе считанной температуры
   // Блок принимает решение на основе считанной температуры [Если нет ошибки]
   
    //static int8_t OneRazRedRuka = 0; //  чтобы один раз картинку очистить и вывести что температура повышена

    if (  BodyTemp > 32 && BodyTemp < 37.5 ) { // Если поднесли руку и чел не горит от температуры
        if(RedActivate == false and YellowActivate == false){  // Если флаг красного сброшен
        digitalWrite(YellowDiodPin,LOW);// Погасить  жёлтую  лампу
        digitalWrite(RedDiodPin,LOW);   // Погасить  красную лампу

        digitalWrite(GreenDiodPin,HIGH);// Засветить зелёной лампой   
        //  из красной зоны в зелёную Очистка Дисплея и Ламп
            if(OneRazRedRuka == 2) {
                DrawDisplay("clean"); // Очистить дисплей
                //digitalWrite(GreenDiodPin,HIGH); // Погасить зелёную лампу
                digitalWrite(RedDiodPin,LOW);   // Погасить красную лампу
                OneRazRedRuka = 0; // Перезаряд
            }
        //  из красной зоны в зелёную Очистка Дисплея и Ламп

        if (OneRazRuka == false) { // Выбрать одно из 3х случайных значений 36.6 36.7 36.8 ОДИН раз
          
          // Собрать и напечатать на дисплей хорошую температуру
          GenerateGodRandTemp(); // Cоздаём случайное число

          char string[10];  // Create a character array of 10 characters
          dtostrf( RandGoodResult, 3, 1, string); //// Convert float to a string: (<variable>,<amount of digits we are going to use>,<amount of decimal digits>,<string name>)
          
          DrawDisplay(string); // Вывод 36.5 -36.7  // Вывод на экран [Рабочий] режим
          // Собрать и напечатать на дисплей хорошую температуру

          mp3_play (1);

          OneRazRuka = true;
      }
        } // Если флаг красного сброшен
      
    }
    if ( BodyTemp >= 37.6 && BodyTemp < 40.6 ) { // Если чел сгорает от температуры
        // Засветить красной лампой
      if(YellowActivate == false){ // Если жёлтый флаг опущен
          digitalWrite(RedDiodPin,HIGH); // Зажечь красный диод
          digitalWrite(GreenDiodPin,LOW);// Засветить зелёной лампой 

        // Один раз очистить дисплей 
        if (OneRazRedRuka == 0) {
            DrawDisplay("clean"); // Очистить дисплей
            OneRazRedRuka = 1;
        }
        // Один раз очистить дисплей
        
        // Вывести на дисплей инфу = повышена температура
            if (OneRazRedRuka == 1) {
                //Serial.println("Red Temp"); // Debug
                DrawDisplay("HighTemp"); // Вывести на дисплей H Temp
                mp3_play (2); // Воспроизвести "Температура повышена"
                //delay(1500);
                OneRazRedRuka = 2;
            }
        // Вывести на дисплей инфу = повышена температура
        RedActivate = true; // Поднять флаг что вход в красную зону выполнен
      
      // Отправить команду на PC о выводе повышенной температуры
      
      } // Если жёлтый флаг опущен
    }

   

    // Блок принимает решение на основе считанной температуры [Если нет ошибки]
}

void loop() {

  PotRead(); // Чтение потенциометра и перевод в удобные 0.1 для калибровки
  #ifdef PodstavkaTemp
  BodyTemp = InptuF;
  #else
  BodyTemp = ( filter(mlx.readObjectTempC() + PotVal) );
  #endif

  if (digitalRead(PinTumbler) == LOW) { // Если тумблер в положении калибровка
      CalibrDispPrint(); // Вывод на дисплей калибровочных данных (Исходн знач температуры + калибр значение с потенциометра)
      CalibrPcPrint(); // Печать температуры постоянно на PC
      delay(100);
  }
  else { // Если тумблер в положении Рабочий режим
    
    //Если ошибка
    TubmblerDebugDispClean = false; // Перезаряд очистки экрана при щёлкании тумблером и переходе в дебаг
    
    if ( ( BodyTemp > -9999 && BodyTemp < 10 ) || (BodyTemp>41  && BodyTemp <9999 ) ) {
        digitalWrite(YellowDiodPin,HIGH);  // Засветить жёлтой лампой
        digitalWrite(GreenDiodPin,LOW);// Погасить зелёную лампой
        digitalWrite(RedDiodPin,LOW);  // Погасить красную лампой
        DrawDisplay("clean"); // Очистить дисплей
      
        YellowActivate = true; // Подняли жёлтый флаг

        OneRazRedRuka = 0; // Чтобы после жёлтого ошибки на экран при красном могла вывестись надпись h temp
        if(OneRazYellowDangerSay == false){
            mp3_play (3); // Воспроизвести "Ошибка"
            Serial.println(String(BodyTemp));Serial.println(" ");
            //delay(1900);
            OneRazYellowDangerSay = true;
        }
          //Serial.println(" mistake");
          //Serial.println(BodyTemp);
    }
    //Если ошибка

    // А если нет ошибки
      else{
          //Serial.println("no mistake");
          digitalWrite(YellowDiodPin,LOW);
          OneRazYellowDangerSay = false; // Перезарядка один раз Воспроизезвести "Ошибка"
          // Старт точка отсчёта
          if(BeginOtchet == false){ //Если не было начало отсчёта
            timingDelayBetweenTempCheck = millis(); // Присвоить текущее время
            BeginOtchet = true; // Был конец отсчёта
          }
          // Старт точка отсчёта

          // Если температура измеряемая датчиком говорит что рука поднесена, надо выждать секунду, а затем принимать решение что делать, если это не ситуация когда у нас ошибка
          if(IsBeDelay == false && BodyTemp > 32 ){ // Если задержки ещё небыло и рука была поднесена
              //!Serial.println (F("I sees hand"));
              if (millis() - timingDelayBetweenTempCheck > 1000){ // Вместо 10000 подставьте нужное вам значение паузы 
                 // delay(2500);
                  //timingDelayBetweenTempCheck = millis(); 
                  //Serial.println (F("1.0 seconds Delay afther Ruka"));
                  IsBeDelay = true; // Подняли флаг что была задержка
              }
          }        
          // Если температура измеряемая датчиком говорит что рука поднесена, надо выждать секунду, а затем принимать решение что делать, если это не ситуация когда у нас ошибка
          if(IsBeDelay == true){ // Если была задержка в одну секунду после начала поднесения руки
              LogicTemp(); // Блок принимает решение на основе считанной температуры
          }
          if ( BodyTemp >= 10 && BodyTemp <= 32 ) { //Если убрали руку
             // Serial.println("No hand"); // Debug
              digitalWrite(GreenDiodPin,LOW);// Погасить зелёную лампой
              digitalWrite(RedDiodPin,LOW);  // Погасить красную лампой
              
              OneRazRuka = false;
              OneRazRedRuka = 0;

              IsBeDelay = false; // Перезарядить начало задержки первоначального считывания при убранной руке
              BeginOtchet = false; // Разрешить заново присвоить точку отсчёта [Ждать секунду если поднесли руку]
              
              DrawDisplay("clean"); // Очистить дисплей
              /*
              // Один раз
              if(OneRazStartZamer == false){
                  IsBeDelay = false; //N Перезарядить начало задержки первоначального считывания при убранной руке
                  BeginOtchet = false; //N Разрешить заново присвоить точку отсчёта [Ждать секунду если поднесли руку]        
                  OneRazStartZamer = true;
              }
              // Один раз
              */
             RedActivate = false; // Сбросили Красный флаг. Теперь получится зелёному загорется
             YellowActivate = false;
        }
      }
    // А если нет ошибки

  } // Если тумблер в положении Рабочий режим
 // */
}