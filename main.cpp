#include <Arduino.h>
#include <AccelStepper.h>
#include <Metro.h>

// /**
//  * Running the Ultrasonic Sensors
//  * Last updated: 2/27/2019 at 21:05 by Amanda Steffy
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
int US_F_TRIG_Pin = 16;
int US_F_ECHO_Pin = 17;
int US_B_TRIG_Pin = 18;
int US_B_ECHO_Pin = 19;
int US_R_TRIG_Pin = 20;
int US_R_ECHO_Pin = 21;
int US_L_TRIG_Pin = 22;
int US_L_ECHO_Pin = 23;

/*---------------WARRIOR Main Functions----------------*/
void setup() {
  //Begin Serial Monitor
  Serial.begin(9600);
  //Initialize Teensy pins
  pinMode(US_F_TRIG_Pin, OUTPUT);
  pinMode(US_F_ECHO_Pin, INPUT);
  pinMode(US_B_TRIG_Pin, OUTPUT);
  pinMode(US_B_ECHO_Pin, INPUT);
  pinMode(US_R_TRIG_Pin, OUTPUT);
  pinMode(US_R_ECHO_Pin, INPUT);
  pinMode(US_L_TRIG_Pin, OUTPUT);
  pinMode(US_L_ECHO_Pin, INPUT);
  //Initialize variables
  state = driving_to_munition_button_from_throne_room;
}

void loop() {
  /*
  //Readout of the Ultrasonic sensors
  int distance_F = readUS_F();
  Serial.print("Front: ");
  Serial.println(distance_F);
  delay(1000);
  int distance_B = readUS_B();
  Serial.print("Back: ");
  Serial.println(distance_B);
  delay(1000);
  int distance_R = readUS_R();
  Serial.print("Right: ");
  Serial.println(distance_R);
  delay(1000);
  int distance_L = readUS_L();
  Serial.print("Left: ");
  Serial.println(distance_L);
  delay(1000);
  */
  //Testing in the loop
  if (testForWObstacle()) respToWObstacle();
  if (testForNObstacle()) respToNObstacle();
  if (testForEObstacle()) respToEObstacle();
  //Switch statement for states
  switch (state) {
    case driving_to_munition_button_from_throne_room:
      switch (sub_state) {
        case drivingW:
          //TODO
          break;
        case drivingNArmoury:
          //TODO
          break;
        default:
          Serial.println("I'm stuck in a driving_to_munition_button_from_throne_room nested state!");
      }
      break;
    case stopped:
      //TODO
      break;
    case driving_to_crossroads:
      switch (sub_state) {
        case drivingE:
          //TODO
          break;
        case drivingS:
          //TODO
          break;
        default: 
          Serial.println("I'm stuck in a driving_to_crossroads nested state!");
      }
      break;
    case shooting:
      //TODO
      break;
    case driving_to_munition_button_from_crossroads:
      switch (sub_state) {
        case drivingN:
          //TODO
          break;
        case drivingW:
          //TODO
          break;
        case drivingNArmoury:
          //TODO
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
  //TODO
}
void driveN(){
  //TODO
}
void driveS(){
  //TODO
}
void driveE(){
  //TODO
}
void stopMotors(){
  //TODO
}

uint8_t testForWObstacle() {
  float US_L = readUS_L();
	if (US_L < 5) return 1;
  else return 0; //UPDATE AFTER TEST
}

void respToWObstacle() {
  //TODO
  Serial.println("W Obstacle Detected!");
}

uint8_t testForNObstacle() {
  //TODO
  float US_F = readUS_F();
	if (US_F < 5) return 1;
  else return 0; //UPDATE AFTER TEST
}

void respToNObstacle() {
  //TODO
  Serial.println("N Obstacle Detected!");
}

uint8_t testForEObstacle() {
  //TODO
  float US_R = readUS_R();
	if (US_R < 5) return 1;
  else return 0; //UPDATE AFTER TEST
}
void respToEObstacle() {
  //TODO
  Serial.println("E Obstacle Detected!");
}
void respToMunitionButton() {
  //TODO
}
uint8_t testForCenter() {
  //TODO
}
void respToCenter() {
  //TODO
}
void startShooter() {
  //TODO
}
uint8_t testForMunitionTimer() {
  //TODO
}
void respToMunitionTimer() {
  //TODO
}
uint8_t testForShooterTimer() {
  //TODO
}
void respToShooterTimer() {
  //TODO
}