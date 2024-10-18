volatile float kp=1,kd=1,ki=1;
void setup() {
  // put your setup code here, to run once:

}
int lastTime=0;
void loop() {
  if(millis()-lastTime>1000){
  noInterrupts();
if(Serial.available()>0)
  {
    String input= Serial.readString();
    Serial.println(input.length());
    if(input.length()==19){
    kp=(input.substring(0,6)).toFloat();
    ki=(input.substring(6,12)).toFloat();
    kd=(input.substring(12)).toFloat();
Serial.printf("kp:%f ki:%f kd:%f\n",kp,ki,kd);
    
    }}
    else
        Serial.printf("kp:%f ki:%f kd:%f\n",kp,ki,kd);
    interrupts();
    lastTime=millis();}}
