#include "USBHost_t36.h"
int dir=17,step=16;
USBHost myusb;
JoystickController joystick1(myusb);
BluetoothController bluet(myusb, true, "0000");
uint32_t buttons_prev = 0;
uint32_t buttons;

int psAxis[64];
int psAxis_prev[64];
bool first_joystick_message = true;


void setup() {
   Serial.println("\n\nUSB Host Testing - Joystick Bluetooth");
  if (CrashReport) Serial.print(CrashReport);
  myusb.begin();

  // put your setup code here, to run once:
pinMode(3,OUTPUT);
Serial.begin(9600);
}

void loop() {
  myusb.Task();
  if (joystick1.available()) {
    for (uint8_t i = 0; i < 64; i++) {
      psAxis_prev[i] = psAxis[i];
      psAxis[i] = joystick1.getAxis(i);
    }

    buttons = joystick1.getButtons();
    
        int y = psAxis[1] - 128;
        Serial.println(y);
            // rpm_sp = map(left_y, -128, 128,max_rpm, -max_rpm);
    if(y!=0)
    {
      digitalWrite(dir,y<0?LOW:HIGH);
      for(int i=0;i<100;i++){
      digitalWrite(step,HIGH);
      delay(1);
      digitalWrite(step,LOW);
      delay(1);}
    }
  }}
  // put your main code here, to run repeatedly:


