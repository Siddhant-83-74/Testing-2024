int dir=1;
int step=2;

void setup() {
  // put your setup code here, to run once:
pinMode(dir,OUTPUT);
pinMode(step,OUTPUT);
pinMode(13, OUTPUT);
digitalWrite(13, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
if(Serial.available()){

  String rc=Serial.readString();
  int del=(rc.substring(1)).toInt();
  
  String firstc = rc.substring(0,1);
  Serial.println(firstc);
  Serial.println(del);
  if(rc.substring(0,1)=="s")
  { digitalWrite(dir,HIGH);
    Serial.println("S mode");
    Serial.println(del);
    for(int i=0;i<200*4;i++)
      {
        digitalWrite(step,HIGH);
        delayMicroseconds(del);
        digitalWrite(step,LOW);
        delayMicroseconds(del);
      }
  }

   else if(rc.substring(0,1)=="w")
  { digitalWrite(dir,LOW);
  Serial.println("W mode");
    Serial.println(del);
    for(int i=0;i<200*4;i++)
      {
        digitalWrite(step,HIGH);
        delayMicroseconds(del);
        digitalWrite(step,LOW);
        delayMicroseconds(del);
      }
}
}}
