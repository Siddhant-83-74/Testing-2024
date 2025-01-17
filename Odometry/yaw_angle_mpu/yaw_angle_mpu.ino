/*
The contents of this code and instructions are the intellectual property of Carbon Aeronautics. 
The text and figures in this code and instructions are licensed under a Creative Commons Attribution - Noncommercial - ShareAlike 4.0 International Public Licence. 
This license lets you remix, adapt, and build upon your work non-commercially, as long as you credit Carbon Aeronautics 
(but not in any way that suggests that we endorse you or your use of the work) and license your new creations under the identical terms.
This code and instruction is provided "As Is” without any further warranty. Neither Carbon Aeronautics or the author has any liability to any person or entity 
with respect to any loss or damage caused or declared to be caused directly or indirectly by the instructions contained in this code or by 
the software and hardware described in it. As Carbon Aeronautics has no control over the use, setup, assembly, modification or misuse of the hardware, 
software and information described in this manual, no liability shall be assumed nor accepted for any resulting damage or injury. 
By the act of copying, use, setup or assembly, the user accepts all resulting liability.

1.0  5 October 2022 -  initial release
*/
#include <Wire.h>
IntervalTimer yaw_angle;
float delta_angle=0;
volatile float RateRoll, RatePitch, RateYaw;
void gyro_signals(void) {
  Wire2.beginTransmission(0x68);
  Wire2.write(0x1A);
  Wire2.write(0x05);
  Wire2.endTransmission(); 
  Wire2.beginTransmission(0x68);
  Wire2.write(0x1B); 
  Wire2.write(0x8); 
  Wire2.endTransmission(); 
  Wire2.beginTransmission(0x68);
  Wire2.write(0x43);
  Wire2.endTransmission();
  Wire2.requestFrom(0x68,6);
  int16_t GyroX=Wire2.read()<<8 | Wire2.read();
  int16_t GyroY=Wire2.read()<<8 | Wire2.read();
  int16_t GyroZ=Wire2.read()<<8 | Wire2.read();
  RateRoll=(float)GyroX/65.5;
  RatePitch=(float)GyroY/65.5;
  RateYaw=(float)GyroZ/65.5;
}
void integ()
{
gyro_signals();
delta_angle+=RateYaw*0.01;
Serial.print("Delta angle :");
Serial.println(delta_angle);

}
void setup() {
  Serial.begin(57600);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  Wire2.setClock(400000);
  Wire2.begin();
  delay(250);
  Wire2.beginTransmission(0x68); 
  Wire2.write(0x6B);
  Wire2.write(0x00);
  Wire2.endTransmission();
  //yaw_angle.begin(integ,10000);
}
int lastTime=0;
void loop() {
  //if(millis()-lastTime){

  gyro_signals();
  Serial.print("Roll rate [°/s]= ");
  Serial.print(RateRoll);
  Serial.print(" Pitch Rate [°/s]= ");
  Serial.print(RatePitch);
  Serial.print(" Yaw Rate [°/s]= ");
  Serial.println(RateYaw-0.20);
  //delay(50);
  //lastTime=millis();
  
  }
