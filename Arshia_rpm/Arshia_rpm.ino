#include <Encoder.h>
#include <IntervalTimer.h> 
#include "USBHost_t36.h"

Encoder myEnc(28, 27);

//PS4 connection 
USBHost myusb;   // initializes and manages the USB host port, enabling Teensy to detect and commuincate with USB bluetooth dongle
USBHIDParser hid1(myusb);  //works behind the scenes to parse HID data that comes from the PS4 controller, such as joystick movements and button presses.
JoystickController joystick(myusb);
BluetoothController bluet(myusb, true, "0000");   // Version does pairing to device

//coordinates of joystick (x,y -> right joystick; leftX -> left joystick)
int x, y, leftX; 

int PWM = 7;
int DIR = 6;
volatile long encoderCounts = 0; // Shared variable to track encoder counts
volatile long lastCount = 0;      
volatile double rpm = 0;         // Stores the calculated RPM
long positionChange;

IntervalTimer timer; // Timer object for periodic execution

void calculateRPM() {
 // noInterrupts();

  myusb.Task(); // Handle USB host tasks

  if (joystick.available()) {
    // Left Stick values (axes 0 and 1)
    int leftStickX = joystick.getAxis(0);
    leftX = map(leftStickX, 0, 255, -127, 127);

    // Right Stick values (axes 2 and 5)
    int rightStickX = joystick.getAxis(2);
    x = map(rightStickX, 0, 255, -127, 127);
    int rightStickY = joystick.getAxis(5);
    y = map(rightStickY, 0, 255, -127, 127);

    // Ignore small joystick values
    if (abs(x) < 5) x = 0;
    if (abs(y) < 5) y = 0;
    if (abs(leftX) < 5) leftX = 0;
  } 
  int pwmValue = map(y,-127,127, -255, 255);
  pwmValue = abs(pwmValue); 
  if (y > 0) {      //to check direction: if +ve - HIGH, else LOW
    digitalWrite(DIR, HIGH);
  } else if (y < 0) {
    digitalWrite(DIR, LOW);
    y = -y;
  } else {
    pwmValue = 0;
  }
  analogWrite(PWM, pwmValue);

  // Serial.print("PWM:");
  // Serial.println(pwmValue);

  //noInterrupts(); 
  long currentCounts = myEnc.read();
  positionChange = currentCounts - lastCount;
  lastCount = currentCounts;
  //interrupts();

  // Calculate RPM
  rpm = (positionChange / 1300.0)*(60*(1000.0/75)) ;

  Serial.print("RPM: ");
  Serial.println(rpm);
 // interrupts();
}

void setup() {
  Serial.begin(9600);

  // Motor control pins setup
  pinMode(PWM, OUTPUT);
  pinMode(DIR, OUTPUT);

  // Initialize motor to stop
  analogWrite(PWM, 0);
  digitalWrite(DIR, LOW);

  myusb.begin();
  delay(2000);

  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  // Set up the timer to call calculateRPM every 1 second (1000000 microseconds)
  timer.begin(calculateRPM, 75000);
}

void loop() {
  //PS4();
  //Serial.print("Y:");
  //Serial.println(y);
  //runMotor1(y);

  //Print RPM
  //noInterrupts();
  //Serial.print("RPM: ");
  //Serial.println(rpm);
  //interrupts();
  
}
void runMotor1(float speed) {
  noInterrupts();
  Serial.print("speed: ");
  Serial.println(speed);
  int pwmValue = map(speed,-127,127, -255, 255);
  pwmValue = abs(pwmValue); 
  if (speed > 0) {      //to check direction: if +ve - HIGH, else LOW
    digitalWrite(DIR, HIGH);
  } else if (speed < 0) {
    digitalWrite(DIR, LOW);
    speed = -speed;
  } else {
    pwmValue = 0;
  }
  analogWrite(PWM, pwmValue);

  //Serial.print("PWM:");
  //Serial.println(pwmValue);
  interrupts();
}

void PS4() {
  noInterrupts();

  myusb.Task(); // Handle USB host tasks

  if (joystick.available()) {
    // Left Stick values (axes 0 and 1)
    int leftStickX = joystick.getAxis(0);
    leftX = map(leftStickX, 0, 255, -127, 127);

    // Right Stick values (axes 2 and 5)
    int rightStickX = joystick.getAxis(2);
    x = map(rightStickX, 0, 255, -127, 127);
    int rightStickY = joystick.getAxis(5);
    y = map(rightStickY, 0, 255, -127, 127);

    // Ignore small joystick values
    if (abs(x) < 5) x = 0;
    if (abs(y) < 5) y = 0;
    if (abs(leftX) < 5) leftX = 0;
  } else {
    Serial.println("Joystick Not Found");
  }
  interrupts();
}