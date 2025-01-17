#include <Servo.h>
#include "USBHost_t36.h"

IntervalTimer reset;
Servo s1,s2;

USBHost myusb;
JoystickController joystick1(myusb);
BluetoothController bluet(myusb, true, "0000");   // Version does pairing to device
//BluetoothController bluet(myusb);   // version assumes it already was paired
uint32_t buttons_prev = 0;
uint32_t buttons;


int psAxis[64];
int psAxis_prev[64];
bool first_joystick_message = true;
//uint8_t last_bdaddr[6] = {0, 0, 0, 0, 0, 0};


int s1_pwm=0;
int s2_pwm=0;

volatile int flag[20]={0};


void setup()
{

  Serial.begin(2000000);
  // while (!Serial) ; // wait for Arduino Serial Monitor

  Serial.println("\n\nUSB Host Testing - Joystick Bluetooth");
  if (CrashReport) Serial.print(CrashReport);
  myusb.begin();

  s1.attach(5);
  s2.attach(6);
  
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);

  reset.begin(reset_flags,500000);
}
void reset_flags()
{
  for(int i=0;i<20;i++)
    flag[i]=1;
}
int count_r=0;
int count_l=0;
int pwm_l=30;
int pwm_r=30;
//=============================================================================
// Loop
//=============================================================================
void loop()
{

  myusb.Task();

  if (joystick1.available()) {
    for (uint8_t i = 0; i < 64; i++) {
      psAxis_prev[i] = psAxis[i];
      psAxis[i] = joystick1.getAxis(i);
    }

    buttons = joystick1.getButtons();
   // Serial.printf("button: %x \n",buttons);
    if(buttons==(0x10000) && flag[0]==1)
    {
      count_l++;
      pwm_l+=count_l*5;
      Serial.printf("Left_up: %d\n ",count_l);
      s1.write(map(pwm_l,0,255,0,180));
      flag[0]=0;
      
    }

    else if(buttons==(0x40000) && flag[1]==1)
    {
      count_l--;
      pwm_l-=count_l*5;
      Serial.printf("Left_down: %d \n",count_l);
      s1.write(map(pwm_l,0,255,0,180));
      flag[1]=0;
    }


    if(buttons==(0x8) && flag[2]==1)
    {
      count_r++;
      pwm_r+=count_r*5;
      Serial.printf("right_up: %d\n ",count_r);
      s2.write(map(pwm_r,0,255,0,180));
      flag[2]=0;
    }
    else if(buttons==(0x2) && flag[3])
    {
      count_r--;
      pwm_r+=count_r*5;
      Serial.printf("right_down: %d\n ",count_r);
      s2.write(map(pwm_r,0,255,0,180));
      flag[3]=0;
    }

    }


  }



