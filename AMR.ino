const int ER = 5;
const int MR = 4;
const int EL = 6;
const int ML = 7;

const int trigPinR = A0;
const int echoPinR = A1;
const int trigPinL = A2;
const int echoPinL = A3;

const int topRSensor = 12;
const int topLSensor = 9;
const int bottomRSensor = 11;
const int bottomLSensor = 10;
const int midTopSensor = 8;
const int sideRSensor = 3;
const int sideLSensor = 2;

const int led = A5;
const int potPin = A4;

int speedR, mapedVal, speedL, randNum;
const uint32_t period = 500L;
long distance, duration, rightSensor, leftSensor;
bool debug, minDistance, checkRight, checkLeft;
bool topRActive, topLActive, bottomRActive, bottomLActive, midTopActive, sideRActive, sideLActive;

long SonarSensor(int trigPin, int echoPin) {
 digitalWrite(trigPin, LOW);
 delayMicroseconds(2);
 digitalWrite(trigPin, HIGH);
 delayMicroseconds(10);
 digitalWrite(trigPin, LOW);
 duration = pulseIn(echoPin, HIGH);
 return (duration / 2) / 29.1;
}
void initialise() {
  pinMode(trigPinR, OUTPUT);
  pinMode(echoPinR, INPUT);
  pinMode(trigPinL, OUTPUT);
  pinMode(echoPinL, INPUT);

  pinMode(topRSensor, INPUT_PULLUP);
  pinMode(topLSensor, INPUT_PULLUP);
  pinMode(bottomRSensor, INPUT_PULLUP);
  pinMode(bottomLSensor, INPUT_PULLUP);
  pinMode(midTopSensor, INPUT_PULLUP);
  pinMode(sideRSensor, INPUT_PULLUP);
  pinMode(sideLSensor, INPUT_PULLUP);

  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
  delay(100);
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
  delay(100);
  digitalWrite(led, HIGH);
}
void stop() {
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
   digitalWrite(led, HIGH);
   delay(100);
   digitalWrite(led, LOW);
   delay(50);
 }
 digitalWrite(led, HIGH);
}
void turn_L60Deg() {
 for (uint32_t tStart = millis(); (millis() - tStart) < period; ) {
   turn_L(150, 150);
   digitalWrite(led, HIGH);
   delay(100);
   digitalWrite(led, LOW);
   delay(50);
 }
 digitalWrite(led, HIGH);
}
void adjustR(){
  for (uint32_t tStart = millis(); (millis() - tStart) < 350L; ) {
    back_off(speedR, speedL + 20);
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);
    delay(50);
  }
  turn_R(100, 100);
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
  delay(50);
  digitalWrite(led, HIGH);
}
void adjustL() {
  for (uint32_t tStart = millis(); (millis() - tStart) < 350L; ) {
    back_off(speedR + 20, speedL);
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);
    delay(50);
  }
  turn_L(100, 100);
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
  delay(50);
  digitalWrite(led, HIGH);
}

void setup() {
  Serial.begin(9600);
  initialise();
  debug = false;
}
void loop() {
  randNum = random(500);
  speedR = map(analogRead(potPin),0,1023,100,250);
  speedL = speedR;
  rightSensor = SonarSensor(trigPinR, echoPinR);
  delay(50);
  leftSensor = SonarSensor(trigPinL, echoPinL);
  delay(50);
  minDistance = leftSensor > 1 && rightSensor > 1;
  checkRight = rightSensor <= 8;
  checkLeft = leftSensor <= 8;
  
  if (minDistance && checkRight && checkLeft) {
    topRActive = digitalRead(topRSensor) == LOW;
    midTopActive = digitalRead(midTopSensor) == LOW;
    topLActive = digitalRead(topLSensor) == LOW;
    bottomRActive = digitalRead(bottomRSensor) == LOW;
    bottomLActive = digitalRead(bottomLSensor) == LOW;
    sideRActive = digitalRead(sideRSensor) == HIGH;
    sideLActive = digitalRead(sideLSensor) == HIGH;

    if (!(topRActive || topLActive) && !(bottomRActive || bottomLActive) && !midTopActive) {
      advance(speedR, speedL);
    }
    if ((!topRActive && !topLActive && !midTopActive) && (bottomRActive || bottomLActive)) {
      advance(speedR, speedL);
    }
    if ((topRActive || topLActive || midTopActive) && (!bottomRActive && !bottomLActive)) {
      for (uint32_t tStart = millis(); (millis() - tStart) < period; ) {
        back_off(speedR, speedL);
        digitalWrite(led, HIGH);
        delay(100);
        digitalWrite(led, LOW);
        delay(50);
      }
      digitalWrite(led, HIGH);
      if (randNum % 2 == 0) {
        turn_R60Deg();
      } else {
        turn_L60Deg();
      }
    }
    if (((topRActive && bottomRActive) || sideRActive) && !(midTopActive || topLActive || bottomLActive)) {
      adjustR();
    }
    if (((topLActive && bottomLActive) || sideLActive) && !(midTopActive || topRActive || bottomRActive)) {
      adjustL();
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
    Serial.print(digitalRead(topRSensor) == LOW);
    Serial.print(" - ");
    Serial.print(digitalRead(midTopSensor) == LOW);
    Serial.print(" - ");
    Serial.println(digitalRead(topLSensor) == LOW);
    Serial.print(digitalRead(sideRSensor) == HIGH);
    Serial.print(digitalRead(bottomRSensor) == LOW);
    Serial.print(" - ");
    Serial.print("0");
    Serial.print(" - ");
    Serial.print(digitalRead(bottomLSensor) == LOW);
    Serial.println(digitalRead(sideRSensor) == HIGH);
    Serial.println(" - - - - - - - - - - - - - - - - - - - - - - - - - - - ");
    delay(300);
    Serial.print(rightSensor);
    Serial.print(" - ");
    Serial.println(leftSensor);   
  }
}
