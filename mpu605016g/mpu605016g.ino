#include <Wire.h>

float acc_x,acc_y,acc_z;

void acc_value(void)
{
  Wire2.beginTransmission(0x68);

  Wire2.write(0x1C);
  Wire2.write(0x18);
  Wire2.endTransmission();

  Wire2.beginTransmission(0x68);
  Wire2.write(0x3B);
  Wire2.endTransmission();

  Wire2.requestFrom(0x68,6);

  int16_t ax=Wire2.read()<<8|Wire2.read();
  int16_t ay=Wire2.read()<<8|Wire2.read();
  int16_t az=Wire2.read()<<8|Wire2.read();

  acc_x=ax/2048.0*9.806;
  acc_y=ay/2048.0*9.806;
  acc_z=az/2048.0*9.806;
}

void setup()
{
  Serial.begin(57600);
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);

  Wire2.setClock(400000);
  Wire2.begin();
  delay(250);

  Wire2.beginTransmission(0x68);
  Wire2.write(0x6B);
  Wire2.write(0x00);

  Wire2.endTransmission();

}

void loop()
{
  acc_value();

  Serial.printf("X: %f",acc_x);
  Serial.printf("\tY: %f",acc_y);
  Serial.printf("\tZ: %f\n",acc_z);
  //Serial.printf("");
  delay(100);


}