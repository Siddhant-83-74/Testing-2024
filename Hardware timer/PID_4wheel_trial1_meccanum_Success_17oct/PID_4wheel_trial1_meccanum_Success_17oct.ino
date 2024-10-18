#include "USBHost_t36.h"
#include <Encoder.h>
IntervalTimer rpm_timer;
IntervalTimer ps4_timer;
IntervalTimer pid_timer;

int pwm_pin[4] = { 3, 1, 7, 5 };
int dir_pin[4] = { 2, 0, 6, 4 };


Encoder m[4] = { Encoder(9, 8), Encoder(11, 12), Encoder(25, 24), Encoder(28, 27) };

volatile float rpm_rt[4] = { 0, 0, 0, 0 };

int res = pow(2, 14) - 1;
int duty_cycle = 25;                            //in percentage
int max_pwm = (int)(duty_cycle / 100.0 * res);  //6v--250rpm
int max_rpm = 250*2;//hello.....

//int res=pow(2,14)-1;
//int duty_cycle=25;//in percentage
//int max_pwm =(int)(duty_cycle/100.0*res); //6v--250rpm
// void drive(int pwm_val, int pwmPin, int dirPin) {
//   digitalWrite(dirPin, (pwm_val <= 0 ? LOW : HIGH));
//   // Serial.printf("Dir: %d\n", (pwm_val <= 0 ? LOW : HIGH));
//   //Serial.printf("PWM: %d \n", abs(pwm_val),);
//   analogWrite(pwmPin, abs(pwm_val));
//   // Serial.println("Drive");
// }

USBHost myusb;
JoystickController joystick1(myusb);
BluetoothController bluet(myusb, true, "0000");
uint32_t buttons_prev = 0;
uint32_t buttons;

int psAxis[64];
int psAxis_prev[64];
bool first_joystick_message = true;

// int m2_pwm = 3;
// int m2_dir = 2;


void setup() {

  //Serial.begin(200000);
  // while (!Serial) ; // wait for Arduino Serial Monitor

  Serial.println("\n\nUSB Host Testing - Joystick Bluetooth");
  if (CrashReport) Serial.print(CrashReport);
  myusb.begin();

  for (int i = 0; i < 4; i++) {
    analogWriteFrequency(pwm_pin[i], 9000);
    pinMode(dir_pin[i], OUTPUT);
  }

  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  // pinMode(2,OUTPUT);

  Serial.begin(9600);
  ps4_timer.priority(0);
  rpm_timer.priority(1);
  pid_timer.priority(2);
  //const_timer.priority(3);
  ps4_timer.begin(ps4_input, 75000);
  rpm_timer.begin(calc_rpm, 75000);
  pid_timer.begin(pid, 75000);
  //const_timer.begin(input,75000);
  // calcRpm() to run every 0.1 seconds....us
  analogWriteResolution(14);

  // analogWriteFrequency(m4_pwm, 9000);

  //  analogWrite(m4_pwm,6000);
  //  Serial.println("move");
  //  analogWrite(m4_dir,res);
}
volatile long oldPosition[4] = { 0, 0, 0, 0 };
int ledState = LOW;
volatile unsigned long count[4] = { -999, -999, -999, -999 };  // use volatile for shared variables
volatile long newPosition[4] = { 0, 0, 0, 0 };


void calc_rpm() {
  for (int i = 0; i < 4; i++) {
    newPosition[i] = m[i].read();
    count[i] = abs(newPosition[i] - oldPosition[i]);
    // count=newPosition<oldPosition?-count:count;
    rpm_rt[i] = count[i] / 1300.0 * 600 * 4 / 3;
    rpm_rt[i] *= newPosition[i] < oldPosition[i] ? -1 : 1;

    Serial.printf("RPM_output(motor: %d):%0.2f ",i+1, rpm_rt[i]);
    count[i] = 0;
    oldPosition[i] = newPosition[i];
  }
  Serial.printf("\n");
}
volatile int pwm_pid[] = {0,0,0,0};
volatile float rpm_sp[] = {0,0,0,0};
void ps4_input() {
  myusb.Task();
  if (joystick1.available()) {
    for (uint8_t i = 0; i < 64; i++) {
      psAxis_prev[i] = psAxis[i];
      psAxis[i] = joystick1.getAxis(i);
    }

    buttons = joystick1.getButtons();

    int y = psAxis[1] - 128;
    int x= psAxis[0] - 128;
    rpm_sp[0] = map(+x+y,-255,255,max_rpm,-max_rpm);
    rpm_sp[1] = map(+x-y,-255,255,max_rpm,-max_rpm);
    rpm_sp[2] = map(-x-y,-255,255,max_rpm,-max_rpm);
    rpm_sp[3] = map(-x+y,-255,255,max_rpm,-max_rpm);
    //Serial.printf("time: %d\n ",millis());
    for(int i=0;i<4;i++)
    Serial.printf("RPM_%d_input:%0.2f  ",i+1, rpm_sp[i]);
  }
}
volatile float kp[] = {09.0,09.0,09.0,09.0};
volatile float ki[] = {165.0,165.0,165.0,165.0};
volatile float kd[] = {00.50,00.50,00.50,00.50};

float error[]={0,0,0,0};
float eInt[]={0,0,0,0};
float eDer[]={0,0,0,0}; 
float lastError[]={0,0,0,0};
//void input()
//{
//
//
//  }
void pid() {
  //Serial.println(rpm);
  for(int i=0;i<4;i++){
  error[i] = rpm_sp[i] - rpm_rt[i];
  eDer[i] = (error[i] - lastError[i]) / 0.075;
  eInt[i] = eInt[i] + error[i] * 0.075;

  pwm_pid[i] = int(kp[i] * error[i] + ki[i] * eInt[i] + kd[i] * eDer[i]);
  //Serial.printf("pwm_pid:%d ",pwm_pid);
  digitalWrite(dir_pin[i], (pwm_pid[i] <= 0 ? LOW : HIGH));
  analogWrite(pwm_pin[i], abs(pwm_pid[i]));

  lastError[i] = error[i];
  }
}

float rpmCopy = 0;
// The main program will print the blink count
// to the Arduino Serial Monitor
int lastTime = 0;
int ind=-1;
void loop() {

  // if (millis() - lastTime > 1000) {
  //   noInterrupts();
  //   if (Serial.available() > 0) {
  //     String input = Serial.readString();//1,030.000,145.000,000.500
  //     Serial.println(input.length());
  //     if (input.length() <= 100) {
  //       ind=(input.substring(0, 1)).toInt()-1;
  //       kp[ind] = (input.substring(2, 9)).toFloat();
  //       ki[ind] = (input.substring(10, 17)).toFloat();
  //       kd[ind] = (input.substring(18, 25)).toFloat();
  //       Serial.printf("%d   %f %f %f\n", ind,kp[ind], ki[ind], kd[ind]);
  //     }
  //   } 
  //     interrupts();
  //   lastTime = millis();
  // }


  //ps4_input();
}
