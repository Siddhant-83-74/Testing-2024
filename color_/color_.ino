
#include <tcs3200.h>

int red, green, blue, white,present_color;

tcs3200 tcs(4, 5, 6, 7, 8); // (S0, S1, S2, S3, output pin)  //

void setup() {
  Serial.begin(9600);
}

void loop() {
  red = tcs.colorRead('r');   
  green = tcs.colorRead('g');   
  blue = tcs.colorRead('b');    
  white = tcs.colorRead('c');    

  present_color = red;
  if (blue > present_color) {
    present_color = blue;
    Serial.println("Blue color");
  }
  if (green > present_color) {
    present_color = green;
    Serial.println("Green color");
  }
  if (white > present_color) {
    present_color = white;
    Serial.println("White color");
  }
  else
  Serial.println("Red color");
  
  delay(200);

}
