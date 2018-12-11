const int ER = 5;
const int MR = 4;
const int EL = 6;
const int ML = 7;

const int trigPinL = A0;
const int echoPinL = A1;
const int trigPinR = A2;
const int echoPinR = A3;
const int trigPinR = 10;
const int echoPinR = 9;

const int topIRL = 3;
const int topIRM = 11;
const int topIRR = 2;

const int ballDetect = 12;

const int fan = A5;
const int potPin = A4;

int speedR, mapedVal, speedL, randNum;
const uint32_t period = 300L;
long distance, duration, rightSensor, leftSensor;
bool debug, minDistance, checkRight, checkLeft, fireL, fireM, fireR, isSideR, isSideL, lineDetected;

long SonarSensor(int trigPin, int echoPin) {
 digitalWrite(trigPin, LOW);
 delayMicroseconds(2);
 digitalWrite(trigPin, HIGH);
 delayMicroseconds(10);
 digitalWrite(trigPin, LOW);
 duration = pulseIn(echoPin, HIGH);
 return (duration / 2) / 29.1;
}
void initialisePins() {
  pinMode(trigPinR, OUTPUT);
  pinMode(echoPinR, INPUT);
  pinMode(trigPinL, OUTPUT);
  pinMode(echoPinL, INPUT);
  
  pinMode(fan,OUTPUT);

  pinMode(topIRL,INPUT);
  pinMode(topIRM,INPUT);
  pinMode(topIRR,INPUT);

  pinMode(ballDetect, INPUT_PULLUP);

}
void stopNow() {
 digitalWrite(ER, LOW);
 digitalWrite(EL, LOW);
}
void advance(char a, char b) {
 analogWrite(ER, a);
 digitalWrite(MR, HIGH);
 analogWrite(EL, b);
 digitalWrite(ML, HIGH);
}
void back_off(char a, char b) {
 analogWrite(ER, a);
 digitalWrite(MR, LOW);
 analogWrite(EL, b);
 digitalWrite(ML, LOW);
}
void turn_L(char a, char b) {
 analogWrite(ER, a);
 digitalWrite(MR, LOW);
 analogWrite(EL, b);
 digitalWrite(ML, HIGH);
}
void turn_R(char a, char b) {
 analogWrite(ER, a);
 digitalWrite(MR, HIGH);
 analogWrite(EL, b);
 digitalWrite(ML, LOW);
}
void turn_R60Deg(){
 for (uint32_t tStart = millis(); (millis() - tStart) < period; ) {
   turn_R(150, 150);
 }
}
void turn_L60Deg() {
 for (uint32_t tStart = millis(); (millis() - tStart) < period; ) {
   turn_L(150, 150);
 }
}
void adjustR(){
  for (uint32_t tStart = millis(); (millis() - tStart) < 350L; ) {
    back_off(speedR, speedL + 20);
  }
  turn_R(100, 100);
}
void adjustL() {
  for (uint32_t tStart = millis(); (millis() - tStart) < 350L; ) {
    back_off(speedR + 20, speedL);
  }
  turn_L(100, 100);
}
void fan_start(){
  for (uint32_t tStart = millis(); (millis() - tStart) < 5000L; ) {
    analogWrite(fan, 255);
  }
}
void fan_stop(){
  analogWrite(fan, 0);
}

void setup() {
  initialisePins();
  debug = true;
  if (debug) Serial.begin(9600);
  lineDetected = false;
}
void loop() {
  speedR = map(analogRead(potPin),0,1023,100,250);
  rightSensor = SonarSensor(trigPinR, echoPinR);
  delay(50);
  leftSensor = SonarSensor(trigPinL, echoPinL);
  delay(50);
  checkRight = rightSensor <= 25;
  checkLeft = leftSensor <= 25;
  if(ballDetect) {
    Serial.println("Ball Mode");
    
    if(lineDetected){
      Serial.println("Line Mode");
      if (digitalRead(topIRL)) { advance(speedR,0); }
      if (!digitalRead(topIRL)) { advance(0,speedR); }
      if (digitalRead(topIRM) && digitalRead(topIRR) && digitalRead(topIRL)) { stopNow(); }
    }
    else{
      if (digitalRead(topIRL)) {lineDetected = true; 
        advance(speedR,0);
        break;}
      if (checkLeft){ 
        for (uint32_t tStart = millis(); (millis() - tStart) < period; ) {
          turn_L(150, 150);
          if (digitalRead(topIRL)) {lineDetected = true; 
            stopNow();
            break;}
        }
      }
       if (checkRight){ 
        for (uint32_t tStart = millis(); (millis() - tStart) < period; ) {
          turn_R(150, 150);
          if (digitalRead(topIRL)) {lineDetected = true; 
            stopNow();
            break;}
        }
      }
      if (!checkLeft && !checkRight){
        advance(speedR,speedR);
      }
    }  
  } 
  else{
    if (checkLeft){ 
        for (uint32_t tStart = millis(); (millis() - tStart) < period; ) {
          turn_L(150, 150);
        }
      }
      if (checkRight){ 
        for (uint32_t tStart = millis(); (millis() - tStart) < period; ) {
          turn_R(150, 150);
        }
      }
      if (!checkLeft && !checkRight){
        advance(speedR,speedR);
      }
  }
}
