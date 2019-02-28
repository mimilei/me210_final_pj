#include <Arduino.h>
#include <AccelStepper.h>
#include <Metro.h>

// /**
//  * Running the Ultrasonic Sensors
//  * Last updated: 2/27/2019 at 19:48 by Amanda Steffy
//  * */

/*
Nice to have:
 - combine readUS_N into single function with variable
*/

/*---------------Module Defines-----------------------------*/
//Munition Loading Time
#define MUNITION_TIME_INTERVAL 5000
//Shooting time for six wildfires
#define SHOOTER_TIME_INTERVAL 8000

/*---------------Module Function Prototypes-----------------*/
float readUS_F();
float readUS_B();
float readUS_R();
float readUS_L();

/*---------------State Definitions--------------------------*/
typedef enum {
 STATE_MOVE_FORWARD, STATE_MOVE_BACKWARD, STATE_STOPPED, STATE_TURN
} States_t;

/*---------------Module Variables---------------------------*/
States_t state;

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

/*---------------Raptor Main Functions----------------*/
void setup() {
  Serial.begin(9600);
  pinMode(US_F_TRIG_Pin, OUTPUT);
  pinMode(US_F_ECHO_Pin, INPUT);
  pinMode(US_B_TRIG_Pin, OUTPUT);
  pinMode(US_B_ECHO_Pin, INPUT);
  pinMode(US_R_TRIG_Pin, OUTPUT);
  pinMode(US_R_ECHO_Pin, INPUT);
  pinMode(US_L_TRIG_Pin, OUTPUT);
  pinMode(US_L_ECHO_Pin, INPUT);
}

void loop() {
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