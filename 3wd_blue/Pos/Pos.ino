#include <Encoder.h>

IntervalTimer pid_timer;
Encoder enc1(3,2);

int m_pwm=6;
int m_dir=7;
float cpr=1300;
float kp,ki,kd;

float ap=0;
float sp_angle=0;
char dir=0;
void setup()
{
pinMode(13,OUTPUT);
digitalWrite(13,HIGH);
Serial.begin(2000000);
pinMode(m_pwm,OUTPUT);
pinMode(m_dir,OUTPUT);

analogWriteResolution(14);

pid_timer.begin(pid,10000);
}
void drive(int speed)
{
digitalWrite(m_dir,speed<0?LOW:HIGH);
analogWrite(m_pwm,abs(speed));

kp=10.0;
kd=0.0;
ki=0.0;
}
int sp=0;
int err=0,der=0,integ=0;
int speed_m=0;
volatile int last_err=0;


void pid()
{
ap=enc1.read();
sp=sp_angle*cpr/360.0;

err=sp-ap;
der=(err-last_err)*0.01;
integ+=(err-last_err)*0.01;
last_err=err;

speed_m=kp*err+ki*integ+kd*der;
drive(speed_m);
Serial.printf("Sp: %f   ",sp_angle);
Serial.printf("Ap: %f \n",ap*360.0/cpr);
      // Serial.println(enc1.read());

}
int lastTime=0;
void loop()
{

//drive(5000);

    if (millis() - lastTime > 1000) {
      noInterrupts();

     if (Serial.available() > 0) {
       String input = Serial.readString();//-256
      // Serial.println(input.length());
       if (input.length() <= 5) {
         //char sign=(input.substring(0, 1)).charAt(0);
         sp_angle=(input.substring(0)).toInt();
       }
     } 
        interrupts();
      lastTime = millis();
    }


  //ps4_input();
}