#include <Encoder.h>

Encoder dead_x(5,6);
Encoder dead_y(5,6);

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  Serial.printf("Encoder_x: %d\ ",dead_x.read());
    Serial.printf("Encoder_y: %d\ ",dead_y.read());

}