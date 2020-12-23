import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

import processing.serial.*; 
import processing.sound.*; 

import java.util.HashMap; 
import java.util.ArrayList; 
import java.io.File; 
import java.io.BufferedReader; 
import java.io.PrintWriter; 
import java.io.InputStream; 
import java.io.OutputStream; 
import java.io.IOException; 

public class TempPcAppFinal extends PApplet {

// 10.12.2020-24.12.2020
// Романовский И С
// Cкетч принимает данные с ардуино [температуру] и меняет цвет окна в зависимости от данной температуры
// Добавлен приём температуры от ардуино.  Вроде как убрана привязка железная к порту
// !ser receive http://developer.alexanderklimov.ru/arduino/processing.php

// Текст, цвета фона
PFont f;

//https://maximal.github.io/colour/
int Green = 0xff138808;
int Red = 0xff45161c;
int Yellow = 0xffe1cc4f;
int Black = 0xff0a0a0a;
// Текст, цвета фона

// Приём по сериал

Serial[] myPorts = new Serial[2];  // Create a list of objects from Serial class
String received="9999"; // данные, получаемые с последовательного порта
// Приём по сериал

// Блок звук

SoundFile TempIsNormal;
SoundFile HighTemp;
SoundFile ErrorTemp;
// Блок звук

float InputTemp = 36.6f;
boolean IsBeFirst = false; // Прикладывали ли руку
boolean NewMessedge = false;

// Не шуметь при отладке

boolean CanSound = true; 
// Не шуметь при отладке
public void setup() {
  

  f = createFont("SourceCodePro-Regular.ttf", 48);
  textFont(f);
  textAlign(CENTER, CENTER);

  // Приём по сериал/
  /* Работает на моём ПК
  String portOne = Serial.list()[0];
  String portTwo = Serial.list()[1];
  String portThree = Serial.list()[2];
  println (portOne);
  // open the ports:
  myPorts[0] = new Serial(this, portOne, 9600);
  myPorts[1] = new Serial(this, portTwo, 9600);
  myPorts[2] = new Serial(this, portThree, 9600);
  */ //Работает на моём ПК
    String MyArray[]; //Cюда скопируем все ком порты что в налии
  int Myindex; // Индекс последнего
 printArray(Serial.list()); // Печать массива
 MyArray = Serial.list();  // Копируем ком порты в наш массив строк
 println(); println();
 //println(MyArray.length);
 Myindex = (MyArray.length)-1;
 print("Myindex: "); println(Myindex); // Последний индекс - наше устройство
  String portTwo = Serial.list()[Myindex];
  myPorts[0] = new Serial(this, portTwo, 9600);
  // Приём по сериал
  
  // Первая заставочка
  background(255);
  //textFont(f,16);                  // STEP 3 Specify font to be used
  fill(0);                         // STEP 4 Specify font color 
  text("Bring your hand", width/2, height/2);   // STEP 5 Display Text
  // Первая заставочка
  
  // Блок звук
  TempIsNormal = new SoundFile(this, "Температура в норме!.wav");
  HighTemp = new SoundFile(this, "Температура_повышена,_обратитесь_к_врачу_.wav");
  ErrorTemp = new SoundFile(this, "Ошибка! Попробуйте ещё раз.wav");
  
  //TempIsNormal.play(); delay(1500);
  //HighTemp.play();delay(2500);
  //ErrorTemp.play(); delay(1900);
  // Блок звук
}

public void draw() {
  if (IsBeFirst) { // Если заставка промотана первым прикладом руки

    fill(255); // Белый цвет шрифта
    if (InputTemp >32 && InputTemp <37.5f  ) {
      
      
      // Один раз воспроизвести звук
      if(NewMessedge == true){   
          if(CanSound){ // Если рабочий режим
            background(Green); //
            text(nf(InputTemp, 0, 1) + "°C", width/2, height/2);
            TempIsNormal.play(); //delay(1500);            
          }
          else{ // Если отладка режим
            background(Black); //
            text(nf(InputTemp, 0, 1) + "°C", width/2, height/2);
          }
         NewMessedge = false; 
      }
      // Один раз воспроизвести звук
    } else if (InputTemp >= 37.6f && InputTemp <40.6f) {
     
      
       // Один раз воспроизвести звук
      if(NewMessedge == true){
        TempIsNormal.stop(); // Прервать другой звук если он идёт
          if(CanSound){
             background(Red); //
             text(nf(InputTemp, 0, 1) + "°C", width/2, height/2);
             HighTemp.play(); //delay(2200);
          }
          else{
            background(Black);
            text(nf(InputTemp, 0, 1) + "°C", width/2, height/2);
          }
         NewMessedge = false; 
      }
      // Один раз воспроизвести звук
    } else {
     
      
      // Один раз воспроизвести звук
      if(NewMessedge == true){
        TempIsNormal.stop(); // Прервать другой звук если он идёт
        HighTemp.stop(); // Прервать другой звук если он идёт
        if(CanSound){
           background(Yellow);
           text(nf(InputTemp, 0, 1) + "°C", width/2, height/2);
           ErrorTemp.stop();
           ErrorTemp.play();// delay(1500);
        }
        else{
          background(Black);
          text(nf(InputTemp, 0, 1) + "°C", width/2, height/2);
        }
        NewMessedge = false; 
      }
      // Один раз воспроизвести звук
    }
    
  }
   /*
   // Cмотрит на то как часто приходят сообщенения по сериал. Если чаще чем 150 МиллиСек то это дебаг
    if(millis()- Debugtiming < 150){
      //println("Nay debug");
      //Debugtiming=millis();
      CanSound = false;
    }
    else{
      CanSound = true;
      println("No debug");
    }
    // Cмотрит на то как часто приходят сообщенения по сериал. Если чаще чем 150 МиллиСек то это дебаг
    */
}




public void serialEvent(Serial thisPort) {
  // variable to hold the number of the port:
  /*
  int portNumber = -1;

  // iterate over the list of ports opened, and match the 
  // one that generated this event:
  for (int p = 0; p < myPorts.length; p++) {
    if (thisPort == myPorts[p]) {
      portNumber = p;
    }
  }
  */
  // read a byte from the port:
  // int inByte = thisPort.read();
  received = thisPort.readStringUntil(' ');
  if (received != null) { //удаляем пробелы

    received = trim(received);
    //print("received: ");println(received);
    //print("received.len: ");println(received.length() );
    char c1 = received.charAt(0);
    //print("c1: ");println(c1);
    if(c1 != 'D'){ // Если не отладка
      InputTemp=PApplet.parseFloat(received); // Преобразовать всю строку в число
      CanSound = true; // Разрешить звуки играть
    }
    else{
      CanSound = false; // Запретить звуки играть
     // print("InputTemp: ");println(received.substring(1, (received.length()-1) ));
       InputTemp=PApplet.parseFloat(received.substring(1, (received.length()-1) ));
    }
    print("InputTemp:");println(InputTemp);println();
    IsBeFirst = true; //Пометка выключить заставку
    NewMessedge = true; //Подняли флаг что пришло новое сообщение
    //println("Got " + received + " from serial port " + portNumber);
   

     //
}
  // put it in the list that holds the latest data from each port:
  // dataIn[portNumber] = received;
  // tell us who sent what:
 
 
}
  public void settings() {  size(640, 360); }
  static public void main(String[] passedArgs) {
    String[] appletArgs = new String[] { "TempPcAppFinal" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}
