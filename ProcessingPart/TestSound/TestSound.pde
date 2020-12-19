
import processing.sound.*;

SoundFile file;

void setup() {
  size(640, 360);
  background(255);

  // Load a soundfile and start playing it on loop
  file = new SoundFile(this, "beat.aiff");
  //file.loop();
  file.play();
}      


void draw() {
}
