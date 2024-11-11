#include <Encoder.h>
Encoder enc(3,4);

void setup()
{
  Seria.begin(9600);
}

void loop()
{
  int encoder_val=enc.read();
  Serial.printf("Encoder value: %d ",encoder_val);
}