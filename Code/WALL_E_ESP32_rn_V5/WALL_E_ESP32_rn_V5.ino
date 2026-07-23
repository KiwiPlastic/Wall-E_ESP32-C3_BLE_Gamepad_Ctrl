#define VERSION 5.0  // 23-7-26 
/* * * * * * * * * * * * * * * * * * * * * * *
 *
 * Upgraded Code by:  Richard Nicholson
 * Email:    richn01@msn.com
 * based on the original project and code from Simon Bluett ( hello@chillibasket.com )
 * Version:  1.0
 * Date:     23 July 2026
 * Copyright (C) 2020, MIT License
 *
 * HOW TO USE:
 * 1. Install the following library's
 *    In the Arduino IDE, on the left, click the libary manager icon
 *    BLE-Game_Client by Tomaz bekas V0.12.1
 *    ESP_Software_Serial by Dirk Kaar SNDVAPS 8.3.1
 *    Adafruit SSD1306 by Adfruit v2.5.17
 *    Adafruit PWM Servo Driver lib by Adfruit v 3.03 
 *    Adafruit PCF8574 by Adfruit v 1.1.2
 *    DFRobotDFPlayerMini by DFRobot v1.0.6
 *    I2C Wire driver, Could be built in to IDE 
 *    
 * 2. Calibrate the servo motors, using the calibration sketch provided in the
 *    GitHub repository. Paste the calibrated values between lines 369 to 391.
 * 3. Upload the sketch to the micro-controller, and open the serial monitor 
 *    at a baud rate of 115200.
 * 4. Additional instructions and hints can be found at:
 *    https://wired.chillibasket.com/3d-printed-wall-e/
 *
 * * * * * * * * * * * * * * * * * * * * * * *

NOTE: BLE Controler should be OFF on Wall-E Power up
 
  // BLE Gamepad - Xbox controler - button map
    Tracks turn left/right  Left Stick X (hoz)      
    Tracks forward/back     Left Stick Y (Vert) 
    Head rotation           Right Stick X
    Head Up/Down            Right Stick Y
    Neck Bottom             Button A + Dpad Up/Dwn
    Lhs Eye                 Button X + Dpad Up/Dwn
    Lhs Eyebrow             Button X + Dpad Left/Right
    Rhs Eye                 Button B + Dpad Up/Dwn
    Rhs Eyebrow             Button B + Dpad Left/Right
    Door Open/Close         Button Y + Dpad Left Right
    Left Arm                Left Trigger  + Dpad Up/Dwn
    Right Arm               Right Trigger + Dpad Up/Dwn
    Play Sound              Left Bumper + Dpad Left/Right - can crash
    Motor dead zone         Left Bumper + Dpad Up/Dwn - 0 to 250
    LEDS move               Right Bumper + Dpad Left/Right - loop x 4
    Steering Offset         Right Bumper + Dpad Up/Dwn -  -100 to 100
    Animation Number        ViewButton +  Dpad Up/Dwn
    Automonus Servo Mode    MenuButton On/Off toggel
    Dpad Up                 adjust up
    Dpad Down               adjust down
    Dpad Left               adjust left
    Dpad Right              adjust right
    Left Stick Button
    Right Stick Button
    ShareButton
    XboxButton

 // LED Push Buttons on Front
    RED_PB = Play Prev
    GRN_PB = PlaY Next
    ORG_PB = Autonomus Servos On/Off
    WHT_PB = Door Open/Close

 //  CLI
    Wn -100 to 100 Motor turn Left/Right Ctrl
    Yn -100 to 100 Motor Forward/Reverse
    sn -100 to 100 Steering Offset
    On 0 to 250 Motor dead zone
    An  Animation number 
    Mn  0 to 1 Autoomonus Servo Mode on/off
    Ln 0 to 100 Left Arm move 
    Rn 0 to 100 Right Arm Movement
    Bn 0 to 100 Neck Bottom
    Tn 0 to 100 Neck Top        
    Gn 0 to 100 Head Rotation
    En 0 to 100 Lhs Eye
    Un 0 to 100 Rhs Eye
    Cn 0 to 100 Lhs Eyebrow
    Vn 0 to 100 Rhs Eyebrow
    Dn 0 to 1 Door open/close
    w Move forwrad
    q Stop
    s Backward
    a Drive & look Left
    d Drive and look Right
    j Left head Tilt
    l Right Head tilt
    i Sad Head
    k Head Neutral
    f Head Up
    g Head Forward
    h Head Down
    b LHS Arm Low,RHS Arm up
    n Both Arms Natural
    m left arm high,right arm low
    !n play sound n (not 0 will crash) - oled
    vn Volume n 0-30? - ole

 // PWM Mux channel #
    0 Head Rotation
    1 Neck Top
    2 Neckk Bottom
    3 Eye Right
    4 Eye Left
    5 Arm Left
    6 Arm Right
    7 Eye Brow Right
    8 Eye Brow Left
    9 Door
    13 RED RGB LED
    14 GRN RGB LED
    15 BLU RGB LED

 // Servos
    Pwm range is 0-4096 (12 bit) recommended range 150 min 700 max @ 60hz
    We work in percent 0-100
    Use calibration prog to set high and low position
    Default servo positions are set from animation (0); 
*/

// ====================================================================================

//#include <Arduino.h>
#include <BLEGamepadClient.h>         // Blue Tooth Xbox 1 controler
#include <SoftwareSerial.h>           // ESP_Software_Serial by Dirk Kaar SNDVAPS 8.3.1
#include <Wire.h>                     // I2C Driver
#include <SSD1306AsciiWire.h>         // I2C Oled display
#include <Adafruit_PWMServoDriver.h>  // I2C PWM Servo Driver
#include <Adafruit_PCF8575.h>         // I2C IO Expander
#include "Queue.hpp"                  //
#include "MotorControllerL298.hpp"    // L298 Tracks Motor Sheild
#include "DFRobotDFPlayerMini.h"      // MP3 DFPlayer

// BIOS ESP32-C3
#define DIRA1 D0             // 12v Motors with L298N
#define DIRB1 D1             // 12v Motors with L298N
#define ENABLE1 D2           // 12v Motors with L298N
#define DIRA2 D3             // 12v Motors with L298N
#define PIN_SDA D4           // PCA9685 I2C servo mux. used without definition
#define PIN_SCL D5           // PCA9685 I2C servo mux.  used without definition
#define DFPLAYER_TX_PIN D6   // DFPlayer Communication use Resitors 1k-gnd, 660, VCC
#define DFPLAYER_RX_PIN D7   // DFPlayer Communication
#define DIRB2 D10            // 12v Motors with L298N
#define SERVO_ENABLE_PIN D9  // Servo shield output enable pin, | Bootstrap
#define ENABLE2 D8           // 12v Motors with L298N | Bootstrap

// D8/D9 Bootstap pins must be high at Power Up/Boot
// pinMode(LED_BUILTIN, OUTPUT);  // Config Built in Led as an output. NOT avaliable ON C3

// PCF8575 I2C I/O Expander
#define PIN_RED_PB 0     // input
#define PIN_GRN_PB 1     // in
#define PIN_ORG_PB 2     // in
#define PIN_WHT_PB 3     // in
#define PIN_DFP_BUSY 4   // in
#define PIN_WHT_LED 8    // out
#define PIN_ORG_LED 9    // out
#define PIN_GRN_LED 10   // out
#define PIN_RED_LED 11   // out
#define PIN_BAY_LED 12   // out outout is active low mosfet is active high HIGH = mosfte on

// PWM RGB LED channels on the PCA9685 16 Ch PWM module
#define RED_CHANNEL 13
#define GREEN_CHANNEL 14
#define BLUE_CHANNEL 15

// Motors L298N settings
#define PROG_RUN
#define MAX_SPEED 240       // sets speed of DC  motors
#define MAX_BACK_SPEED 240  // sets speed of DC  motors for backward movement
#define MAX_SPEED_OFFSET 0  // @ 20 Difference between left and right motor
#define turn_amount 750     //

// DFPlayer Speed
#define NORMAL_SPEED 1  // These are the playback speeds, change to
#define FAST_SPEED 1.5  // see the effect on the sound sample. 1 is default speed
#define SLOW_SPEED 2    // 0.75  // >1 faster, <1 slower, 2 would be twice as fast, 0.5 half as fast

// Define other constants
#define NUMBER_OF_SERVOS 10      // Number of servo motors, 7 without eyelids, 9 with, plus door is 10
#define SERVO_UPDATE_TIME 10     // Time in milliseconds of how often to update servo and motor positions
#define SERVO_OFF_TIME 6000      // Turn servo motors off after 6 seconds
#define SERVOMIN 150             // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX 600             // This is the 'maximum' pulse length count (out of 4096)
#define STATUS_CHECK_TIME 100    // Time in milliseconds of how often to check robot status (eg. battery level)
#define CONTROLLER_THRESHOLD 1   // The minimum error which the dynamics controller tries to achieve
#define MAX_SERIAL_LENGTH 5      // Maximum number of characters that can be received
#define ONESEC 1000              // One second dah
#define DISPLAY_IDEAL_TIME 5000  // display ideal 5 sec then home screen
#define OLED_ADDR 0x3C           //
#define LSTICK_DZ_Xmax 0.05      // Left Stick Dead Zone X Max...
#define LSTICK_DZ_Xmin -0.05     //
#define LSTICK_DZ_Ymax 0.05      //
#define LSTICK_DZ_Ymin -0.05     //
#define RSTICK_DZ_Xmax 0.05      // Right Stick Dead Zone...
#define RSTICK_DZ_Xmin -0.05     //
#define RSTICK_DZ_Ymax 0.05      //
#define RSTICK_DZ_Ymin -0.05     //
#define S0_HEAD_DPOS 50          // Servo 0 Head Defualt Position
#define S1_NECKT_DPOS 55         // Servo 1 Neck Top
#define S2_NECKB_DPOS 50         // Servo 2 Neck Bottom
#define S3_EYE_RHS_DPOS 50       // Servo eye RHS
#define S4_EYE_LHS_DPOS 50       // Servo eye LHS
#define S5_ARM_LHS_DPOS 50       // Servo Arm LHS
#define S6_ARM_RHS_DPOS 50       // Servo Arm RHS
#define S7_EYEB_RHS_DPOS 50      // Servo Eye Brow LHS
#define S8_EYEB_LHS_DPOS 50      // Servo Eye Brow RHS
#define S9_DOOR_DPOS 50          // Servo Door

// OLED Display
SSD1306AsciiWire Oled;
#define DisplayFlip 1

// Servo shield controller class
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();  //default address 0x40

// I/O Expander
Adafruit_PCF8575 pcf;  // default address= 0x20

// BLE Controller - Xbox gamepad
XboxController controller;

// Set up motor controller classes for L298 controller
MotorController motorL(DIRA1, ENABLE1, DIRA2, false);
MotorController motorR(DIRB1, ENABLE2, DIRB2, false);

// SpeedArray contains the order in which the code will playback the sample at the designated speeds
// This is not used anywhere, it is to do with DFplayer
float SpeedArray[] = { NORMAL_SPEED, FAST_SPEED, SLOW_SPEED };
uint8_t NoOfSpeeds = 1;  // Number of difference speeds in the Speed array above
uint8_t SpeedIdx = 0;    // In effect when the checks in the main loop are made this will increment to 0

// Sound Variables DFPlayer
bool soundPlaying = false;  //
bool isSound = true;        // Assume sound effects are available. Not realy used
uint8_t playIndex = 2;
uint8_t lastplayIndex = 2;

// Create Serial comms and DFPlayer
//HardwareSerial mySerial(1);  // Use UART1, this did not work with xbox
SoftwareSerial softSerial(/*rx =*/DFPLAYER_RX_PIN, /*tx =*/DFPLAYER_TX_PIN);
#define FPSerial softSerial
DFRobotDFPlayerMini myDFPlayer;

// Queue for animations - buffer is defined outside of the queue class
// so that the compiler knows how much dynamic memory will be used
struct animation_t {
  uint16_t timer;
  int8_t servos[NUMBER_OF_SERVOS];
};

#define QUEUE_LENGTH 50
animation_t buffer[QUEUE_LENGTH];
Queue<animation_t> queue(QUEUE_LENGTH, buffer);

// Motor Control Variables
int pwmspeed = 100;
int moveValue = 0;
int turnValue = 0;
int turnOffset = 0;
int motorDeadzone = 0;
int oldCurvel = 0;

// Runtime Variables
unsigned long lastTime = 0;
unsigned long animeTimer = 0;
unsigned long motorTimer = 0;
unsigned long statusTimer = 0;
unsigned long updateTimer = 0;
bool autoMode = false;
bool autonomousMode = false;  // ble interlock
bool autoModeFlag = false;
unsigned long autonomousModeTimer;  //for 1 sec led display
unsigned long LastBlinkLED;
bool stat_LED = false;
unsigned long displaytime;
unsigned long doorTime;
bool doorMode = false;  // BLE interlock
bool doorFlag = false;
bool BLE_Conected_Flag = false;
uint8_t ledIndex = 0;  // PB Leds on body
uint8_t lastledIndex = 0;
uint8_t indexRGB = 0;

// Serial Parsing
char firstChar;
char serialBuffer[MAX_SERIAL_LENGTH];
uint8_t serialLength = 0;
String value = "";

// ****** SERVO MOTOR CALIBRATION *********************
// Servo Positions:  Low,High Range: 150-600 pause witdth2
int preset[][2] = { { 458, 162 },   // head rotation
                    { 565, 98 },    // neck top
                    { 170, 570 },   // neck bottom
                    { 215, 436 },   // eye right
                    { 215, 436 },   // eye left
                    { 350, 150 },   // arm left
                    { 150, 360 },   // arm right
                    { 400, 150 },   // eyebrow right
                    { 400, 150 },   // eyebrow left
                    { 10, 800 } };  // Door
// *****************************************************

// Servo Control - Position, Velocity, Acceleration
// Servo Pins:	     0,   1,   2,   3,   4,   5,   6,   -,   -
// Joint Name:	  head,necT,necB,eyeR,eyeL,armL,armR,eyebL,eybR,Dr,motL,motR,
float curpos[] = { 248, 400, 140, 475, 270, 250, 290, 210, 307, 250, 180, 180 };    // Current position (units)
float setpos[] = { 248, 400, 140, 475, 270, 250, 290, 210, 307, 250, 0, 0 };        // Required position (units)
float curvel[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };                            // Current velocity (units/sec)
float maxvel[] = { 500, 500, 500, 1000, 1000, 500, 500, 500, 500, 500, 250, 250 };  // Max Servo velocity (units/sec)
float accell[] = { 350, 350, 350, 100, 100, 50, 50, 350, 350, 350, 800, 800 };      // Servo acceleration (units/sec^2)
//float maxvel[] = { 500, 400, 500, 2400, 2400, 600, 600, 400, 400, 250, 250, 500 };  // Max Servo velocity (units/sec)
//float accell[] = { 350, 300, 480, 1800, 1800, 500, 500, 300, 300, 800, 800, 350 };  // Servo acceleration (units/sec^2)

// BLE Game control Values 0-100
uint8_t neckBottomIndex = S2_NECKB_DPOS;
uint8_t eyeRHSIndex = S3_EYE_RHS_DPOS;
uint8_t eyeLHSIndex = S4_EYE_LHS_DPOS;
uint8_t leftArmIndex = S5_ARM_LHS_DPOS;
uint8_t rightArmIndex = S6_ARM_RHS_DPOS;
uint8_t eyebrowRHSIndex = S7_EYEB_RHS_DPOS;
uint8_t eyebrowLHSIndex = S8_EYEB_LHS_DPOS;
uint8_t motorDeadZoneIndex = 0;
int16_t steerOffsetIndex = 0;
uint8_t animationIndex = 0;
bool neutral_LHS_JS_X = true;
bool neutral_LHS_JS_Y = true;
bool neutral_RHS_JS_X = true;
bool neutral_RHS_JS_Y = true;

// -------------------------------------------------------------------
// Initial setup
// -------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  Serial.print(F("--- Wall-E Control Sketch - V"));
  Serial.print(VERSION);
  Serial.println(F(" ---"));
  delay(1000);

  // Output Enable (EO) pin for the servo motors
  pinMode(SERVO_ENABLE_PIN, OUTPUT);
  digitalWrite(SERVO_ENABLE_PIN, HIGH);

  // Motor pins
  pinMode(DIRA1, OUTPUT);
  pinMode(DIRB1, OUTPUT);
  pinMode(ENABLE1, OUTPUT);
  pinMode(DIRA2, OUTPUT);
  pinMode(DIRB2, OUTPUT);
  pinMode(ENABLE2, OUTPUT);
  digitalWrite(DIRA1, HIGH);
  digitalWrite(DIRB1, HIGH);
  digitalWrite(DIRA2, HIGH);
  digitalWrite(DIRB2, HIGH);
  digitalWrite(ENABLE1, HIGH);
  digitalWrite(ENABLE2, HIGH);

  // Communicate with servo shield (Analog servos run at ~60Hz)
  pwm.begin();
  pwm.setPWMFreq(60);

  // Turn off servo outputs
  for (int i = 0; i < NUMBER_OF_SERVOS; i++) {
    pwm.setPin(i, 0);
  }

  //BLE xbox controler
  controller.begin();
  delay(1000);

  // OLED Display
  Oled.begin(&Adafruit128x64, OLED_ADDR);  // 0.96 Inch mono display
  Oled.displayRemap( DisplayFlip );        // set true or false to flip screen
  oledWelcome();

  // START the PCF8575 Expander
  if (!pcf.begin(0x20, &Wire)) {
    Serial.println("Couldn't find PCF8575");
    while (1)
      delay(100);
  }
  delay(1000);
  
  pcf.pinMode(PIN_RED_PB, INPUT_PULLUP);    //
  pcf.pinMode(PIN_GRN_PB, INPUT_PULLUP);    //
  pcf.pinMode(PIN_ORG_PB, INPUT_PULLUP);    //
  pcf.pinMode(PIN_WHT_PB, INPUT_PULLUP);    //
  pcf.pinMode(PIN_DFP_BUSY, INPUT_PULLUP);  //

  pcf.pinMode(PIN_RED_LED, OUTPUT);   // active low
  pcf.pinMode(PIN_GRN_LED, OUTPUT);   //
  pcf.pinMode(PIN_ORG_LED, OUTPUT);   //
  pcf.pinMode(PIN_WHT_LED, OUTPUT);   //
  pcf.pinMode(PIN_BAY_LED, OUTPUT);   // active high

  alloff();  // all PushButton leds off
  delay(1000);

  // START DFPlayer
  Serial.println("Staring DFPlayer...");
  //mySerial.begin(9600, SERIAL_8N1, D7, D6);  // RX = D7, TX = D6 (hardware serial)
  FPSerial.begin(9600);  // Software serial
  delay(1000);
  //myDFPlayer.begin(mySerial);   // HW Serial
  myDFPlayer.begin(FPSerial, /*isACK = */ true, /*doReset = */ true);  //SW Serial
  delay(4000);                                                         // it needs time to read SD card and start up
  //if (!myDFPlayer.begin(mySerial)) {    //HW serial
  if (!myDFPlayer.begin(FPSerial)) {  //SW serial
    isSound = false;                  // If failed, no sound available : continue
    Serial.println(myDFPlayer.readType(), HEX);
    Serial.println(F("Error with DFPlayer."));
  } else {
    Serial.println(F("DFPlayer Mini online."));
    myDFPlayer.setTimeOut(500);  //Set serial communictaion time out 500ms
    delay(4000);                 // this is needed, else might not start

    //----Set volume----
    myDFPlayer.volume(30);    // Set volume value (0~30).
    myDFPlayer.volumeUp();    // Volume Up
    myDFPlayer.volumeDown();  // Volume Down
    myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
    myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
    soundPlaying = true;
    myDFPlayer.play(playIndex);  //Play the mp3 (file: 2 sounds good)
  }

  randomSeed(analogRead(0));

  // Check if servo animation queue is working, and move servos to known starting positions
  if (queue.errors()) Serial.println(F("Error: Unable to allocate memory for servo animation queue"));

  // Soft start the servo motors
  Serial.println(F("Starting up the servo motors"));
  digitalWrite(SERVO_ENABLE_PIN, LOW);

  //  Set servo defaults here
  // servoDefaults();
  playAnimation(0);              // case 0 loads servo default positions
  softStart(queue.pop(), 3500);  // trigger queue

  Serial.println(F("Startup complete; entering main loop"));
}

//==========================================================================================================
// ---------------------------------------------
// transfers BLE commands to serial interface
// ---------------------------------------------
void doEvaluateSerial(char fs, String val) {  // val is, value converting to string
  firstChar = fs;
  serialLength = 0;
  for (uint8_t i = 0; i < val.length() && i < MAX_SERIAL_LENGTH - 1; i++) {  // get length of string pased in val
    serialBuffer[i] = val[i];                                                // load serial buffer at i with char at val(i), this is the hock in
    serialLength++;
  }
  serialBuffer[serialLength] = 0;
  // Serial.print(" fs = ");
  //  Serial.print(fs);
  //  Serial.print(" serialBuffer = ");
  //  Serial.print(serialBuffer);
  //  Serial.print(", ");

  evaluateSerial();  // read serial buffer (CLI)
  serialBuffer[0] = 0;
  serialBuffer[1] = 0;
  serialBuffer[2] = 0;
  serialBuffer[3] = 0;
  serialBuffer[4] = 0;
}

// -------------------------------------------------------------------
// Read input from USB serial port
//
// This function reads incoming characters in the serial port
// and inserts them into a buffer to be processed later.
// -------------------------------------------------------------------
void readSerial() {
  char inchar = Serial.read();             // Read incoming USB byte
  if (inchar == '\n' || inchar == '\r') {  // If the string has ended, evaluate the serial buffer
    if (serialLength > 0) evaluateSerial();
    serialBuffer[0] = 0;
    serialLength = 0;
  } else {  // Otherwise add to the character to the buffer
    if (serialLength == 0) firstChar = inchar;
    else {
      serialBuffer[serialLength - 1] = inchar;
      serialBuffer[serialLength] = 0;
    }
    serialLength++;
    if (serialLength == MAX_SERIAL_LENGTH) {  // To prevent overflows, evalute the buffer if it is full
      evaluateSerial();
      serialBuffer[0] = 0;
      serialLength = 0;
    }
  }
}

// -------------------------------------------------------------------
// Evaluate input from serial port CLI
//
// Parse the received serial message which is stored in
// the "serialBuffer" filled by the "readSerial()" function
// -------------------------------------------------------------------
void evaluateSerial() {
  int number = atoi(serialBuffer);  // Evaluate integer number in the serial buffer

  Serial.print(firstChar);  // de bug
  Serial.println(number);   // de bug

  // Motor Inputs and Offsets
  if (firstChar == 'X' && number >= -100 && number <= 100) turnValue = int(number * 2.55);       // Left/right control
  else if (firstChar == 'Y' && number >= -100 && number <= 100) moveValue = int(number * 2.55);  // Forward/reverse control

  // Steering offset
  else if (firstChar == 'S' && number >= -100 && number <= 100) {
    turnOffset = number;  // Steering offset
    OledSteeringOffset(turnOffset);
  }

  // Motor Dead Zone
  else if (firstChar == 'O' && number >= 0 && number <= 250) {
    motorDeadzone = int(number);  // Motor deadzone offset
    OledMotorDeadZone(motorDeadzone);
  }

  // Animations
  else if (firstChar == 'A') {
    oledAnimation(number);
    playAnimation(number);
  }

  // Manual servo control
  else if (firstChar == 'L' && number >= 0 && number <= 100) {  // Move left arm
    autoMode = false;
    queue.clear();
    setpos[5] = int(number * 0.01 * (preset[5][1] - preset[5][0]) + preset[5][0]);

  } else if (firstChar == 'R' && number >= 0 && number <= 100) {  // Move right arm
    autoMode = false;
    queue.clear();
    setpos[6] = int(number * 0.01 * (preset[6][1] - preset[6][0]) + preset[6][0]);

  } else if (firstChar == 'B' && number >= 0 && number <= 100) {  // Move neck bottom
    autoMode = false;
    queue.clear();
    setpos[2] = int(number * 0.01 * (preset[2][1] - preset[2][0]) + preset[2][0]);

  } else if (firstChar == 'T' && number >= 0 && number <= 100) {  // Move neck top
    autoMode = false;
    queue.clear();
    setpos[1] = int(number * 0.01 * (preset[1][1] - preset[1][0]) + preset[1][0]);

  } else if (firstChar == 'G' && number >= 0 && number <= 100) {  // Move head rotation
    autoMode = false;
    queue.clear();
    setpos[0] = int(number * 0.01 * (preset[0][1] - preset[0][0]) + preset[0][0]);

  } else if (firstChar == 'E' && number >= 0 && number <= 100) {  // Move eye left
    autoMode = false;
    queue.clear();
    setpos[4] = int(number * 0.01 * (preset[4][1] - preset[4][0]) + preset[4][0]);

  } else if (firstChar == 'U' && number >= 0 && number <= 100) {  // Move eye right
    autoMode = false;
    queue.clear();
    setpos[3] = int(number * 0.01 * (preset[3][1] - preset[3][0]) + preset[3][0]);

  } else if (firstChar == 'C' && number >= 0 && number <= 100) {  // Move eyebrow left
    autoMode = false;
    queue.clear();
    setpos[8] = int(number * 0.01 * (preset[8][1] - preset[8][0]) + preset[8][0]);

  } else if (firstChar == 'V' && number >= 0 && number <= 100) {  // Move eyebrow right
    autoMode = false;
    queue.clear();
    setpos[7] = int(number * 0.01 * (preset[7][1] - preset[7][0]) + preset[7][0]);

  } else if (firstChar == 'D' && !doorFlag) {  // open/close door
    doorFlag = true;                           // this needs reset after servo moved
    doorTime = millis();                       // to trap for temp lock on doorFlag
    if (!number) {
      //Serial.println("door close");
      pcf.digitalWrite(PIN_WHT_LED, LOW);  // turn LED on
      autoMode = false;
      queue.clear();
      number = 10;  // close poition
      setpos[9] = int(number * 0.01 * (preset[9][1] - preset[9][0]) + preset[9][0]);
    } else if (number) {
      //Serial.println("door open");
      pcf.digitalWrite(PIN_WHT_LED, LOW);  // turn LED on
      autoMode = false;
      queue.clear();
      number = 80;  // open poition
      setpos[9] = int(number * 0.01 * (preset[9][1] - preset[9][0]) + preset[9][0]);
    }

    // Autonomous servo mode
  } else if (firstChar == 'M' && !autoModeFlag) {
    autoModeFlag = true;
    autonomousModeTimer = millis();
    if (!number) {
      autoMode = false;
      oledAutonomousMode(autoMode);
      pcf.digitalWrite(PIN_RED_LED, HIGH);  // turn LED off
    } else if (number) {
      autoMode = true;
      oledAutonomousMode(autoMode);
      pcf.digitalWrite(PIN_RED_LED, LOW);  // turn LED on
    }
  }
  // Manual Movements with WASD
  else if (firstChar == 'w') {  // Forward movement
    moveValue = pwmspeed;
    turnValue = 0;
    setpos[0] = (preset[0][1] + preset[0][0]) / 2;
  } else if (firstChar == 'q') {  // Stop movement
    moveValue = 0;
    turnValue = 0;
    setpos[0] = (preset[0][1] + preset[0][0]) / 2;
  } else if (firstChar == 's') {  // Backward movement
    moveValue = -pwmspeed;
    turnValue = 0;
    setpos[0] = (preset[0][1] + preset[0][0]) / 2;
  } else if (firstChar == 'a') {  // Drive & look left
    moveValue = 0;
    turnValue = -pwmspeed;
    setpos[0] = preset[0][0];
  } else if (firstChar == 'd') {  // Drive & look right
    moveValue = 0;
    turnValue = pwmspeed;
    setpos[0] = preset[0][1];
  }

  // Manual Eye Movements
  else if (firstChar == 'j') {  // Left head tilt
    setpos[4] = preset[4][0];
    setpos[3] = preset[3][1];
  } else if (firstChar == 'l') {  // Right head tilt
    setpos[4] = preset[4][1];
    setpos[3] = preset[3][0];
  } else if (firstChar == 'i') {  // Sad head
    setpos[4] = preset[4][0];
    setpos[3] = preset[3][0];
  } else if (firstChar == 'k') {  // Neutral head
    setpos[4] = int(0.4 * (preset[4][1] - preset[4][0]) + preset[4][0]);
    setpos[3] = int(0.4 * (preset[3][1] - preset[3][0]) + preset[3][0]);
  }

  // Head movement
  else if (firstChar == 'f') {  // Head up
    setpos[1] = preset[1][1];
    setpos[2] = preset[2][1];     //(preset[2][1] + preset[2][0]) / 2;
  } else if (firstChar == 'g') {  // Head forward
    setpos[1] = preset[1][1];
    setpos[2] = preset[2][0];
  } else if (firstChar == 'h') {  // Head down
    setpos[1] = preset[1][0];
    setpos[2] = preset[2][0];
  }

  // Arm Movements
  else if (firstChar == 'b') {  // Left arm low, right arm high
    setpos[5] = preset[5][0];
    setpos[6] = preset[6][1];
  } else if (firstChar == 'n') {  // Both arms neutral
    setpos[5] = (preset[5][0] + preset[5][1]) / 2;
    setpos[6] = (preset[6][0] + preset[6][1]) / 2;
  } else if (firstChar == 'm') {  // Left arm high, right arm low
    setpos[5] = preset[5][1];
    setpos[6] = preset[6][0];
  }

  // play sound
  else if ((firstChar == '!') && (!soundPlaying)) {  // Play sound
    if (number == 0) { number = 1; }
    playIndex = number;

    // Set volume
  } else if (firstChar == 'v') {  // Set volume
    myDFPlayer.volume(number);    //Set the volume to the value
  }
}

// -------------------------------------------------------------------
// Sequence and generate animations
// -------------------------------------------------------------------
void manageAnimations() {
  // If we are running an animation
  if ((queue.size() > 0) && (animeTimer <= millis())) {
    // Set the next waypoint time
    animation_t newValues = queue.pop();
    animeTimer = millis() + newValues.timer;

    // Set all the joint positions
    for (int i = 0; i < NUMBER_OF_SERVOS; i++) {
      // Scale the positions using the servo calibration values
      setpos[i] = int(newValues.servos[i] * 0.01 * (preset[i][1] - preset[i][0]) + preset[i][0]);
    }

    // If we are in autonomous mode and no movements are queued, generate random movements
  } else if (autoMode && queue.empty() && (animeTimer <= millis())) {

    // For each of the servos
    for (int i = 0; i < NUMBER_OF_SERVOS; i++) {

      // Randomly determine whether or not to update the servo
      if (random(2) == 1) {

        // For most of the servo motors
        if (i == 0 || i == 1 || i == 2 || i >= 5) {

          // Randomly determine the new position
          unsigned int min = preset[i][0];
          unsigned int max = preset[i][1];
          if (min > max) {
            min = max;
            max = preset[i][0];
          }

          setpos[i] = random(min, max + 1);

          // Since the eyes should work together, only look at one of them
        } else if (i == 3) {

          int midPos1 = int((preset[i][1] - preset[i][0]) * 0.4 + preset[i][0]);
          int midPos2 = int((preset[i + 1][1] - preset[i + 1][0]) * 0.4 + preset[i + 1][0]);

          // Determine which type of eye movement to do
          // Both eye move downwards
          if (random(2) == 1) {
            setpos[i] = random(midPos1, preset[i][0]);
            float multiplier = (setpos[i] - midPos1) / float(preset[i][0] - midPos1);
            setpos[i + 1] = ((1 - multiplier) * (midPos2 - preset[i + 1][0])) + preset[i + 1][0];

            // Both eyes move in opposite directions
          } else {
            setpos[i] = random(midPos1, preset[i][0]);
            float multiplier = (setpos[i] - preset[i][1]) / float(preset[i][0] - preset[i][1]);
            setpos[i + 1] = (multiplier * (preset[i + 1][1] - preset[i + 1][0])) + preset[i + 1][0];
          }
        }
      }
      //Serial.print(setpos[i]);
      //Serial.print(",");
    }
    //Serial.println();

    // Finally, figure out the amount of time until the next movement should be done
    animeTimer = millis() + random(500, 3000);
  }
}

// -------------------------------------------------------------------
// Manage the movement of the servo motors
// @param  dt  Time in milliseconds since function was last called
//
// This function uses the formulae:
//   (s = position, v = velocity, a = acceleration, t = time)
//   s = v^2 / (2*a)  <- to figure out whether to start slowing down
//   v = v + a*t      <- to calculate new servo velocity
//   s = s + v*t      <- to calculate new servo position
// -------------------------------------------------------------------
void manageServos(float dt) {
  bool moving = false;

  // For each of the servo motors
  for (int i = 0; i < NUMBER_OF_SERVOS; i++) {

    float posError = setpos[i] - curpos[i];

    // If position error is above the threshold
    if (abs(posError) > CONTROLLER_THRESHOLD && (setpos[i] != -1)) {

      digitalWrite(SERVO_ENABLE_PIN, LOW);
      moving = true;

      // Determine motion direction
      bool dir = true;
      if (posError < 0) dir = false;

      // Determine whether to accelerate or decelerate
      float acceleration = accell[i];
      if ((curvel[i] * curvel[i] / (2 * accell[i])) > abs(posError)) acceleration = -accell[i];

      // Update the current velocity
      if (dir) curvel[i] += acceleration * dt / 1000.0;
      else curvel[i] -= acceleration * dt / 1000.0;

      // Limit Velocity
      if (curvel[i] > maxvel[i]) curvel[i] = maxvel[i];
      if (curvel[i] < -maxvel[i]) curvel[i] = -maxvel[i];

      float dP = curvel[i] * dt / 1000.0;

      if (abs(dP) < abs(posError)) curpos[i] += dP;
      else curpos[i] = setpos[i];

      pwm.setPWM(i, 0, curpos[i]);
      //Serial.println(i, curpos[i]);

    } else {
      curvel[i] = 0;
    }
  }

  // Disable servos if robot is not moving
  // This helps prevents the motors from overheating
  if (moving) motorTimer = millis();
  else if (millis() - motorTimer >= SERVO_OFF_TIME) {
    //digitalWrite(SERVO_ENABLE_PIN, HIGH);
    for (int i = 0; i < NUMBER_OF_SERVOS; i++) {
      pwm.setPin(i, 0);
    }
  }
}

// -------------------------------------------------------------------
// Servo "Soft Start" function
//
// This function tries to start the servos up servo gently,
// reducing the sudden jerking motion which usually occurs
// when the motors power up for the first time.
//
// @param  targetPos  The target position of the servos after startup
// @param  timeMs     Time in milliseconds in which soft start should complete
// -------------------------------------------------------------------

void softStart(animation_t targetPos, int timeMs) {
  for (int i = 0; i < NUMBER_OF_SERVOS; i++) {
    if (targetPos.servos[i] >= 0) {
      curpos[i] = int(targetPos.servos[i] * 0.01 * (preset[i][1] - preset[i][0]) + preset[i][0]);

      unsigned long endTime = millis() + timeMs / NUMBER_OF_SERVOS;

      while (millis() < endTime) {
        pwm.setPWM(i, 0, curpos[i]);
        delay(10);
        pwm.setPin(i, 0);
        delay(50);
      }
      pwm.setPWM(i, 0, curpos[i]);
      setpos[i] = curpos[i];
    }
  }
}

// -------------------------------------------------------------------
// Manage the movement of the main motors
//
// @param  dt  Time in milliseconds since function was last called
// -------------------------------------------------------------------
void manageMotors(float dt) {
  // Update Main Motor Values
  setpos[NUMBER_OF_SERVOS] = moveValue - turnValue;
  setpos[NUMBER_OF_SERVOS + 1] = moveValue + turnValue;

  // Apply turn offset (motor trim) only when motors are active
  if (setpos[NUMBER_OF_SERVOS] != 0) setpos[NUMBER_OF_SERVOS] -= turnOffset;
  if (setpos[NUMBER_OF_SERVOS + 1] != 0) setpos[NUMBER_OF_SERVOS + 1] += turnOffset;

  for (int i = NUMBER_OF_SERVOS; i < NUMBER_OF_SERVOS + 2; i++) {

    float velError = setpos[i] - curvel[i];

    // If velocity error is above the threshold
    if (abs(velError) > CONTROLLER_THRESHOLD && (setpos[i] != -1)) {

      // Determine whether to accelerate or decelerate
      float acceleration = accell[i];
      if (setpos[i] < curvel[i] && curvel[i] >= 0) acceleration = -accell[i];
      else if (setpos[i] < curvel[i] && curvel[i] < 0) acceleration = -accell[i];
      else if (setpos[i] > curvel[i] && curvel[i] < 0) acceleration = accell[i];

      // Update the current velocity
      float dV = acceleration * dt / 1000.0;
      if (abs(dV) < abs(velError)) curvel[i] += dV;
      else curvel[i] = setpos[i];
    } else {
      curvel[i] = setpos[i];
    }

    // Apply deadzone offset. WHAT IS THIS??
    if (curvel[i] > 0) curvel[i] += motorDeadzone;
    else if (curvel[i] < 0) curvel[i] -= motorDeadzone;

    // Limit Velocity
    if (curvel[i] > maxvel[i]) curvel[i] = maxvel[i];
    if (curvel[i] < -maxvel[i]) curvel[i] = -maxvel[i];
  }

  // Update motor speeds
  motorL.setSpeed(curvel[NUMBER_OF_SERVOS]);
  motorR.setSpeed(curvel[NUMBER_OF_SERVOS + 1]);
#ifdef LED_PIN
  pwm.setPWM(LED_PIN, 0, abs(curvel[NUMBER_OF_SERVOS]) * 2);
#endif
  /* // debugging
    if (curvel[NUMBER_OF_SERVOS] != oldCurvel) {
      Serial.print("Motor values : ");
      Serial.print(curvel[NUMBER_OF_SERVOS]);
      Serial.print(" : ");
      Serial.println(curvel[NUMBER_OF_SERVOS + 1]);
      oldCurvel = curvel[NUMBER_OF_SERVOS];
    }*/
}

//--------------------------------------
void processSound() {
  if (playIndex != lastplayIndex) {
    if (!soundPlaying) {
      soundPlaying = true;
      lastplayIndex = playIndex;
      myDFPlayer.play(playIndex);  // Play mp3 file number on SD card
      Serial.print("Playing SD file : ");
      Serial.println(playIndex);
    }
  }
}

//-----------------------------------
void processPB_LEDS() {
  if (doorFlag && ((millis() - doorTime >= ONESEC))) {
    doorFlag = 0;
    //doorTime = 0;  //not needed
    alloff();
  }
  if (autoModeFlag && ((millis() - autonomousModeTimer >= ONESEC))) {
    autoModeFlag = false;
  }
}

//------------------------------------------------
// RGB LED (0-800 pulse value for 60hz pwm)
// -----------------------------------------------
void ProcessBlinkLED() {
  if ((millis() - LastBlinkLED) > ONESEC) {
    LastBlinkLED = millis();
    int r, g, b;
    indexRGB += 5;
    if (indexRGB >= 801) indexRGB = 0;  // 800/3=266 (3rds)
    if (indexRGB < 266) {
      r = 800 - (indexRGB * 3);
      g = indexRGB * 3;
      b = 0;
    } else if (indexRGB < 532) {
      int j = indexRGB - 266;
      r = 0;
      g = 800 - (j * 3);
      b = j * 3;
    } else {
      int j = indexRGB - 532;
      r = j * 3;
      g = 0;
      b = 800 - (j * 3);
    }
    setRGB(r, g, b);
  }
}

// Example code------------------------------------------------
void ProcessBlinkLED_org() {
  if ((millis() - LastBlinkLED) > ONESEC) {
    stat_LED = !stat_LED;
    LastBlinkLED = millis();
    if (stat_LED) {
      indexRGB ++;
      if (indexRGB >= 6) indexRGB = 1;
      if (indexRGB == 1) setRGB(800, 0, 0);      // Red color
      if (indexRGB == 2) setRGB(0, 800, 0);      // Green color
      if (indexRGB == 3) setRGB(0, 0, 800);      // Blue color
      if (indexRGB == 4) setRGB(800, 800, 800);  // White color (All colors full on)
      if (indexRGB == 5) setRGB(0, 0, 0);        // Black color (All colors full off)
    }
  }
}

//-----------------------------------------------------------------------
// set RGB LED pwm colour
void setRGB(int r, int g, int b) {
  pwm.setPWM(RED_CHANNEL, 0, r);
  pwm.setPWM(GREEN_CHANNEL, 0, g);
  pwm.setPWM(BLUE_CHANNEL, 0, b);
}

// -------------------------------------------------------------------
// Main program loop
// -------------------------------------------------------------------
void loop() {
  if (Serial.available() > 0) {                       // Read any new serial messages
    readSerial();                                     //
  }                                                   //
  manageAnimations();                                 // Read any new serial messages
  if (millis() - updateTimer >= SERVO_UPDATE_TIME) {  // Move Servos and wheels at regular time intervals
    updateTimer = millis();
    unsigned long newTime = micros();
    float dt = (newTime - lastTime) / 1000.0;
    lastTime = newTime;
    manageServos(dt);
    manageMotors(dt);
  }
  if (millis() - statusTimer >= STATUS_CHECK_TIME) {  // Update robot status
    statusTimer = millis();                           //
    PCF8575_Update();                                 // update expander i/o
    processSound();                                   //
    Controler();                                      // get xbox control update
    DisplayIdeal();                                   // if display ideal goto home screen
    processPB_LEDS();                                 //
    //ProcessBlinkLED();                                // hart beat
    ProcessBlinkLED_org();
    
    // checkBatteryLevel();
  }
}
