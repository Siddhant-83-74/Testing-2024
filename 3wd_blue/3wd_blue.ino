#include <SPI.h>
#include <Servo.h>

#define FORWARD 'F'
#define BACKWARD 'B'
#define LEFT 'L'
#define RIGHT 'R'
#define CIRCLE 'C'
#define CROSS 'X'
#define TRIANGLE 'T'
#define SQUARE 'S'
#define START 'A'
#define PAUSE 'P'

Servo servo1;
Servo servo2;

int m3_dir = 5;
int m2_dir = 24;
int m1_dir = 22;

int m3_pwm = 6;
int m2_pwm = 4;
int m1_pwm = 3;

int servo_1 = 46;
int servo_2 = 44;
int em = 42;

int max_pwm = 60;
int robo_track = 1;
int servo_limit = 130;
void set_robotic_arm_forward() {
  if (robo_track == -1) {
    for (int i = 0; i <= servo_limit; i++) {
      servo1.write(i);
      servo2.write(servo_limit - i);
      delay(10);
    }
    robo_track = 1;
  } else if (robo_track == 0) {
    for (int i = 90; i <= servo_limit; i++) {
      servo1.write(i);
      servo2.write(servo_limit - i);
      delay(10);
    }
    robo_track = 1;
  }
  //obo_track=1
}

void set_robotic_arm_backward() {
  if (robo_track == 1) {
    for (int i = 0; i <= servo_limit - 30; i++) {
      servo1.write(servo_limit - i);
      servo2.write(i);
      delay(10);
    }
    robo_track = -1;
  } else if (robo_track == 0) {
    for (int i = 90; i <= servo_limit - 30; i++) {
      servo1.write(servo_limit - i);
      servo2.write(i);
      delay(10);
    }
    robo_track = -1;
  }
}

void set_robotic_arm_middle() {
  if (robo_track == -1) {
    for (int i = 90; i <= servo_limit; i++) {
      servo1.write(i);
      servo2.write(servo_limit - i);
      delay(10);
    }
    robo_track = 0;
  } else if (robo_track == 1) {
    for (int i = 90; i <= servo_limit; i++) {
      servo1.write(servo_limit - i);
      servo2.write(i);
      delay(10);
    }
    robo_track = 0;
  }
}

void drive(int s, int pwm, int dir) {
  if (s > 0) {
    digitalWrite(dir, HIGH);
    analogWrite(pwm, s);
    Serial.println("Positive speed");

  } else {
    digitalWrite(dir, LOW);
    analogWrite(pwm, -1 * s);
    Serial.println("Negative speed");
  }
}

void i_k(int x, int y, int w) {
  int s1 = 0;
  int s2 = 0;
  int s3 = 0;
  if (w != 0) {
    s1 = w;
    s2 = -1*w;
    s3 = -1*w;
  } else {
    s1 = -1*x;
    s2 = -0.5 * x +0.86 * y;
    s3 = -0.5 * x - 0.86 * y;
  }

  s1 = map(s1, -139, 139, -max_pwm, max_pwm);
  s2 = map(s2, -139, 139, -max_pwm, max_pwm);
  s3 = map(s3, -139, 139, -max_pwm * 0.95, max_pwm * 0.95);

  Serial.println(s1);
  Serial.println(s2);
  Serial.println(s3);
  drive(s1, m1_pwm, m1_dir);
  drive(s2, m2_pwm, m2_dir);
  drive(s3, m3_pwm, m3_dir);
}
int flag_em = 0;
void setup() {
  pinMode(m1_dir, OUTPUT);
  pinMode(m2_dir, OUTPUT);
  pinMode(m3_dir, OUTPUT);

  pinMode(m1_pwm, OUTPUT);
  pinMode(m2_pwm, OUTPUT);
  pinMode(m3_pwm, OUTPUT);

  servo1.attach(servo_1);
  servo2.attach(servo_2);

  Serial.begin(9600);
}

void loop() {
  // char command=' ';
  if (Serial.available()) {
    char command = Serial.read();
    Serial.println(command);

    switch (command) {
      case FORWARD:  // Perform action for moving forward
        i_k(0, -100, 0);
        Serial.println("Forward");
        break;
      case BACKWARD:  // Perform action for moving backward
        i_k(0, +100, 0);
        Serial.println("Backwrd");
        break;
      case LEFT:  // Perform action for turning left
        i_k(100, 0, 0);
        Serial.println("Left");
        break;
      case RIGHT:  // Perform action for turning right
        i_k(-100, 0, 0);
        Serial.println("Right");
        break;
      case CROSS:  // Perform action for circle
        set_robotic_arm_forward();
        Serial.println("robotic arm forward");
        break;
      case CIRCLE:  // Perform action for immediate stop or crossing
        i_k(0, 0, 100);
        Serial.println("Antilockwise");
        break;
      case SQUARE:  // Clockwise
        i_k(0, 0, -100);
        Serial.println("Clockwise");
        break;
      case TRIANGLE:  // Perform action for retrieving and sending status information
        set_robotic_arm_backward();
        Serial.println("robotic arm backward");
        break;
      case START:
        if (flag_em == 0)  // Controlling electromagnet
        {
          digitalWrite(em, HIGH);
          flag_em = 1;
          Serial.println("EM ON");

        } else {
          digitalWrite(em, LOW);
          flag_em = 0;
          Serial.println("EM OFF");
        }
        break;
      case PAUSE:
        set_robotic_arm_middle();  // Perform action for pausing a process or operation
        Serial.println("robotic arm middle");
        break;
      default:  // Invalid command received
        i_k(0, 0, 0);
        break;
    }
  }


  // delay(1000);
}