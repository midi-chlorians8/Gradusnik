// 10.12.2020
// Романовский И С
// Cкетч принимает данные с ардуино [температуру] и меняет цвет окна в зависимости от данной температуры
// Добавлен приём температуры от ардуино.  Вроде как убрана привязка железная к порту
// !ser receive http://developer.alexanderklimov.ru/arduino/processing.php

// Текст, цвета фона
PFont f;

float InputTemp = 36.6;

//https://maximal.github.io/colour/
color Green = #138808;
color Red = #45161c;
color Yellow = #e1cc4f;
// Текст, цвета фона

// Приём по сериал
import processing.serial.*;
Serial[] myPorts = new Serial[2];  // Create a list of objects from Serial class
String received="9999"; // данные, получаемые с последовательного порта
// Приём по сериал

void setup() {
  size(640, 360);
  
  f = createFont("SourceCodePro-Regular.ttf", 48);
  textFont(f);
  textAlign(CENTER, CENTER);
  
  // Приём по сериал
  String portOne = Serial.list()[0];
  String portTwo = Serial.list()[2];
  // open the ports:
  myPorts[0] = new Serial(this, portOne, 9600);
  myPorts[1] = new Serial(this, portTwo, 9600);
  // Приём по сериал
}

void draw() {

  if (InputTemp >32.5 && InputTemp <37.5  ) {
    background(Green); //
  } else if (InputTemp >37.5 && InputTemp <40.6) {
    background(Red); //
  } else {
    background(Yellow);
  }
     fill(255);
   //  text(InputTemp, width/2, height/2);
     text(nf(InputTemp, 0, 1) + "°C", width/2, height/2);
    // text( received + "°C", width/2, height/2);
}




void serialEvent(Serial thisPort) {
  // variable to hold the number of the port:
  int portNumber = -1;
  
  // iterate over the list of ports opened, and match the 
  // one that generated this event:
  for (int p = 0; p < myPorts.length; p++) {
    if (thisPort == myPorts[p]) {
      portNumber = p;
    }
  }
  // read a byte from the port:
 // int inByte = thisPort.read();
  received = thisPort.readStringUntil(' ');
   if (received != null) { //удаляем пробелы

    received = trim(received);
    
    /*
    String FirstChar = received.substring(0,1);
    print("FirstChar:"); println(FirstChar);
    if(FirstChar == "A"){
      println("qwer");
    }
    //
    //
    if(received.substring(0,1) == "A"){
        print("I receive it:");  println(received.substring(0,1));
    }
     */
// */
 println(received.substring(0,1));
   // println(received); //ищем нашу строку 'A' , чтобы начать рукопожатие
   
    InputTemp=float(received);
    //println("Got " + received + " from serial port " + portNumber);
   }
  // put it in the list that holds the latest data from each port:
 // dataIn[portNumber] = received;
  // tell us who sent what:
  
}
