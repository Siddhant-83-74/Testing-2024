#include <Encoder.h>

IntervalTimer pos_pid_timer;
Encoder enc1(9,8);

int shooter_rotation_pwm=7;
int shooter_rotation_dir=6;
float shooter_rotation_cpr=17500;
float kP=50,kI=1,kD=1;

int shooting_angle=0;

float ap=0.0;
float sp_angle=0.0;
//char dir=0;
void setup()
{
pinMode(13,OUTPUT);
digitalWrite(13,HIGH);
Serial.begin(2000000);
pinMode(shooter_rotation_pwm,OUTPUT);
pinMode(shooter_rotation_dir,OUTPUT);

analogWriteResolution(14);

pos_pid_timer.begin(pos_pid,10000);
}
void setPosition(int speed)
{
//Serial.printf("speed_dir: %d",speed<0?LOW:HIGH);

digitalWrite(shooter_rotation_dir,speed<0?LOW:HIGH);

//speed=abs(speed)>=1920?1920:abs(speed);
//Serial.printf("speed_drive: %d",abs(speed));
analogWrite(shooter_rotation_pwm,abs(speed));



}

float sp=0;
float err=0,der=0,integ=0;
float speed_m=0;
volatile float last_err=0;


void pos_pid()
{
ap=enc1.read();
sp=sp_angle*shooter_rotation_cpr/360.0;

err=sp-ap;
der=(err-last_err)/0.01;
integ+=(err-last_err)*0.01;
last_err=err;

speed_m=kP*err+kI*integ+kD*der;

// Serial.printf("speed: %f   ",speed_m);
// Serial.printf("Kp: %f.   Ki: %f.    Kd: %f   ",kp,ki,kd);
Serial.print("Sp:");
Serial.print(shooting_angle);
Serial.print("     Ap:");
Serial.println(int((ap*360.0/shooter_rotation_cpr)/6.25));
      // Serial.println(enc1.read());
setPosition(int(speed_m));

}
int lastTime=0;
void loop()
{

//drive(5000);

    if (millis() - lastTime > 1000) {
      noInterrupts();

     if (Serial.available() > 0) {
       String input = Serial.readString();//0050,0001,0001,030
      // Serial.println(input.length());
       if (input.length() <= 20) {
         //char sign=(input.substring(0, 1)).charAt(0);
         kP=(input.substring(0,4)).toFloat();
         kI=(input.substring(5,9)).toFloat();
         kD=(input.substring(10,14)).toFloat();
         shooting_angle=input.substring(15).toInt();
         sp_angle=int(shooting_angle*6.25);
       }
     } 
        interrupts();
      lastTime = millis();
    }


  //ps4_input();
}