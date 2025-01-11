#include "USBHost_t36.h"
#include <Encoder.h>
IntervalTimer rpm_timer;
IntervalTimer ps4_timer;
//const int ledPin = LED_BUILTIN;  // the pin with a LED
Encoder myEnc(28, 27);

int m2_pwm = 3;
int m2_dir = 2;
volatile float rpm=0;

int res=pow(2,14)-1;
int duty_cycle=25;//in percentage
int max_pwm =(int)(duty_cycle/100.0*res); //6v--250rpm

//int res=pow(2,14)-1;
//int duty_cycle=25;//in percentage
//int max_pwm =(int)(duty_cycle/100.0*res); //6v--250rpm
void drive(int pwm_val, int pwmPin, int dirPin)
{
  
  digitalWrite(dirPin, (pwm_val <= 0 ? LOW : HIGH));
 // Serial.printf("Dir: %d\n", (pwm_val <= 0 ? LOW : HIGH));
  //Serial.printf("PWM: %d \n", abs(pwm_val),);
  analogWrite(pwmPin, abs(pwm_val));
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

  Serial.begin(9600);
  // while (!Serial) ; // wait for Arduino Serial Monitor

  Serial.println("\n\nUSB Host Testing - Joystick Bluetooth");
  if (CrashReport) Serial.print(CrashReport);
  myusb.begin();

  analogWriteFrequency(m2_pwm, 9000);
  
  pinMode(13, OUTPUT);
  digitalWrite(13,HIGH);
  pinMode(2,OUTPUT);
  
  Serial.begin(9600);
  ps4_timer.priority(0);
  rpm_timer.priority(1);
  ps4_timer.begin(ps4_input,75000);
  rpm_timer.begin(calc_rpm, 75000);// calcRpm() to run every 0.1 seconds....us
    analogWriteResolution(14);

  //analogWriteFrequency(m4_pwm, 9000);
  
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
  rpm=count/1300.0*600*4/3;
   rpm *= newPosition < oldPosition ? -1 : 1;
  Serial.printf("RPM: %f   ",rpm);
  count=0;
  oldPosition=newPosition;
//Serial.println(myEnc.read());
//
//Serial.println(millis());
}


void ps4_input(){
myusb.Task();
  if (joystick1.available()) {
    for (uint8_t i = 0; i < 64; i++) {
      psAxis_prev[i] = psAxis[i];
      psAxis[i] = joystick1.getAxis(i);
    }

    buttons = joystick1.getButtons();

        int left_y = psAxis[1] - 128;
            int y = map(left_y, -128, 128,max_pwm, -max_pwm);
            Serial.printf(" PWM:  %d\n",y);
          drive(y, m2_pwm, m2_dir);

  }
}
          
float rpmCopy=0;
// The main program will print the blink count
// to the Arduino Serial Monitor
void loop() {

    
          

//ps4_input();
  

 
}

  
