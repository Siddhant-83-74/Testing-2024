#include "USBHost_t36.h"
#include <Encoder.h>

IntervalTimer rpm_timer;
IntervalTimer ps4_timer;
IntervalTimer pid_timer;
//IntervalTimer const_timer;

//const int ledPin = LED_BUILTIN;  // the pin with a LED
//  Encoder myEnc(9, 8);
//  int m4_pwm = 2;
//  int m4_dir = 3;

// Encoder myEnc(12, 11);
// int m4_pwm = 1;
// int m4_dir = 0;

// Encoder myEnc(25, 24);
// int m4_pwm = 7;
// int m4_dir = 6;

Encoder myEnc(28, 27);
int m4_pwm = 5;
int m4_dir = 4;
volatile float rpm_rt=0;

int res=pow(2,14)-1;
int duty_cycle=25;//in percentage
int max_pwm =(int)(duty_cycle/100.0*res); //6v--250rpm
int max_rpm=250;

//int res=pow(2,14)-1;
//int duty_cycle=25;//in percentage
//int max_pwm =(int)(duty_cycle/100.0*res); //6v--250rpm
void drive(int pwm_val, int pwmPin, int dirPin)
{ digitalWrite(dirPin, (pwm_val <= 0 ? LOW : HIGH));
 // Serial.printf("Dir: %d\n", (pwm_val <= 0 ? LOW : HIGH));
  //Serial.printf("PWM: %d \n", abs(pwm_val),);
  analogWrite(pwmPin, abs(pwm_val));
 // Serial.println("Drive");
}

USBHost myusb;
JoystickController joystick1(myusb);
BluetoothController bluet(myusb, true, "0000");
uint32_t buttons_prev = 0;
uint32_t buttons;

int psAxis[64];
int psAxis_prev[64];
bool first_joystick_message = true;




void setup() {

  //Serial.begin(200000);
  // while (!Serial) ; // wait for Arduino Serial Monitor

  Serial.println("\n\nUSB Host Testing - Joystick Bluetooth");
  if (CrashReport) Serial.print(CrashReport);
  myusb.begin();

 // analogWriteFrequency(m4_pwm, 9000);
  
  pinMode(13, OUTPUT);
  digitalWrite(13,HIGH);
  pinMode(m4_dir,OUTPUT);
  
  Serial.begin(9600);
  ps4_timer.priority(0);
  rpm_timer.priority(1);
  pid_timer.priority(2);
  //const_timer.priority(3);
  ps4_timer.begin(ps4_input,75000);
  rpm_timer.begin(calc_rpm, 75000);
  pid_timer.begin(pid,75000);
  //const_timer.begin(input,75000);
  // calcRpm() to run every 0.1 seconds....us
    analogWriteResolution(14);

  analogWriteFrequency(m4_pwm, 9000);
  
//  analogWrite(m4_pwm,6000);
//  Serial.println("move");
//  analogWrite(m4_dir,res);
  
}
 long oldPosition  = 0;
int ledState = LOW;
volatile unsigned long count = -999; // use volatile for shared variables
volatile long newPosition;


void calc_rpm() {

  newPosition=myEnc.read();
  count=abs(newPosition-oldPosition);
 // count=newPosition<oldPosition?-count:count;
  rpm_rt=count/1300.0*600*4/3;
  rpm_rt*=newPosition<oldPosition?-1:1;

  Serial.printf("RPM_output:%f\n ",rpm_rt);
  count=0;
  oldPosition=newPosition;
  

}
volatile int pwm_pid=0;
volatile float rpm_sp=0;
void ps4_input(){
myusb.Task();
  if (joystick1.available()) {
    for (uint8_t i = 0; i < 64; i++) {
      psAxis_prev[i] = psAxis[i];
      psAxis[i] = joystick1.getAxis(i);
    }

    buttons = joystick1.getButtons();

        int left_y = psAxis[1] - 128;
             rpm_sp = map(left_y, -127.5, 127.5,max_rpm, -max_rpm);
            //Serial.printf("time: %d\n ",millis());
            Serial.printf("RPM_input:%f  ",rpm_sp);
         
          

  }
}
volatile float kp=09.0;
volatile float ki=165.0;
volatile float kd=00.50;

float error,eInt,eDer,lastError=0;
//void input()
//{
//  
//
//  }
void pid(){
  //Serial.println(rpm);
  error=rpm_sp-rpm_rt;
  eDer=(error-lastError)/0.075;
  eInt=eInt+error*0.075;

    pwm_pid=int(kp*error+ki*eInt+kd*eDer);
//Serial.printf("pwm_pid:%d ",pwm_pid);
drive(pwm_pid, m4_pwm, m4_dir); 

  lastError=error;
  
  }
          
float rpmCopy=0;
// The main program will print the blink count
// to the Arduino Serial Monitor
int lastTime=0;
void loop() {
  if(millis()-lastTime>10){
  noInterrupts();
if(Serial.available()>0)
  {
    String input= Serial.readString();
    Serial.println(input.length());
    if(input.length()<=100){
    kp=(input.substring(0,6)).toFloat();
    ki=(input.substring(7,14)).toFloat();
    kd=(input.substring(15,23)).toFloat();
Serial.printf("%f %f %f\n",kp,ki,kd);
    
    }}
    else
   //     Serial.printf("kp:%f ki:%f kd:%f\n",kp,ki,kd);
    interrupts();
    lastTime=millis();}
          

//ps4_input();
  

 
}

  
