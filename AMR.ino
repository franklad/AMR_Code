const int ER = 5;
const int MR = 4;
const int EL = 6;
const int ML = 7;

const int trigPinR = A0;
const int echoPinR = A1;
const int trigPinL = A2;
const int echoPinL = A3;

const int topIRL = 12;
const int topIRM = 11;
const int topIRR = 10;

const int sideL = 9;
const int sideR = 8;

const int fan = A5;
const int potPin = A4;

int speedR, mapedVal, speedL, randNum;
const uint32_t period = 500L;
long distance, duration, rightSensor, leftSensor;
bool debug, minDistance, checkRight, checkLeft, fireL, fireM, fireR, isSideR, isSideL;

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

  pinMode(sideL, INPUT_PULLUP);
  pinMode(sideR, INPUT_PULLUP);
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
}
void loop() {
  randNum = random(300);
  speedR = map(analogRead(potPin),0,1023,100,250);
  speedL = speedR + 10;
  rightSensor = SonarSensor(trigPinR, echoPinR);
  delay(50);
  leftSensor = SonarSensor(trigPinL, echoPinL);
  delay(50);
  minDistance = leftSensor > 1 && rightSensor > 1;
  checkRight = rightSensor <= 8;
  checkLeft = leftSensor <= 8;
  fireL = (digitalRead(topIRL) == 1);
  fireM = (digitalRead(topIRM) == 1);
  fireR = (digitalRead(topIRR) == 1);
  isSideL = (digitalRead(sideL) == 1);
  isSideR = (digitalRead(sideR) == 1);
  
  if (minDistance && checkRight && checkLeft) {
    if (!isSideL && !isSideR){
    if (fireL && fireM && fireR){
      stopNow();
      fan_start();
    } 
    else if (!fireR && (fireL || (fireL && fireM))){
      for (uint32_t tStart = millis(); (millis() - tStart) < 200; ) {
        turn_R(150, 150);
      }
      stopNow();
      fan_start();
    }
    
    else if(!fireL && (fireR || (fireR && fireM)) ){
      for (uint32_t tStart = millis(); (millis() - tStart) < 200; ) {
        turn_L(150, 150);
      }
      stopNow();
      fan_start();
    }
    else if(!fireL && !fireM && !fireR){
      fan_stop();
      advance(speedR, speedL);
    }
    }
    if (!isSideL && isSideR){
      turn_R(150, 150);
    }
    if (isSideL && !isSideR){
      turn_L(150, 150);
    }
    if (isSideL && isSideR){
      for (uint32_t tStart = millis(); (millis() - tStart) < 500; ) {
        back_off(150, 150);
      }
      if (randNum%2 == 1){
        turn_R60Deg();
      } else {turn_R60Deg();}
    }
  }
  if (!checkRight) {
    turn_L60Deg();
  }
  if (!checkLeft) {
    turn_R60Deg();
  }   
  if (!checkRight && !checkLeft) {
    turn_R60Deg();
    turn_R60Deg();
  }

  if (debug) {
    Serial.print(digitalRead(topIRL));
    Serial.print(" - ");
    Serial.print(digitalRead(topIRM));
    Serial.print(" - ");
    Serial.println(digitalRead(topIRR));
    Serial.println(" ---------------------------------- ");
    Serial.print(rightSensor);
    Serial.print(" - ");
    Serial.println(leftSensor);
  }
}
