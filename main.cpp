#include <Arduino.h>
#include <AccelStepper.h>
#include <Metro.h>

// /**
//  * WARRIOR Code
//  * Last updated: 3/2/2019 at 14:41 by Amanda Steffy
//  * */

/*
Nice to have:
 - combine readUS_N into single function with variable
 - cleaner way to handle substates
*/

/*---------------Module Defines-----------------------------*/
//Munition Loading Time
#define MUNITION_TIME_INTERVAL 5000
//Shooting time for six wildfires
#define SHOOTER_TIME_INTERVAL 8000

/*---------------Module Function Prototypes-----------------*/
//Ultrasonic sensor read functions
float readUS_F();
float readUS_B();
float readUS_R();
float readUS_L();
//Drive functions
void driveW();
void driveN();
void driveS();
void driveE();
void stopMotors();
//Obstacle testing and response
uint8_t testForWObstacle();
void respToWObstacle();
uint8_t testForNObstacle();
void respToNObstacle();
uint8_t testForEObstacle();
void respToEObstacle();
void respToMunitionButton();
//Crossroads testing and response
uint8_t testForCenter();
void respToCenter();
//Shooter start
void startShooter();
//Timer testings and response
uint8_t testForMunitionTimer();
void respToMunitionTimer();
uint8_t testForShooterTimer();
void respToShooterTimer();

/*---------------State Definitions--------------------------*/
typedef enum {
  driving_to_munition_button_from_throne_room, stopped, driving_to_crossroads, shooting, driving_to_munition_button_from_crossroads
} States_t;

typedef enum {
 drivingW, drivingN, drivingE, drivingS, drivingNArmoury
} Sub_states_t;

/*---------------Module Variables---------------------------*/
States_t state;
Sub_states_t sub_state;

//Timer Assignments
static Metro munition_timer = Metro(MUNITION_TIME_INTERVAL);
static Metro shooter_timer = Metro(SHOOTER_TIME_INTERVAL);

// Pin Assignments
int shooter_enable1 = 2;
int shooter_dir1 = 3;
int shooter_dir2 = 4;
int shooter_enable2 = 5;
int pwmNorthSouth = 6; //UPDATED from Drivetrain code
int pwmEastWest = 10; //UPDATED from Drivetrain code
int dirPin1 = 11; //UPDATED from Drivetrain code, On the L298N motor driver, there are two direction pins per motor and they must have opposing polarity for the motor to run.
int dirPin2 = 12; //UPDATED from Drivetrain code
int US_F_TRIG_Pin = 16;
int US_F_ECHO_Pin = 17;
int US_B_TRIG_Pin = 18;
int US_B_ECHO_Pin = 19;
int US_R_TRIG_Pin = 20;
int US_R_ECHO_Pin = 21;
int US_L_TRIG_Pin = 22;
int US_L_ECHO_Pin = 23;

//Motor Settings
int maxSpeedMotor = 255; //HELP! SHOULD THIS BE 200? Letti got it to work at 200 but not higher
int stopSpeedMotor = 0;
//int shooterDirState = LOW;

/*---------------WARRIOR Main Functions----------------*/
void setup() {
  //Begin Serial Monitor
  Serial.begin(9600);
  //Initialize Teensy pins
  pinMode(shooter_enable1, OUTPUT);
  pinMode(shooter_dir1, OUTPUT);
  pinMode(shooter_dir2, OUTPUT);
  pinMode(shooter_enable2, OUTPUT);
  pinMode(pwmNorthSouth, OUTPUT);
  pinMode(pwmEastWest, OUTPUT);
  pinMode(dirPin1, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  pinMode(US_F_TRIG_Pin, OUTPUT);
  pinMode(US_F_ECHO_Pin, INPUT);
  pinMode(US_B_TRIG_Pin, OUTPUT);
  pinMode(US_B_ECHO_Pin, INPUT);
  pinMode(US_R_TRIG_Pin, OUTPUT);
  pinMode(US_R_ECHO_Pin, INPUT);
  pinMode(US_L_TRIG_Pin, OUTPUT);
  pinMode(US_L_ECHO_Pin, INPUT);
  //Initialize motor pin settings
  digitalWrite(shooter_dir1, LOW);
  digitalWrite(shooter_dir2, LOW);
  analogWrite(shooter_enable1, stopSpeedMotor);
  analogWrite(shooter_enable2, stopSpeedMotor);
  analogWrite(pwmNorthSouth, stopSpeedMotor);
  analogWrite(pwmEastWest, stopSpeedMotor);
  digitalWrite(dirPin1, HIGH);
  digitalWrite(dirPin2, LOW);
  //Initialize variables
  state = driving_to_munition_button_from_throne_room;
  sub_state = drivingW;
}

void loop() {
  /*
  //Readout of the Ultrasonic sensors
  int distance_F = readUS_F();
  Serial.print("Front: ");
  Serial.println(distance_F);
  delay(500);
  int distance_B = readUS_B();
  Serial.print("Back: ");
  Serial.println(distance_B);
  delay(500);
  int distance_R = readUS_R();
  Serial.print("Right: ");
  Serial.println(distance_R);
  delay(500);
  int distance_L = readUS_L();
  Serial.print("Left: ");
  Serial.println(distance_L);
  delay(500);*/
  
  //Testing in the loop

  //Switch statement for states
  switch (state) {
    case driving_to_munition_button_from_throne_room:
      switch (sub_state) {
        case drivingW:
          //TODO: test driveW
          driveW();
          if (testForWObstacle()) respToWObstacle();
          break;
        case drivingNArmoury:
          //TODO: test driveN
          driveN();
          if (testForNObstacle()) respToMunitionButton();
          break;
        default:
          Serial.println("I'm stuck in a driving_to_munition_button_from_throne_room nested state!");
      }
      break;
    case stopped:
      //TODO: test stopMotors
      stopMotors();
      if (testForMunitionTimer()) respToMunitionTimer();
      break;
    case driving_to_crossroads:
      switch (sub_state) {
        case drivingE:
          //TODO: test driveE
          driveE();
          if (testForEObstacle()) respToEObstacle();
          break;
        case drivingS:
          //TODO: test driveS
          driveS();
	        if (testForCenter()) respToCenter();
          break;
        default: 
          Serial.println("I'm stuck in a driving_to_crossroads nested state!");
      }
      break;
    case shooting:
      //TODO: test stopMotors
      stopMotors();
      startShooter();
      if (testForShooterTimer()) respToShooterTimer();
      break;
    case driving_to_munition_button_from_crossroads:
      switch (sub_state) {
        case drivingN:
          //TODO: test driveN
          driveN();
		      if (testForNObstacle()) respToNObstacle();
          break;
        case drivingW:
          //TODO: test driveW
          driveW();
          if (testForWObstacle()) respToWObstacle();
          break;
        case drivingNArmoury:
          //TODO: test driveN
          driveN();
          if (testForNObstacle()) respToMunitionButton();
          break;
        default:
          Serial.println("I'm stuck in a driving_to_munition_button_from_crossroads nested state!");
      }
      break;
    default: // Should never get into an unhandled state
      Serial.println("What is this I do not even...");
  }
}

/*----------------Module Functions--------------------------*/
float readUS_F(){
  // Trigger sensor to send pulse
  digitalWrite(US_F_TRIG_Pin, LOW);
  delayMicroseconds(5); //Note: blocking code
  digitalWrite(US_F_TRIG_Pin, HIGH);
  delayMicroseconds(10); //Note: blocking code
  digitalWrite(US_F_TRIG_Pin, LOW);

  //Read the feedback
  unsigned long duration;
  duration = pulseIn(US_F_ECHO_Pin, HIGH);

  //Convert feedback to distance
  long cm;
  cm = (duration/2) / 29.1;
  return cm;
}

float readUS_B(){
  // Trigger sensor to send pulse
  digitalWrite(US_B_TRIG_Pin, LOW);
  delayMicroseconds(5); //Note: blocking code
  digitalWrite(US_B_TRIG_Pin, HIGH);
  delayMicroseconds(10); //Note: blocking code
  digitalWrite(US_B_TRIG_Pin, LOW);

  //Read the feedback
  unsigned long duration;
  duration = pulseIn(US_B_ECHO_Pin, HIGH);

  //Convert feedback to distance
  long cm;
  cm = (duration/2) / 29.1;
  return cm;
}

float readUS_R(){
  // Trigger sensor to send pulse
  digitalWrite(US_R_TRIG_Pin, LOW);
  delayMicroseconds(5); 
  digitalWrite(US_R_TRIG_Pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(US_R_TRIG_Pin, LOW);

  //Read the feedback
  unsigned long duration;
  duration = pulseIn(US_R_ECHO_Pin, HIGH);

  //Convert feedback to distance
  long cm;
  cm = (duration/2) / 29.1;
  return cm;
}

float readUS_L(){
  // Trigger sensor to send pulse
  digitalWrite(US_L_TRIG_Pin, LOW);
  delayMicroseconds(5); //Note: blocking code
  digitalWrite(US_L_TRIG_Pin, HIGH);
  delayMicroseconds(10); //Note: blocking code
  digitalWrite(US_L_TRIG_Pin, LOW);

  //Read the feedback
  unsigned long duration;
  duration = pulseIn(US_L_ECHO_Pin, HIGH);

  //Convert feedback to distance
  long cm;
  cm = (duration/2) / 29.1;
  return cm;
}

void driveW(){
  //TODO: test
  digitalWrite(dirPin1, LOW);
  digitalWrite(dirPin2, HIGH);
  analogWrite(pwmNorthSouth, stopSpeedMotor);
  analogWrite(pwmEastWest, maxSpeedMotor); 
}
void driveN(){
  //TODO: test
  digitalWrite(dirPin1, HIGH);
  digitalWrite(dirPin2, LOW);
  analogWrite(pwmNorthSouth, maxSpeedMotor);
  analogWrite(pwmEastWest, stopSpeedMotor); 
}
void driveS(){
  //TODO: test
  digitalWrite(dirPin1, LOW);
  digitalWrite(dirPin2, HIGH);
  analogWrite(pwmNorthSouth, maxSpeedMotor);
  analogWrite(pwmEastWest, stopSpeedMotor); 
}
void driveE(){
  //TODO: test
  digitalWrite(dirPin1, HIGH);
  digitalWrite(dirPin2, LOW);
  analogWrite(pwmNorthSouth, stopSpeedMotor);
  analogWrite(pwmEastWest, maxSpeedMotor); 
}
void stopMotors(){
  //TODO: test
  analogWrite(pwmNorthSouth, stopSpeedMotor);
  analogWrite(pwmEastWest, stopSpeedMotor);
}

uint8_t testForWObstacle() {
  float US_L = readUS_L();
	if (US_L < 5) return 1;
  else return 0; //UPDATE AFTER TEST
}

void respToWObstacle() {
  Serial.println("W Obstacle Detected!");
  sub_state = drivingNArmoury;
}

uint8_t testForNObstacle() {
  float US_F = readUS_F();
	if (US_F < 5) return 1;
  else return 0; //UPDATE AFTER TEST
}

void respToNObstacle() {
  Serial.println("N Obstacle Detected!");
  sub_state = drivingW;
}

uint8_t testForEObstacle() {
  float US_R = readUS_R();
	if (US_R < 5) return 1;
  else return 0; //UPDATE AFTER TEST
}
void respToEObstacle() {
  Serial.println("E Obstacle Detected!");
  sub_state = drivingS;
}
void respToMunitionButton() {
  munition_timer.reset();
  state = stopped;
  Serial.println("Responding to Munition Button!");
}
uint8_t testForCenter() {
  float US_F = readUS_F();
	float US_B = readUS_B();
	if (US_F == US_B) return 1;
  else return 0;
}
void respToCenter() {
  Serial.println("WARRIOR is at the Center");
  shooter_timer.reset();
  state = shooting;
}
void startShooter() {
  //TODO: test
  Serial.println("FIRE!");
  digitalWrite(shooter_dir1, HIGH);
  digitalWrite(shooter_dir2, HIGH);
  analogWrite(shooter_enable1, maxSpeedMotor);
  analogWrite(shooter_enable2, maxSpeedMotor);
}
uint8_t testForMunitionTimer() {
  return (uint8_t) munition_timer.check();
}
void respToMunitionTimer() {
  Serial.println("Munition timer has expired!");
  state = driving_to_crossroads;
  sub_state = drivingE;
}
uint8_t testForShooterTimer() {
  return (uint8_t) shooter_timer.check();
}
void respToShooterTimer() {
  //TODO: test
  Serial.println("Shooting timer has expired!");
  digitalWrite(shooter_dir1, LOW);
  digitalWrite(shooter_dir2, LOW);
  analogWrite(shooter_enable1, stopSpeedMotor);
  analogWrite(shooter_enable2, stopSpeedMotor); 
  state = driving_to_munition_button_from_crossroads;
  sub_state = drivingN;
}