#include <Arduino.h>
#include <AccelStepper.h>
#include <Metro.h>

// /**
//  * WARRIOR Code
//  * Last updated: 3/2/2019 at 21:23 by Letti Kittel
//  * */

/*
NOTES FROM LETTI'S TESTING:
  - Fallen parts of Casterly rock are messing with our ulstrasonic senros.
  If we hit the munition button at an angle the back sensor may see that 
  instead of the back wall (which is currently part of our criteria for 
  hitting the munition button). Potential solutions:
    - Add a "WestObstacle" component to the "hit munition button" criteria.
    And if US_F > US_R, move West, (and whatever else you'd need to do)
- Enemy ammunition may cause similar problems (but that would involve a decent
  amount of bad luck and is more than MVP).
- My USB cable uploads code to the Teensy in ~2 s.  I'll put a sticky note
  on it so Amanda doesn't waste any more significants portions of her life. ;)
- The "middle of the board" state is broken. TBD why. --> a wire was hanging
  down in front of the front US.  WE NEED CABLE MANAGEMENT!
- Need to add a "west" component to the munition criteria. It needs to be in 
  the armory and we have a tendacy to veer east when travelling north.
    - "If US_L < 7, drive LEFT; else stop;"
    - Done. Semi tested.
*/

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
// Print out serial US data at a human consumable rate
#define SERIAL_PRINT_INTERVAL 500

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
uint8_t testForMunitionButton();
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
long convertFeedbackToDistance(unsigned long);

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
static Metro serial_print_timer = Metro(SERIAL_PRINT_INTERVAL);

// Pin Assignments
int shooter_enable1 = 2;
int shooter_dir1 = 3;
int shooter_dir2 = 4;
int shooter_enable2 = 5;
int pwmNorthSouth = 6; //UPDATED from Drivetrain code
int pwmEastWest = 10; //UPDATED from Drivetrain code
// On the L298N motor driver, there are two direction pins per motor and they 
// must have opposing polarity for the motor to run.
int dirPin1 = 11; //UPDATED from Drivetrain code, 
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
int maxSpeedMotor = 255;
int stopSpeedMotor = 0;

// Ultrasonic Sensor Obstacle Detection Parameters
int northObstacleThreshold = 5;
int eastObstacleThreshold = 5;
int westObstacleThreshold = 10;
int southernWallFromMunitionButton = 200;
// TODO (letti): Add a south obstacle threshold just in case...

long cm;

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
  
  //Readout of the Ultrasonic sensors
  if (serial_print_timer.check()) {
    int distance_F = readUS_F();
    Serial.print("Front: ");
    Serial.println(distance_F);
    int distance_B = readUS_B();
    Serial.print("Back: ");
    Serial.println(distance_B);
    int distance_R = readUS_R();
    Serial.print("Right: ");
    Serial.println(distance_R);
    int distance_L = readUS_L();
    Serial.print("Left: ");
    Serial.println(distance_L);
  }
  
  //Drivetrain Testing
  /*
  driveW();
  delay(500);
  stopMotors();
  delay(500);
  driveN();
  delay(500);
  stopMotors();
  delay(500);
  driveE();
  delay(500);
  stopMotors();
  delay(500);
  driveS();
  delay(500);
  stopMotors();
  delay(500);*/
  
  //Switch statement for states
  switch (state) {
    case driving_to_munition_button_from_throne_room:
      switch (sub_state) {
        case drivingW:
          driveW();
          if (testForWObstacle()) respToWObstacle();
          break;
        case drivingNArmoury:
          driveN();
          if (testForMunitionButton()) respToMunitionButton();
          break;
        default:
          Serial.println("I'm stuck in a driving_to_munition_button_from_throne_room nested state!");
      }
      break;
    case stopped:
      stopMotors();
      if (testForMunitionTimer()) respToMunitionTimer();
      break;
    case driving_to_crossroads:
      switch (sub_state) {
        case drivingE:
          driveE();
          if (testForEObstacle()) respToEObstacle();
          break;
        case drivingS:
          driveS();
	        if (testForCenter()) respToCenter();
          break;
        default: 
          Serial.println("I'm stuck in a driving_to_crossroads nested state!");
      }
      break;
    case shooting:
      stopMotors();
      startShooter();
      if (testForShooterTimer()) respToShooterTimer();
      break;
    case driving_to_munition_button_from_crossroads:
      switch (sub_state) {
        case drivingN:
          driveN();
		      if (testForNObstacle()) respToNObstacle();
          break;
        case drivingW:
          driveW();
          if (testForWObstacle()) respToWObstacle();
          break;
        case drivingNArmoury:
          driveN();
          if (testForMunitionButton()) respToMunitionButton();
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

float readUS (int trigPin, int echoPin) {
  // Trigger sensor to send pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5); 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW);

  //Read the feedback
  unsigned long duration;
  duration = pulseIn(echoPin, HIGH);
  return convertFeedbackToDistance(duration);
}

float readUS_F(){
  return readUS(US_F_TRIG_Pin, US_F_ECHO_Pin);
}

float readUS_B(){
  return readUS(US_B_TRIG_Pin, US_B_ECHO_Pin);
}

float readUS_R(){
  return readUS(US_R_TRIG_Pin, US_R_ECHO_Pin);
}

float readUS_L(){
  return readUS(US_L_TRIG_Pin, US_L_ECHO_Pin);
}

long convertFeedbackToDistance(unsigned long duration) {
  // TODO(Amanda): Add a comment describing the 29.1 value.
  cm = (duration/2) / 29.1;
  return cm;
}

void driveW(){
  digitalWrite(dirPin1, LOW);
  digitalWrite(dirPin2, HIGH);
  analogWrite(pwmNorthSouth, stopSpeedMotor);
  analogWrite(pwmEastWest, maxSpeedMotor);
}
void driveN(){
  digitalWrite(dirPin1, HIGH);
  digitalWrite(dirPin2, LOW);
  analogWrite(pwmNorthSouth, maxSpeedMotor);
  analogWrite(pwmEastWest, stopSpeedMotor); 
}
void driveS(){
  digitalWrite(dirPin1, LOW);
  digitalWrite(dirPin2, HIGH);
  analogWrite(pwmNorthSouth, maxSpeedMotor);
  analogWrite(pwmEastWest, stopSpeedMotor); 
}
void driveE(){
  digitalWrite(dirPin1, HIGH);
  digitalWrite(dirPin2, LOW);
  analogWrite(pwmNorthSouth, stopSpeedMotor);
  analogWrite(pwmEastWest, maxSpeedMotor); 
}
void stopMotors(){
  analogWrite(pwmNorthSouth, stopSpeedMotor);
  analogWrite(pwmEastWest, stopSpeedMotor);
}

uint8_t testForWObstacle() {
  float US_L = readUS_L();
	if (US_L < eastObstacleThreshold) return 1;
  else return 0; //UPDATE AFTER TEST
}

void respToWObstacle() {
  Serial.println("W Obstacle Detected!");
  stopMotors();
  sub_state = drivingNArmoury;
}

uint8_t testForNObstacle() {
  float US_F = readUS_F();
	if (US_F < northObstacleThreshold) return 1;
  else return 0; //UPDATE AFTER TEST
}

void respToNObstacle() {
  Serial.println("N Obstacle Detected!");
  stopMotors();
  sub_state = drivingW;
}

uint8_t testForEObstacle() {
  float US_R = readUS_R();
	if (US_R < eastObstacleThreshold) return 1;
  else return 0; //UPDATE AFTER TEST
}
void respToEObstacle() {
  Serial.println("E Obstacle Detected!");
  stopMotors();
  sub_state = drivingS;
}
uint8_t testForMunitionButton() {
  float US_F = readUS_F();
  float US_B = readUS_B();
  float US_L = readUS_L();
  if ((US_B > southernWallFromMunitionButton) && (US_F < northObstacleThreshold)) {
    // Ensure that we are in the corner and well within the bounds of the armoury.
    if (US_L > 7) {
      driveE();
      delay(50);  // blocking code. TO BE REPLACED.
    }
    driveN();
    // Ultrasonic sensors can be less than accurate. This delay is to ensure we 
    // continue driving north and hit the munition button.
    delay(200); //blocking code
    return 1;
  }
  else return 0;
}
void respToMunitionButton() {
  munition_timer.reset();
  driveS();
  delay(50); //blocking code
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
  //TODO: add release code for the release mechanism
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
  Serial.println("Shooting timer has expired!");
  digitalWrite(shooter_dir1, LOW);
  digitalWrite(shooter_dir2, LOW);
  analogWrite(shooter_enable1, stopSpeedMotor);
  analogWrite(shooter_enable2, stopSpeedMotor); 
  state = driving_to_munition_button_from_crossroads;
  sub_state = drivingN;
}
