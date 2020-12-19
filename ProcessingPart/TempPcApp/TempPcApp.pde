// 10.12.2020
// Романовский И С
// Cкетч принимает данные с ардуино [температуру] и меняет цвет окна в зависимости от данной температуры

PFont f;

float InputTemp = 39.1;
int x=50;
int y=50;

//https://maximal.github.io/colour/
color Green = #138808;
color Red = #45161c;
color Yellow = #e1cc4f;

void setup() {
  size(640, 360);
  
    f = createFont("SourceCodePro-Regular.ttf", 48);
  textFont(f);
  textAlign(CENTER, CENTER);
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
     text(InputTemp, width/2, height/2);
}
