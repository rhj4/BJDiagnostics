/***********************************************************************
*
*	Name:	ServoMotorTester
*	Author:	Bob Jones <bob@rhj4.com>
*	Ver:	2.0
*	Desc:	Tests functionality of Pan and Tilt servos
*
*/
#include "Config.h"
#include <Servo.h>
int servoPin = SERVO_PIN;
int servoLeftPin = 11;
int servoRightPin = 10;
int servo2LeftPin = 9;
int servo2RightPin = 8;

int pos = 0;    // variable to store the servo position
int potVal = 0;
int lastPotVal = 0;
int currentAngle = -1;
int newAngle = -1;

char currentCommand;

#include "BJServo.h"

BJServo *servo;

void setup () {
	Serial.begin ( 115200 );
#ifdef DESCRIPTION
	Serial.println ( DESCRIPTION );
#endif

#ifdef VERSION
	Serial.print ( F ( "Version " ) );
	Serial.println ( VERSION );
#endif

	if (RED_LED > 0) {
		pinMode ( RED_LED, OUTPUT );
		flashPin ( RED_LED );
	}

	if (GREEN_LED > 0) {
		pinMode ( GREEN_LED, OUTPUT );
		flashPin ( GREEN_LED );
	}

	if (SWITCH_PIN > 0) {
		pinMode ( SWITCH_PIN, INPUT );
	}
	if (servoPin > 0) {
		servo = new BJServo ();
		servo->configure ( ( char* )"SERVO", servoPin, 90, 0, 180 );
		pinMode ( servoLeftPin, INPUT );
		pinMode ( servoRightPin, INPUT );
		servo->attach ( SERVO_PIN );
		if (servo->attached ()) {
			Serial.println ( F ( "Servo attached" ) );
		} else {
			Serial.println ( F ( "Servo attach failed" ) );
		}

	}

	Serial.println ( "\nStarting Servo Tester" );
	showCommands ();
}

void showCommands () {
	Serial.println ( "Enter Commands in Serial Window" );
	Serial.println ( F ( "+ = Attach servo" ) );
	Serial.println ( F ( "- = Detach servo" ) );
	Serial.println ( F ( "C = Center servo" ) );
	Serial.println ( F ( "S = Slow Sweep" ) );
	Serial.println ( F ( "F = Fast Sweep" ) );
	Serial.println ( F ( "L = Left" ) );
	Serial.println ( F ( "R = Right" ) );
	Serial.println ( F ( "P<int> = Point ti <int> degrees" ) );
	Serial.println ( F ( "F = Fast Sweep" ) );
	Serial.println ( F ( "X = Show servo extreme points" ) );
	Serial.println ( F ( "V = Read and show servo current value" ) );

	Serial.println ( F ( "? = Show this list" ) );
}

bool checkForCommand () {
	if (!Serial.available ()) {
		return false;
	}

	newAngle = -1;
	char cmd = Serial.read ();
	if (cmd > 0x20) {
		currentCommand = cmd;
		newAngle = Serial.parseInt ();
		if (newAngle > SERVO_MAX_POINT || newAngle < SERVO_MIN_POINT) {
			Serial.print ( F ( "ANGLE " ) );
			Serial.print ( F ( "ANGLE " ) );
			Serial.print ( newAngle );
			Serial.println ( F ( " IS OUT OF RANGE" ) );

			newAngle = -1;
		}
		return true;
	}

	currentCommand = '?';
	return false;
}

void getPotVal () {
	potVal = analogRead ( POT_PIN );
	potVal = map ( potVal, 0, 1023, SERVO_MIN_POINT, SERVO_MAX_POINT );
	if (potVal == lastPotVal) return;
	lastPotVal = potVal;
	servo->point ( potVal );
	Serial.print ( F ( "POT=" ) );
	Serial.println ( potVal );
}

void loop () {
	if (!checkForCommand ()) {
		getPotVal ();
		return;
	}

	int switchVal = digitalRead ( SWITCH_PIN );
	if (switchVal == HIGH) {
		Serial.println ( F ( "HIGH" ) );
		turnOff ( GREEN_LED );
		flashPin ( RED_LED );
		turnOn ( RED_LED );
	} else {
		Serial.println ( F ( "LOW" ) );
		turnOff ( RED_LED );
		flashPin ( GREEN_LED );
		turnOn ( GREEN_LED );
	}


	Serial.print ( F ( "Command:" ) );
	Serial.println ( currentCommand );

	switch (toUpperCase ( currentCommand )) {

	case 0x10:
	case 0x13:
		break;

	case '-':
		servo->detach ();
		showServoStatus ();
		break;

	case '+':
		servo->attach ( servoPin );
		showServoStatus ();
		break;

	case 'P':
		if (newAngle > SERVO_MIN_POINT) {
			servo->point ( newAngle );
		}
		break;
		//
	case 'C':
		servo->center ();
		break;

	case 'S':
		servo->sweep ();
		break;

	case 'F':
		servo->fastSweep ();
		break;

	case 'L':
		pointLefter ( 9 );
		break;
		break;

	case 'R':
		pointRighter ( 9 );
		break;

	case 'X':
		servo->point ( SERVO_MIN_POINT );
		delay ( 500 );
		servo->point ( SERVO_MAX_POINT );
		break;

	case 'V':
		Serial.print ( F ( "Servo value is " ) );
		Serial.println ( servo->read () );
		break;

	case '?':
		showCommands ();
		break;

	default:
		Serial.print ( F ( "Unknown command: [" ) );
		Serial.print ( currentCommand, HEX );
		Serial.println ( F ( "]" ) );
		break;
	}
	//delay ( 10 );
}

void pointLefter ( int increment ) {
	currentAngle = servo->read ();
	newAngle = ( currentAngle < 180 ) ? currentAngle + increment : currentAngle;
	servo->point ( newAngle );

}

void pointRighter ( int increment ) {
	currentAngle = servo->read ();
	newAngle = ( currentAngle > 0 ) ? currentAngle - increment : currentAngle;
	servo->point ( newAngle );
}

void showServoStatus () {
	Serial.print ( F ( "SERVO IS " ) );
	Serial.println ( ( servo->attached () ) ? "ATTACHED" : "DETACHED" );
}

void turnOn ( int pin ) {
	digitalWrite ( pin, HIGH );
}

void turnOff ( int pin ) {
	digitalWrite ( pin, LOW );
}

void flashPin ( int pin ) {
	flashPin ( pin, 10, 40 );
}

void flashPin ( int pin, int times, int duration ) {
	if (pin == 0) return;
	for (int i = 0; i < times; i++) {
		turnOn (pin);
		delay ( duration );
		turnOff ( pin );
		delay ( duration );
	}
}