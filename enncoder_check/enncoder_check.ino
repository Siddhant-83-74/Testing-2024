#include <Encoder.h>
Encoder enc(20,21);
Encoder enc2(22,23);

void setup()
{
  Serial.begin(2000000);
}

void loop()
{
  int encoder_val=enc.read();
  int encoder_val2=enc2.read();

  Serial.printf("Encoder 1 value: %d    ",encoder_val);
  Serial.printf("Encoder 2 value: %d \n",encoder_val2);

}