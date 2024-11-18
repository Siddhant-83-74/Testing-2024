#include <Encoder.h>

IntervalTimer pid_timer;
Encoder enc1(31,30);

int m_pwm=5;
int m_dir=4;
float cpr=1300;
float kp=0,ki=0,kd=0;

float ap=0.0;
float sp_angle=0.0;
//char dir=0;
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
//Serial.printf("speed_dir: %d",speed<0?LOW:HIGH);

digitalWrite(m_dir,speed<0?LOW:HIGH);

//speed=abs(speed)>=1920?1920:abs(speed);
//Serial.printf("speed_drive: %d",abs(speed));
analogWrite(m_pwm,abs(speed));



}

float sp=0;
float err=0,der=0,integ=0;
float speed_m=0;
volatile float last_err=0;


void pid()
{
ap=enc1.read();
sp=sp_angle*cpr/360.0;

err=sp-ap;
der=(err-last_err)/0.01;
integ+=(err-last_err)*0.01;
last_err=err;

speed_m=kp*err+ki*integ+kd*der;

// Serial.printf("speed: %f   ",speed_m);
// Serial.printf("Kp: %f.   Ki: %f.    Kd: %f   ",kp,ki,kd);
Serial.print("Sp:");
Serial.print(sp_angle);
Serial.print("     Ap:");
Serial.println(ap*360.0/cpr);
      // Serial.println(enc1.read());
drive(int(speed_m));

}
int lastTime=0;
void loop()
{

//drive(5000);

    if (millis() - lastTime > 1000) {
      noInterrupts();

     if (Serial.available() > 0) {
       String input = Serial.readString();//0020,0000,0000,030
      // Serial.println(input.length());
       if (input.length() <= 20) {
         //char sign=(input.substring(0, 1)).charAt(0);
         kp=(input.substring(0,4)).toFloat();
         ki=(input.substring(5,9)).toFloat();
         kd=(input.substring(10,14)).toFloat();
         sp_angle=(input.substring(15)).toInt();
       }
     } 
        interrupts();
      lastTime = millis();
    }


  //ps4_input();
}