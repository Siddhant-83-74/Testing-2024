#include <Encoder.h>

//IntervalTimer pid_timer;
Encoder enc1(12,11);

void setup()
{
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
  pinMode(0,OUTPUT);//dir
  pinMode(1,OUTPUT);//pwm
  digitalWrite(0,LOW);
  analogWrite(1,30);
  Serial.begin(9600);
}
void loop(){

//int a=enc1.read();
int last_a=0;
if(last_a!=enc1.read())
Serial.println(enc1.read());

last_a=enc1.read();
}