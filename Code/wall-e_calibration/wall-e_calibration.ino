/**
 * WALL-E CALIBRATION CODE
 *
 * 15-6-26 Richard Nicholson
 * added eyebrows, and door servo
 * press 1 to 9 for servos
 * will know display default values set from restpos[] in here
 * PWm Module command works 0 - 4096 recommended range is 150- 700 
 * this represents Pwm Full range pulses
 * We work in 0-100 range in code for these servos
 *
 * 
 * @file      wall-e_calibration.ino
 * @brief     Sketch to calibrate the servo motor min/max positions
 * @author    Simon Bluett
 * @copyright MIT license
 * @version   1.1
 * @date      9th July 2020
 *
 * HOW TO USE:
 * 1. Install the Adafruit_PWMServoDriver library
 *    a. In the Arduino IDE, go to Sketch->Include Library->Manage Libraries
 *    b. Search for Adafruit PWM Library, and install the latest version
 * 2. Upload the sketch to the micro-controller, and open serial monitor at 
 *    a baud rate of 115200.
 * 3. The sketch will then let you control each servo motor individually. The
 *    aim is to find the maximum and minimum PWM pulse widths, corresponding 
 *    to the LOW and HIGH positions to which each servo motors can move. 
 *    Diagrams showing these positions can be found at 
 *    https://wired.chillibasket.com/3d-printed-wall-e/
 *    a. The sketch starts with the head rotation servo (which is on pin 0). 
 *       First you need to move to the LOW position (head turned to the left),
 *       by sending the 'a' or 'd' characters to move the servo in increments
 *       of 10 pulse width. For finer control, send the 'z' and 'c' characters 
 *       to move in increments of 1.
 *    b. Once the head is in the LOW position, send the 'n' character to confirm
 *       the position and move on to the HIGH position. Repeat the process.
 *    d. The process will then repeat itself for each of the 7 servos, getting
 *       you to specify the LOW and HIGH positions.
 * 4. Once the process is complete, the calibrated positions will be output to
 *    the serial monitor; for example:
 *
 *    // Servo Positions:  Low,High
 *    int preset[][2] =  {{398, 112},  // head rotation
 *                        {565, 188},  // neck top
 *                        {470, 100},  // neck bottom
 *                        {475, 230},  // eye right
 *                        {270, 440},  // eye left
 *                        {350, 185},  // arm left
 *                        {188, 360}}; // arm right
 *
 *    Copy the array and paste it into lines 144 to 150 in [wall-e.ino]
 */

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

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

// Define the pin-mapping
// -- -- -- -- -- -- -- -- -- -- -- -- -- --
#define SR_OE D9  // Servo shield output enable pin

// Define other constants
// -- -- -- -- -- -- -- -- -- -- -- -- -- --
#define SERVOS 10  // Number of servo motors

// Instantiate objects
// -- -- -- -- -- -- -- -- -- -- -- -- -- --
// Servo shield controller class
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Servo Positions:  Low,High    Range: 130-700 pwm pulses
int preset[][2] = { { 398, 112 },  // 0 head rotation
	                  { 565, 188 },  // 1 neck top
	                  { 200, 400 },  // 2 neck bottom
	                  { 475, 230 },  // 3 eye right
	                  { 270, 440 },  // 4 eye left
	                  { 350, 185 },  // 5 arm left
	                  { 188, 360 },  // 6 arm right
	                  { 210, 150 },  // 7 eyebrow right
	                  { 210, 150 },  // 8 eyebrow left
	                  { 0, 850 } };  // 9 Door

// Rest position (defaults) - Percent 0-100
float restpos[10] = { 50, 50, 40, 0, 0, 100, 100, 0, 0, 50 };

// Messages
String messages1[10] = { "0 Head Rotation - ",
	                       "1 Neck Top Joint - ",
	                       "2 Neck Bottom Joint - ",
	                       "3 Eye Right - ",
	                       "4 Eye Left - ",
	                       "5 Arm Left - ",
	                       "6 Arm Right - ",
	                       "7 Eyebrow Right - ",
	                       "8 Eyebrow Left - ",
	                       "9 Door - " };

String messages2[][2] = { { "LOW (head facing left)", "HIGH (head facing right)" },
	                        { "LOW (head looking up)", "HIGH (head looking down)" },
	                        { "LOW (head looking down)", "HIGH (head looking up)" },
	                        { "LOW (eye rotated down)", "HIGH (eye rotated up)" },
	                        { "LOW (eye rotated down)", "HIGH (eye rotated up)" },
	                        { "LOW (arm rotated down)", "HIGH (arm rotated up)" },
	                        { "LOW (arm rotated down)", "HIGH (arm rotated up)" },
	                        { "LOW (Eyebrow rotated down)", "HIGH (Eyebrow rotated up)" },
	                        { "LOW (Eyebrow rotated down)", "HIGH (Eyebrow rotated up)" },
	                        { "LOW (Door rotated down)", "HIGH (Door rotated up)" } };

// Runtime Variables
int currentServo = 0;
int currentPosition = -1;
int position = preset[0][0] - 1;
int defaultPosition = 0;

// ------------------------------------------------------------------
/// INITIAL SETUP
// ------------------------------------------------------------------
void setup() {

	pinMode(DIRA1, OUTPUT);			// this is to turn drive motor off
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

	// Output Enable (EO) pin for the servo motors
	pinMode(SR_OE, OUTPUT);
	digitalWrite(SR_OE, HIGH);

	// Communicate with servo shield (Analog servos run at ~60Hz)
	pwm.begin();
	pwm.setPWMFreq(60);

	// Turn off servo outputs
	for (int i = 0; i < SERVOS; i++) pwm.setPin(i, 0);

	// Initialize serial communication for debugging
	Serial.begin(115200);
	while (!Serial)
		;
	Serial.println();
	Serial.println(F("////////// Starting Wall-E Calibration Program //////////"));
	Serial.println();

	digitalWrite(SR_OE, LOW);
	softStart();
	moveToNextPosition();
}

// -------------------------------------------------------------------
/// MOVE TO NEXT POSITION OR SERVO
// -------------------------------------------------------------------
void moveToNextPosition() {

	if (currentPosition != -1) {
		// Save the current position
		preset[currentServo][currentPosition] = position;
		Serial.print(F("[Confirmed Position: "));
		Serial.print(position);
		Serial.println("]");
	}

	// Move on to the next position (Low/High)
	if (currentPosition < 1) {
		currentPosition++;
		position = preset[currentServo][currentPosition];

		// Else move servo to middle position and go to the next servo
	} else {
		changeServoPosition(int(restpos[currentServo] / 100.0 * (preset[currentServo][1] - preset[currentServo][0]) + preset[currentServo][0]));
		pwm.setPin(currentServo, 0);
		defaultPosition = int(restpos[currentServo] / 100.0 * (preset[currentServo][1] - preset[currentServo][0]) + preset[currentServo][0]);
		Serial.print(F("[Default Position: "));
		Serial.print(defaultPosition);				// raw number
		Serial.print("  ");
		Serial.print(int (restpos[currentServo]));				//  show as percentage
		Serial.println(" ]\n");

		currentServo++;
		currentPosition = 0;		
		position = preset[currentServo][currentPosition] - 1;

		// If all servos are calibrated, output the results
		if (currentServo == SERVOS) {
			digitalWrite(SR_OE, HIGH);
			outputResults();
		}

		// Soft start the servo to prevent sudden jumps
		softStart();
	}

	// Output message to serial monitor
	Serial.println(F("-----------------------------------"));
	Serial.println(F("Commands: 'a'=-10deg, 'd'=+10deg, 'z'=-1deg, 'c'=+1deg, 'n'=confirm position"));
	Serial.println(F("-----------------------------------"));
	Serial.print(messages1[currentServo]);
	Serial.print(messages2[currentServo][currentPosition]);
	Serial.print("  Position = ");
	Serial.println(position);

	// Move the current servo to the proper position
	changeServoPosition(preset[currentServo][currentPosition]);
}

// -------------------------------------------------------------------
/// CHANGE SERVO POSITION
// -------------------------------------------------------------------
void changeServoPosition(int newPosition) {
	while (position != newPosition) {
		if (position < newPosition) position++;
		else position--;
		pwm.setPWM(currentServo, 0, position);
		delay(5);
	}
}

// -------------------------------------------------------------------
/// SOFT START - Try and start up servo gently
// -------------------------------------------------------------------
void softStart() {
	unsigned long endTime = millis() + 1000;
	while (endTime > millis()) {
		pwm.setPWM(currentServo, 0, position);
		delay(10);
		pwm.setPin(currentServo, 0);
		delay(50);
	}
	pwm.setPWM(currentServo, 0, position);
}

// -------------------------------------------------------------------
/// OUTPUT THE RESULTS
// -------------------------------------------------------------------
void outputResults() {
	Serial.println("Calibrated values - please copy and paste these into the 'wall-e.ino' sketch:\n");
	Serial.print("int preset[][2] =  {{");
	Serial.print(preset[0][0]);
	Serial.print(",");
	Serial.print(preset[0][1]);
	Serial.println("},  // head rotation");
	Serial.print("                    {");
	Serial.print(preset[1][0]);
	Serial.print(",");
	Serial.print(preset[1][1]);
	Serial.println("},  // neck top");
	Serial.print("                    {");
	Serial.print(preset[2][0]);
	Serial.print(",");
	Serial.print(preset[2][1]);
	Serial.println("},  // neck bottom");
	Serial.print("                    {");
	Serial.print(preset[3][0]);
	Serial.print(",");
	Serial.print(preset[3][1]);
	Serial.println("},  // eye right");
	Serial.print("                    {");
	Serial.print(preset[4][0]);
	Serial.print(",");
	Serial.print(preset[4][1]);
	Serial.println("},  // eye left");
	Serial.print("                    {");
	Serial.print(preset[5][0]);
	Serial.print(",");
	Serial.print(preset[5][1]);
	Serial.println("},  // arm left");
	Serial.print("                    {");
	Serial.print(preset[6][0]);
	Serial.print(",");
	Serial.print(preset[6][1]);
	Serial.println("},  // arm right");
	Serial.print("                    {");
	Serial.print(preset[7][0]);
	Serial.print(",");
	Serial.print(preset[7][1]);
	Serial.println("},  // Eyebrow Right");
	Serial.print("                    {");
	Serial.print(preset[8][0]);
	Serial.print(",");
	Serial.print(preset[8][1]);
	Serial.println("},  // Eyebrow Left");
	Serial.print("                    {");
	Serial.print(preset[9][0]);
	Serial.print(",");
	Serial.print(preset[9][1]);
	Serial.println("}};   // Door");

	// Stop the program
	while (1) {}
}

// -------------------------------------------------------------------
/// READ INPUT FROM SERIAL
// -------------------------------------------------------------------
void readSerial() {
	// Read incoming byte
	char inchar = Serial.read();

	// Move on to next position or servo
	if (inchar == 'n') {
		moveToNextPosition();

		// Decrease servo position by 10 degrees
	} else if (inchar == 'a') {
		changeServoPosition(position - 10);
		Serial.print("Position = ");
		Serial.println(position);

		// Increase servo position by 10 degrees
	} else if (inchar == 'd') {
		changeServoPosition(position + 10);
		Serial.print("Position = ");
		Serial.println(position);

		// Decrease servo position by 1 degree
	} else if (inchar == 'z') {
		changeServoPosition(position - 1);
		Serial.print("Position = ");
		Serial.println(position);

		// Increase servo position by 1 degree
	} else if (inchar == 'c') {
		changeServoPosition(position + 1);
		Serial.print("Position = ");
		Serial.println(position);

	} else if (inchar == '1') {
		currentServo = 1;
		currentPosition = -1;		
		moveToNextPosition();
	
	} else if (inchar == '2') {
		currentServo = 2;
		currentPosition = -1;		
		moveToNextPosition();
	
	} else if (inchar == '3') {
		currentServo = 3;
		currentPosition = -1;		
		moveToNextPosition();
	
	} else if (inchar == '4') {
		currentServo = 4;
		currentPosition = -1;		
		moveToNextPosition();
	
	} else if (inchar == '5') {
		currentServo = 5;
		currentPosition = -1;		
		moveToNextPosition();
	
	} else if (inchar == '6') {
		currentServo = 6;
		currentPosition = -1;		
		moveToNextPosition();
	
	} else if (inchar == '7') {
		currentServo = 7;
		currentPosition = -1;		
		moveToNextPosition();
	
	} else if (inchar == '8') {
		currentServo = 8;
		currentPosition = -1;		
		moveToNextPosition();
	
	} else if (inchar == '9') {
		currentServo = 9;
		currentPosition = -1;		
		moveToNextPosition();
	}
}


// -------------------------------------------------------------------
/// MAIN PROGRAM LOOP
// -------------------------------------------------------------------
void loop() {
	// Read any new serial messages
	// -- -- -- -- -- -- -- -- -- -- -- -- -- --
	if (Serial.available() > 0) {
		readSerial();
	}
}
