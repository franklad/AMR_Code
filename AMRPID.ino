const int ER = 5;
const int MR = 4;
const int EL = 6;
const int ML = 7;

const int trigPinR = A0;
const int echoPinR = A1;
const int trigPinL = A2;
const int echoPinL = A3;

const int lineFollowSensorL = 12;
const int lineFollowSensorM = 11;
const int lineFollowSensorR = 10;

const int led = A5;

int speedR = 80, speedL = 80, randNum;

int LFSArray[3]={0, 0, 0};
int error, previousError, P, I, D, PIDvalue, iniMotorPower = 25;
float Kp = 4.5 , Ki = 6.9, Kd = 3.2;

const uint32_t period = 500L;
long distance, duration, rightSensor, leftSensor;
bool debug, minDistance, checkRight, checkLeft;

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

  pinMode(lineFollowSensorL, INPUT);
  pinMode(lineFollowSensorM, INPUT);
  pinMode(lineFollowSensorR, INPUT);

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
void PID(char SR, char SL, uint8_t DR, uint8_t DL) {
 analogWrite(ER, SR);
 digitalWrite(MR, DR);
 analogWrite(EL, SL);
 digitalWrite(ML, DL);
}

void calcPID(){
    P = error;
    I = I + error;
    D = error-previousError;
    PIDvalue = (Kp*P) + (Ki*I) + (Kd*D);
    previousError = error;
}

void setup() {
  Serial.begin(9600);
  initialise();
  debug = false;
}
void loop() {
  randNum = random(500);

  LFSArray[0] = digitalRead(lineFollowSensorL);
  LFSArray[1] = digitalRead(lineFollowSensorM);
  LFSArray[2] = digitalRead(lineFollowSensorR);
  
  rightSensor = SonarSensor(trigPinR, echoPinR);
  delay(10);
  leftSensor = SonarSensor(trigPinL, echoPinL);
  delay(10);

  minDistance = leftSensor > 1 && rightSensor > 1;
  checkRight = rightSensor <= 8;
  checkLeft = leftSensor <= 8;
  
  if (minDistance && checkRight && checkLeft) {
    if((LFSArray[0] == 0) && (LFSArray[1] == 0) && (LFSArray[2] == 1)) error = 2
    if((LFSArray[0] == 0) && (LFSArray[1] == 1) && (LFSArray[2] == 1)) error = 1
    if((LFSArray[0] == 0) && (LFSArray[1] == 1) && (LFSArray[2] == 0)) error = 0
    if((LFSArray[0] == 1) && (LFSArray[1] == 1) && (LFSArray[2] == 0)) error = -1
    if((LFSArray[0] == 1) && (LFSArray[1] == 0) && (LFSArray[2] == 0)) error = -2

    PIDSpeedL = (speedL + calcPID)
    PIDSpeedR = (speedR - calcPID)
    
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
    
    Serial.print(rightSensor);
    Serial.print(" - ");
    Serial.println(leftSensor);   
  }
}


const int LS = 12;
const int MS = 11;
const int RS = 10; 

int SArr[3] = {0,0,0};


void readSensors(){
  SArr[0] = digitalRead(LS);
  SArr[1] = digitalRead(MS);
  SArr[2] = digitalRead(RS);

  if(SArr[0] == 1 && SArr[1] == 0 && SArr[2] == 0) error = 2;
  if(SArr[0] == 1 && SArr[1] == 1 && SArr[2] == 0) error = 1;
  if(SArr[0] == 0 && SArr[1] == 1 && SArr[2] == 0) error = 0;
  if(SArr[0] == 0 && SArr[1] == 1 && SArr[2] == 1) error = -1;
  if(SArr[0] == 0 && SArr[1] == 0 && SArr[2] == 1) error = -2;
  
}

void calculatePID(){
  P = error;
  I = I + error;
  D = error-previousError;
  PIDvalue = (Kp*P) + (Ki*I) + (Kd*D);
  previousError = error;
}

void motorPIDcontrol()
{
  int leftMotorSpeed = 1500 + PIDvalue;
  int rightMotorSpeed = 1500 - PIDvalue;

  Serial.print("Error: ");
  Serial.print(error);
  Serial.print(" - ");
  Serial.print("Left Motor Speed: ");
  Serial.print(leftMotorSpeed);
  Serial.print(" - ");
  Serial.print("Right Motor Speed: ");
  Serial.println(rightMotorSpeed);
}


void setup() {
  Serial.begin(9600);
  pinMode(LS,INPUT);
  pinMode(MS,INPUT);
  pinMode(RS,INPUT);
}

void loop() {
  readSensors();
  calculatePID();
  motorPIDcontrol();
}