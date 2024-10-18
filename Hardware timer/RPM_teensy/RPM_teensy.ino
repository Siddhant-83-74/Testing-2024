
#include <Encoder.h>
IntervalTimer myTimer;
//const int ledPin = LED_BUILTIN;  // the pin with a LED
Encoder myEnc(9, 8);

int m4_pwm = 3;
int m4_dir = 2;

int res=pow(2,14)-1;
int duty_cycle=25;//in percentage
int max_pwm =(int)(duty_cycle/100.0*res); //6v--250rpm

//int res=pow(2,14)-1;
//int duty_cycle=25;//in percentage
//int max_pwm =(int)(duty_cycle/100.0*res); //6v--250rpm
void drive(int pwm_val, int pwmPin, int dirPin)
{
//  if (pwm_val > 255)
//    pwm_val = 255;
//  else if (pwm_val < -255)
//    pwm_val = -255;

  digitalWrite(dirPin, (pwm_val <= 0 ? LOW : HIGH));
 // Serial.printf("Dir: %d\n", (pwm_val <= 0 ? LOW : HIGH));
  //Serial.printf("PWM: %d\n", abs(pwm_val));
  analogWrite(pwmPin, abs(pwm_val));
}

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13,HIGH);
  pinMode(2,OUTPUT);
  Serial.begin(9600);
  myTimer.begin(calc_rpm, 75000);// calcRpm() to run every 0.1 seconds....us
  analogWriteFrequency(m4_pwm, 9000);

//  analogWrite(m4_pwm,6000);
//  Serial.println("move");
//  analogWrite(m4_dir,res);
  
}
 long oldPosition  = 0;
int ledState = LOW;
volatile unsigned long count = -999; // use volatile for shared variables
volatile float rpm=0;
volatile long newPosition;
void calc_rpm() {
  newPosition=myEnc.read();
  count=newPosition-oldPosition;
  rpm=count/1300.0*600*4/3;
  Serial.println(rpm);
  count=0;
  oldPosition=newPosition;
//Serial.println(myEnc.read());
//
//Serial.println(millis());
}
float rpmCopy=0;
// The main program will print the blink count
// to the Arduino Serial Monitor
void loop() {
  digitalWrite(m4_dir,HIGH);
  analogWrite(m4_pwm,127);

 
}

  
