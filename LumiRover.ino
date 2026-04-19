/*
 * PROJECT: LumiRover Prototype (Scale Model)
 * CONTROLLER: MIT App Inventor (Bluetooth on Serial1)
 * UPGRADES: Omni-Directional Mecanum Drive, Smooth Servo, LEDs, TRUE E-STOP
 */

#include <Servo.h>

// --- 1. PIN DEFINITIONS ---
// FRONT LEFT Wheel (TB6612FNG 1)
#define FL_PWM_PIN 2   
#define FL_IN1_PIN 22  
#define FL_IN2_PIN 23  

// REAR LEFT Wheel (TB6612FNG 1)
#define RL_PWM_PIN 3   
#define RL_IN1_PIN 24  
#define RL_IN2_PIN 25  

// FRONT RIGHT Wheel (TB6612FNG 2)
#define FR_PWM_PIN 8   
#define FR_IN1_PIN 36  
#define FR_IN2_PIN 37  

// REAR RIGHT Wheel (TB6612FNG 2)
#define RR_PWM_PIN 9   
#define RR_IN1_PIN 38  
#define RR_IN2_PIN 39  

#define STBY_PIN  26  // Standby for BOTH TB6612FNG drivers

// BTS7960 Spreader Motor
#define SPR_EN_PIN 27   
#define SPR_RPWM_PIN 4  
#define SPR_LPWM_PIN 5  

// Grove Servo (Plow)
#define PLOW_SERVO_PIN 11

// Safety Stall Sensors
#define SENSOR_PIN_L A0
#define SENSOR_PIN_R A1

// --- GROVE LED PINS ---
#define LED_BLUE  32  // Standby / Ready
#define LED_GREEN 33  // Moving / Working
#define LED_RED   34  // Stalled / Error

// --- 2. CONFIGURATION ---
// Set to 'true' ONLY when potentiometers/current sensors are physically plugged in
bool USE_SAFETY_SENSORS = false; 

// Speed Settings (Voltage limit for 6V motors on 11.1V battery)
const int SPEED_SLOW = 60;
const int SPEED_NORMAL = 90;
const int SPEED_FAST = 130;  // MAX SAFE LIMIT
int currentDriveSpeed = SPEED_NORMAL; // Default starting speed

const int SPREADER_SPEED = 200; 
const int STALL_LIMIT = 800;    

// --- 3. OBJECTS & VARIABLES ---
Servo plowServo;

// Servo Speed Control Variables
int currentPlowAngle = 0;       
int targetPlowAngle = 0;        
unsigned long lastServoTime = 0; 
const int SERVO_SPEED = 20; // ms per degree (Higher = Slower)    

// State Machine
enum State { STOPPED, STOPPED_DRIVING, FORWARD, BACKWARD, LEFT, RIGHT, STRAFE_LEFT, STRAFE_RIGHT, STALLED };
State currentState = STOPPED;

bool isSpreaderOn = false; // Tracks spreader for the Green LED

void setup() {
  Serial.begin(9600);  // Laptop USB Debugging
  Serial1.begin(9600); // HC-05 Bluetooth
  Serial.println("LumiRover Omni-Directional System Booting...");
  
  // Init LEDs
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  updateLEDs(); 

  // Init Drive Pins
  pinMode(FL_PWM_PIN, OUTPUT); pinMode(FL_IN1_PIN, OUTPUT); pinMode(FL_IN2_PIN, OUTPUT);
  pinMode(RL_PWM_PIN, OUTPUT); pinMode(RL_IN1_PIN, OUTPUT); pinMode(RL_IN2_PIN, OUTPUT);
  pinMode(FR_PWM_PIN, OUTPUT); pinMode(FR_IN1_PIN, OUTPUT); pinMode(FR_IN2_PIN, OUTPUT);
  pinMode(RR_PWM_PIN, OUTPUT); pinMode(RR_IN1_PIN, OUTPUT); pinMode(RR_IN2_PIN, OUTPUT);
  
  pinMode(STBY_PIN, OUTPUT);
  digitalWrite(STBY_PIN, HIGH); // Wake up both drivers

  // Init Spreader Pins
  pinMode(SPR_EN_PIN, OUTPUT);
  pinMode(SPR_RPWM_PIN, OUTPUT); pinMode(SPR_LPWM_PIN, OUTPUT);
  digitalWrite(SPR_EN_PIN, HIGH); // Wake up spreader driver

  // Init Servo
  plowServo.attach(PLOW_SERVO_PIN);
  plowServo.write(0); 
  
  stopRobot(); // Start in a safe state
}

void loop() {
  // --- A. READ BLUETOOTH COMMANDS ---
  if (Serial1.available() > 0) {
    char cmd = Serial1.read();
    processCommand(cmd);
  }

  // --- B. SAFETY LOGIC ---
  if (USE_SAFETY_SENSORS) {
    int loadL = analogRead(SENSOR_PIN_L);
    int loadR = analogRead(SENSOR_PIN_R);
    
    // Check if we are moving forward or sideways (we ignore backing up so we can escape)
    bool movingIntoDanger = (currentState == FORWARD || currentState == LEFT || currentState == RIGHT || currentState == STRAFE_LEFT || currentState == STRAFE_RIGHT);

    if (movingIntoDanger && (loadL > STALL_LIMIT || loadR > STALL_LIMIT)) {
      if (currentState != STALLED) {
        Serial.println("!!! STALL DETECTED !!!");
        Serial1.println("STALL DETECTED! Press B to reverse."); 
        stopRobot();
      }
      currentState = STALLED;
    }
  }

  // --- C. SMOOTH SERVO CONTROL ---
  if (currentPlowAngle != targetPlowAngle) {
    if (millis() - lastServoTime >= SERVO_SPEED) {
      lastServoTime = millis(); 
      if (currentPlowAngle < targetPlowAngle) {
        currentPlowAngle++; 
      } else {
        currentPlowAngle--;
      }
      plowServo.write(currentPlowAngle); 
    }
  }

  // --- D. UPDATE VISUAL INDICATORS ---
  updateLEDs();

  // --- E. EXECUTE MOTION (True 4-Wheel Omni Kinematics) ---
  switch (currentState) {
    // moveRobot4W(speed, FrontLeft_Fwd, RearLeft_Fwd, FrontRight_Fwd, RearRight_Fwd)
    case FORWARD:      moveRobot4W(currentDriveSpeed, true,  true,  true,  true);  break;
    case BACKWARD:     moveRobot4W(currentDriveSpeed, false, false, false, false); break;
    case LEFT:         moveRobot4W(currentDriveSpeed, false, false, true,  true);  break; // Turn in place
    case RIGHT:        moveRobot4W(currentDriveSpeed, true,  true,  false, false); break; // Turn in place
    
    // TRUE STRAFING (Requires Mecanum Wheels)
    case STRAFE_LEFT:  moveRobot4W(currentDriveSpeed, false, true,  true,  false); break; 
    case STRAFE_RIGHT: moveRobot4W(currentDriveSpeed, true,  false, false, true);  break; 
    
    case STOPPED_DRIVING: 
      // Halts wheels only. Spreader & Plow keep doing their thing.
      analogWrite(FL_PWM_PIN, 0); analogWrite(RL_PWM_PIN, 0);
      analogWrite(FR_PWM_PIN, 0); analogWrite(RR_PWM_PIN, 0);
      break;

    case STOPPED:      
      stopRobot(); // True E-Stop
      break;

    case STALLED:      
      stopRobot(); 
      break;
  }
}

// --- HELPER FUNCTIONS ---

void processCommand(char cmd) {
  // 1. TRUE E-STOP (Kills everything)
  if (cmd == 'S' || cmd == 's') { 
    currentState = STOPPED; 
    Serial1.println("EMERGENCY STOP ACTIVATED");
    return; 
  }

  // 2. HALT DRIVING (TouchUp event from app)
  if (cmd == 'H' || cmd == 'h') {
    if (currentState != STALLED) { // Don't overwrite stall state
      currentState = STOPPED_DRIVING;
    }
    return;
  }
  
  // 3. STALL ESCAPE OVERRIDE
  if (currentState == STALLED) {
    if (cmd == 'B' || cmd == 'b') { 
      currentState = BACKWARD; 
      Serial1.println("Reversing to escape stall...");
    }
    return; // Ignore other movements until unstuck
  }

  // 4. MOVEMENT COMMANDS
  if      (cmd == 'F' || cmd == 'f') currentState = FORWARD;
  else if (cmd == 'B' || cmd == 'b') currentState = BACKWARD;
  else if (cmd == 'L' || cmd == 'l') currentState = LEFT;
  else if (cmd == 'R' || cmd == 'r') currentState = RIGHT;
  else if (cmd == 'X' || cmd == 'x') currentState = STRAFE_LEFT;
  else if (cmd == 'Y' || cmd == 'y') currentState = STRAFE_RIGHT;

  // 5. SPEED COMMANDS
  else if (cmd == '1') { currentDriveSpeed = SPEED_SLOW;   Serial1.println("Speed: SLOW"); }
  else if (cmd == '2') { currentDriveSpeed = SPEED_NORMAL; Serial1.println("Speed: NORMAL"); }
  else if (cmd == '3') { currentDriveSpeed = SPEED_FAST;   Serial1.println("Speed: FAST"); }

  // 6. PLOW COMMANDS (CHANGES WERE MADE BECAUSE MECHANINCAL DID WRONG POSITIONING. THE RIGHT THING IS OPPOSITE OF THIS)
  else if (cmd == 'U' || cmd == 'u') { 
    targetPlowAngle = 120; 
    Serial1.println("Plow LOWERING..."); 
  }
  else if (cmd == 'J' || cmd == 'j') { 
    targetPlowAngle = 0;  
    Serial1.println("Plow LIFTING..."); 
  }
    
  // 7. SPREADER COMMANDS
  else if (cmd == 'O' || cmd == 'o') { 
    isSpreaderOn = true;
    analogWrite(SPR_RPWM_PIN, SPREADER_SPEED);
    analogWrite(SPR_LPWM_PIN, 0);
    Serial1.println("Spreader ON");
  }
  else if (cmd == 'P' || cmd == 'p') { 
    isSpreaderOn = false;
    analogWrite(SPR_RPWM_PIN, 0);
    analogWrite(SPR_LPWM_PIN, 0);
    Serial1.println("Spreader OFF");
  }
}

// Controls the physical Grove LEDs
void updateLEDs() {
  if (currentState == STALLED) {
    // Red Error Light
    digitalWrite(LED_BLUE, LOW); digitalWrite(LED_GREEN, LOW); digitalWrite(LED_RED, HIGH);
  } 
  else if ((currentState == STOPPED || currentState == STOPPED_DRIVING) && !isSpreaderOn) {
    // Blue Standby Light (Everything is completely still)
    digitalWrite(LED_BLUE, HIGH); digitalWrite(LED_GREEN, LOW); digitalWrite(LED_RED, LOW);
  } 
  else {
    // Green Active Light (Driving OR Spreading)
    digitalWrite(LED_BLUE, LOW); digitalWrite(LED_GREEN, HIGH); digitalWrite(LED_RED, LOW);
  }
}

// 4-Wheel Independent Motor Control
void moveRobot4W(int speed, bool fl_fwd, bool rl_fwd, bool fr_fwd, bool rr_fwd) {
  // Front Left
  if (fl_fwd) { digitalWrite(FL_IN1_PIN, HIGH); digitalWrite(FL_IN2_PIN, LOW); } 
  else        { digitalWrite(FL_IN1_PIN, LOW);  digitalWrite(FL_IN2_PIN, HIGH); }
  analogWrite(FL_PWM_PIN, speed);

  // Rear Left
  if (rl_fwd) { digitalWrite(RL_IN1_PIN, HIGH); digitalWrite(RL_IN2_PIN, LOW); } 
  else        { digitalWrite(RL_IN1_PIN, LOW);  digitalWrite(RL_IN2_PIN, HIGH); }
  analogWrite(RL_PWM_PIN, speed);

  // Front Right
  if (fr_fwd) { digitalWrite(FR_IN1_PIN, HIGH); digitalWrite(FR_IN2_PIN, LOW); } 
  else        { digitalWrite(FR_IN1_PIN, LOW);  digitalWrite(FR_IN2_PIN, HIGH); }
  analogWrite(FR_PWM_PIN, speed);

  // Rear Right
  if (rr_fwd) { digitalWrite(RR_IN1_PIN, HIGH); digitalWrite(RR_IN2_PIN, LOW); } 
  else        { digitalWrite(RR_IN1_PIN, LOW);  digitalWrite(RR_IN2_PIN, HIGH); }
  analogWrite(RR_PWM_PIN, speed);
}

// True Emergency Stop
void stopRobot() {
  // 1. STOP DRIVE WHEELS
  digitalWrite(FL_IN1_PIN, LOW); digitalWrite(FL_IN2_PIN, LOW);
  digitalWrite(RL_IN1_PIN, LOW); digitalWrite(RL_IN2_PIN, LOW);
  digitalWrite(FR_IN1_PIN, LOW); digitalWrite(FR_IN2_PIN, LOW);
  digitalWrite(RR_IN1_PIN, LOW); digitalWrite(RR_IN2_PIN, LOW);
  
  analogWrite(FL_PWM_PIN, 0); analogWrite(RL_PWM_PIN, 0);
  analogWrite(FR_PWM_PIN, 0); analogWrite(RR_PWM_PIN, 0);

  // 2. STOP SPREADER MOTOR
  analogWrite(SPR_RPWM_PIN, 0);
  analogWrite(SPR_LPWM_PIN, 0);
  isSpreaderOn = false; // Ensures Green LED turns off

  // 3. FREEZE PLOW SERVO
  targetPlowAngle = currentPlowAngle; 
}
