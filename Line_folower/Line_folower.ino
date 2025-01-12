// Define the analog pins connected to the sensor array
#define bluetooth Serial

// #if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
// #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
// #endif

//BluetoothSerial SerialBT;
const int sensorPins[8] = {A0, A1, A2, A3, A4, A5, A6, A7};
int in1_l=8,in1_r=9,in2_l=7,in2_r=10,pwm_l=6,pwm_r=11,stdby=5;
void setup() {
  Serial.begin(9600); // Initialize serial communication at 9600 baud rate
  for (int i = 0; i < 8; i++) {
    pinMode(sensorPins[i], INPUT); // Set sensor pins as input
  }
  pinMode(in1_l,OUTPUT);
  pinMode(in1_r,OUTPUT);
  pinMode(in2_l,OUTPUT);
  pinMode(in2_r,OUTPUT);
  pinMode(pwm_l,OUTPUT);
  pinMode(pwm_r,OUTPUT);
  pinMode(stdby,OUTPUT);

  digitalWrite(stdby,HIGH);

  bluetooth.begin(9600);
  Serial.println("Bluetooth Started! Ready to pair...");
}
void drive(int speed)
{
    Serial.println(speed);

  
if (speed<0)
{
  digitalWrite(in1_r,HIGH);
  digitalWrite(in2_r,LOW);
  digitalWrite(in1_l,HIGH);
  digitalWrite(in2_l,LOW);
  analogWrite(pwm_l,-1*speed);
  analogWrite(pwm_r,-1*speed);
}
else if(speed>0)
{
  digitalWrite(in1_r,LOW);
  digitalWrite(in2_r,HIGH);
  digitalWrite(in1_l,LOW);
  digitalWrite(in2_l,HIGH);
  analogWrite(pwm_l,speed);
  analogWrite(pwm_r,speed);
}
else
{
  digitalWrite(in1_r,HIGH);
  digitalWrite(in2_r,LOW);
  digitalWrite(in1_l,LOW);
  digitalWrite(in2_l,HIGH);
  analogWrite(pwm_l,maxPwm);
  analogWrite(pwm_r,maxPwm);
}
}


}

int sensorValue=0;
int position=0;
int isBlack=0;

float Kp=0.04,Kd=0,Ki=0,lastTime=0,maxPwm=50;
String receivedData;
float error=0,integ=0,der=0,lastError=0,pidOut=0;
void loop() {
  for (int i = 0; i < 8; i++) {
    if(i<4)
      sensorValue = analogRead(sensorPins[i]); // Read the analog value
    else
      sensorValue = -1*analogRead(sensorPins[i]); // Read the analog value
    isBlack=analogRead(sensorPins[0])-analogRead(sensorPins[7]);
    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(sensorValue);
    position+=sensorValue;
        Serial.print("   ");
    if(i==7){
      Serial.println();
    Serial.print("Postion: ");
    Serial.print(position);
    Serial.print(" Black: ");
    Serial.print(isBlack);
   // position=0;
    isBlack=0;
    }
}
Serial.println();
if (bluetooth.available()) {
    receivedData = bluetooth.readStringUntil('\n');//000,000,000,000----Kp,Ki,Kd,running_speed
    Kp=receivedData.substring(0,3).toFloat();
    Ki=receivedData.substring(4,7).toFloat();
    Kd=receivedData.substring(8,11).toFloat();
    maxPwm=receivedData.substring(12).toInt();
    Serial.print("Received via Bluetooth: ");
    Serial.println(receivedData);
  }
if(millis()-lastTime>=100)
{
  error=position;
  integ=(error-lastError)*0.1;
  der=(error-lastError)/0.1;

  pidOut=Kp*error+Ki*integ+Kd*der;
  drive(int(pidOut));
  Serial.println(pidOut);
  lastError=error;
  
}
position=0;



     // Print the sensor value to the serial monitor // delay(500); // Wait for 500 milliseconds before the next reading
}