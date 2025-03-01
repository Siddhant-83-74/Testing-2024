#include "USBHost_t36.h"
#include <Encoder.h>
#include <Wire.h>

const double PI_v = 3.14159;

volatile float delta_angle=0,length=242.5,breadth=345;
volatile float RateRoll, RatePitch, RateYaw;

Encoder deadWheelX(22,23);//182mm is 2450 counts
Encoder deadWheelY(20,21); 

IntervalTimer thePidTimer;
IntervalTimer theOdoTimer;
IntervalTimer theEncoderTimer;


int pwm_pin[4] = { 3, 1, 7, 5 };
int dir_pin[4] = { 2, 0, 6, 4 };


Encoder m[4] = { Encoder(9, 8), Encoder(12, 11), Encoder(28, 27), Encoder(31, 30) };

volatile float rpm_rt[4] = { 0, 0, 0, 0 };

int res = pow(2, 14) - 1;
int duty_cycle = 25;                            //in percentage
int max_pwm = (int)(duty_cycle / 100.0 * res);  //6v--250rpm
int max_rpm = 250*2;//hello.....

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

  Serial.begin(57600);
  Serial.println("\n\nUSB Host Testing - Joystick Bluetooth");
  if (CrashReport) Serial.print(CrashReport);
  myusb.begin();

  for (int i = 0; i < 4; i++) {
    analogWriteFrequency(pwm_pin[i], 9000);
    pinMode(dir_pin[i], OUTPUT);
  }

  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  Wire2.setClock(400000);
  Wire2.begin();
  delay(250);
  Wire2.beginTransmission(0x68); 
  Wire2.write(0x6B);
  Wire2.write(0x00);
  Wire2.endTransmission();
  thePidTimer.priority(0);
  theOdoTimer.priority(1);
  thePidTimer.begin(pid, 75000);

  theOdoTimer.begin(odometry,10000);

  analogWriteResolution(14);

}
volatile long oldPosition[4] = { 0, 0, 0, 0 };
int ledState = LOW;
volatile unsigned long count[4] = { -999, -999, -999, -999 };  // use volatile for shared variables
volatile long newPosition[4] = { 0, 0, 0, 0 };

volatile int pwm_pid[] = {0,0,0,0};
volatile float rpm_sp[] = {0,0,0,0};
volatile float kp[] = {09.0,09.0,09.0,09.0};
volatile float ki[] = {165.0,165.0,165.0,165.0};
volatile float kd[] = {00.50,00.50,00.50,00.50};

float error[]={0,0,0,0};
float eInt[]={0,0,0,0};
float eDer[]={0,0,0,0}; 
float lastError[]={0,0,0,0};

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

void pid() {

  myusb.Task();
  if (joystick1.available()) {
    for (uint8_t i = 0; i < 64; i++) {
      psAxis_prev[i] = psAxis[i];
      psAxis[i] = joystick1.getAxis(i);
    }

    buttons = joystick1.getButtons();

    int y = psAxis[1] - 128;
    int x= -1*(psAxis[0] - 128);
    int w= psAxis[2]-128;
    rpm_sp[0] = map(-x-y+w,-255,255,max_rpm,-max_rpm);
    rpm_sp[1] = map(-x+y+w,-255,255,max_rpm,-max_rpm);
    rpm_sp[2] = map(x+y+w,-255,255,max_rpm,-max_rpm);
    rpm_sp[3] = map(x-y+w,-255,255,max_rpm,-max_rpm);
    //Serial.printf("time: %d\n ",millis());
   // for(int i=0;i<4;i++)
   // Serial.printf("RPM_%d_input:%0.2f  ",i+1, rpm_sp[i]);
  }


  for (int i = 0; i < 4; i++) {
    newPosition[i] = m[i].read();
    count[i] = abs(newPosition[i] - oldPosition[i]);
    // count=newPosition<oldPosition?-count:count;
    rpm_rt[i] = count[i] / 1300.0 * 600 * 4 / 3;
    rpm_rt[i] *= newPosition[i] < oldPosition[i] ? -1 : 1;

    //Serial.printf("RPM_output(motor: %d):%0.2f ",i+1, rpm_rt[i]);
    count[i] = 0;
    oldPosition[i] = newPosition[i];
  }
  //Serial.printf("\n");
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
volatile int x_count,y_count,x_pos,y_pos;
void odometry()
{
gyro_signals();
delta_angle+=RateYaw*0.01;
delta_angle*=-PI_v/180.0;
x_count=deadWheelX.read();
y_count=deadWheelY.read();

x_pos=-x_count/2450.0*182.0+length*delta_angle;
y_pos=y_count/2450.0*182.0-breadth*delta_angle;
Serial.printf("delta_angle:%d.  ",delta_angle);
Serial.printf("x_pos:%d   ",x_count);
Serial.printf("y_pos:%d\n",y_count);
}
float rpmCopy = 0;
int lastTime = 0;
int ind=-1;
void loop() {

  //  if (millis() - lastTime > 1000) {
  //    noInterrupts();
  //    if (Serial.available() > 0) {
  //      String input = Serial.readString();//1,030.000,145.000,000.500
  //      Serial.println(input.length());
  //      if (input.length() <= 100) {
  //        ind=(input.substring(0, 1)).toInt()-1;
  //        kp[ind] = (input.substring(2, 9)).toFloat();
  //        ki[ind] = (input.substring(10, 17)).toFloat();
  //        kd[ind] = (input.substring(18, 25)).toFloat();
  //        Serial.printf("%d   %f %f %f\n", ind,kp[ind], ki[ind], kd[ind]);
  //      }
  //    } 
  //      interrupts();
  //    lastTime = millis();
  //  }


  //ps4_input();
}
