/************************************************************************
*
*	Name:	MotorTester
*	Author:	Bob Jones <bob@rhj4.com>
*	Version	2.0.0
*	Description: Tests L298N and L9110 H-Bridge Motor Controllers
*	in a two-motor robot. See Config.h for settings.
*
*	This can easily be extended to cover additional motor types by
*	adding appropriate code in Setup, Stop, Left & Right Forward & Back.
*
*	Source: Code derived from example at https://www.bananarobotics.com/shop/How-to-use-the-HG7881-(L9110)-Dual-Channel-Motor-Driver-Module
************************************************************************/

#include "Config.h"
#include "DriverL298N.h"
#include "DriverL293D.h"
#include "DriverL9110.h"
#include "DriverServo.h"

DriverL298N l298n;
DriverL293D l293d;
DriverL9110 l9110;
DriverServo servo;

MotorDriverBase *motor;

// connect motor controller pins to Arduino digital pins
int motorType = MOTOR_TYPE_DEFAULT;
bool _motorConfigured = false;

void setup () {
	Serial.begin ( 115200 );
	Serial.println ();
	Serial.print ( (char*) NAME );
	Serial.print ( F ( " Version " ) );
	Serial.println ( (char*) VERSION );
	delay ( 100 );

	configureMotor ( motorType );
	motor->setup ();
	showMenu ();
}

void configureMotor ( int motorType ) {
	_motorConfigured = false;
	l298n.reset ();
	l293d.reset ();
	l9110.reset ();
	servo.reset ();
	switch (motorType) {

	case MOTOR_TYPE_L298N:
		motor = &l298n;
		motor->init ();
		motor->forwardPin = L298N_FORWARD_PIN;
		motor->backwardPin = L298N_BACKWARD_PIN;
		motor->stoppedPin = L298N_STOPPED_PIN;
		_motorConfigured = true;
		break;

	case MOTOR_TYPE_L293D:
		motor = &l293d;
		l293d.init ();
		motor->forwardPin = L293D_FORWARD_PIN;
		motor->backwardPin = L293D_BACKWARD_PIN;
		motor->stoppedPin = L293D_STOPPED_PIN;
		_motorConfigured = true;
		break;

	case MOTOR_TYPE_L9110:
		motor = &l9110;
		l9110.init ();
		motor->forwardPin = L9110_FORWARD_PIN;
		motor->backwardPin = L9110_BACKWARD_PIN;
		motor->stoppedPin = L9110_STOPPED_PIN;
		_motorConfigured = true;
		break;

	case MOTOR_TYPE_SERVO:
		motor = &servo;
		motor->init ();
		_motorConfigured = true;
		break;

	default:
		unknownMotorType ();
		break;
	}
}

void loop () {
	boolean isValidInput = true;
	do {
		char cmd;
		// get the next character from the serial port
		while (!Serial.available ()); // LOOP UNTIL INPUT AVAILABLE...

		cmd = Serial.read ();
		Serial.print ( F ( "\nCMD: " ) );
		Serial.println ( cmd );

		// execute the menu option based on the character recieved
		switch (toUpperCase ( cmd )) {

		case 13:	//	Skip CR
		case 10:	//	Skip LF
		case 0xFFFFFFF0:
			break;

		case 'T':
			motorType = Serial.parseInt ();
			configureMotor ( motorType );
			showActive ();
			break;

		case 'R':
			if (motorConfigured ()) {
				motor->rightMotorActive = true;
				motor->leftMotorActive = false;
				showActive ();
			}
			break;

		case 'L':
			if (motorConfigured ()) {
				motor->leftMotorActive = true;
				motor->rightMotorActive = false;
				showActive ();
			}
			break;

		case 'N':
			if (motorConfigured ()) {
				motor->leftMotorActive = false;
				motor->rightMotorActive = false;
				showActive ();
			}
			break;

		case 'F':
			Serial.println ( F ( "\nFORWARD" ) );
			showActive ();
			if (motorConfigured ()) {
				if (motor->leftMotorActive) {
					motor->leftForward ( NORMAL_SPEED );
				} else if (motor->rightMotorActive) {
					motor->rightForward ( NORMAL_SPEED );
				} else {
					motor->leftForward ( NORMAL_SPEED );
					motor->rightForward ( NORMAL_SPEED );
				}
			}
			break;

		case 'B':
			Serial.println ( F ( "\nBACKWARD" ) );
			showActive ();
			if (motorConfigured ()) {
				if (motor->leftMotorActive) {
					motor->leftBack ( NORMAL_SPEED );
				} else if (motor->rightMotorActive) {
					motor->rightBack ( NORMAL_SPEED );
				} else {
					motor->leftBack ( NORMAL_SPEED );
					motor->rightBack ( NORMAL_SPEED );
				}
			}
			break;

		case 'S':
			Serial.println ( F ( "\nSTOP" ) );
			showActive ();
			if (motorConfigured ()) {
				if (motor->leftMotorActive) {
					motor->leftStop ();
				} else if (motor->rightMotorActive) {
					motor->rightStop ();
				} else {
					motor->leftStop ();
					motor->rightStop ();
				}
			}
			break;

		case '+':
			Serial.println ( F ( "\nFASTER" ) );
			if (motorConfigured ()) {
				if (motor->leftMotorActive) {
					motor->leftFaster ( INCREMENT );
				} else if (motor->rightMotorActive) {
					motor->rightFaster ( INCREMENT );
				} else {
					motor->leftFaster ( INCREMENT );
					motor->rightFaster ( INCREMENT );
				}
			}
			break;

		case '-':
			Serial.println ( F ( "\nSLOWER" ) );
			if (motorConfigured ()) {
				if (motor->leftMotorActive) {
					motor->leftSlower ( DECREMENT );
				} else if (motor->rightMotorActive) {
					motor->rightSlower ( DECREMENT );
				} else {
					motor->leftSlower ( DECREMENT );
					motor->rightSlower ( DECREMENT );
				}
			}
			break;

		case '#':
			motor->tracePins = !motor->tracePins;
			Serial.print ( F ( "Pin tracing is " ) );
			Serial.println ( ( motor->tracePins )?"ENABLED":"DISABLED" );
			break;

		case 'P':
			Serial.println ( F ( "\nPIN VALUES:" ) );
			if (motorConfigured ()) {
				if (motor->leftMotorActive) {
					motor->showLeftPins ();
				} else if (motor->rightMotorActive) {
					motor->showRightPins ();
				} else {
					motor->showRightPins ();
					motor->showLeftPins ();
				}
			}
			break;

		case'M':
			showActive ();
			if (motorConfigured ()) {
				if (motor->leftMotorActive) {
					motor->showLeftStatus ();
				} else if (motor->rightMotorActive) {
					motor->showRightStatus ();
				} else {
					motor->showStatus ();
				}
			}
			break;
			break;

		case 'X':
			if (motorConfigured ()) {
				motor->swapMotors = !motor->swapMotors;
				Serial.print ( F ( "Swap Motors: " ) );
				Serial.println ( ( motor->swapMotors )?"ON":"OFF" );
			}
			break;

		case 'Z':
			Serial.println ( F ( "\nFlashing Direction Pins" ) );
			motor->flashAll ();
			break;

		case '0':
			Serial.println ( F ( "0: STOP" ) );
			showActive ();
			if (motorConfigured ()) {
				motor->stop ();
			}
			break;

		case '1':
			Serial.println ( F ( "\n1: LEFT FORWARD" ) );
			showActive ();
			if (motorConfigured ()) {
				motor->leftForward ( NORMAL_SPEED );
			}
			break;

		case '2':
			Serial.println ( F ( "\n2: RIGHT FORWARD" ) );
			showActive ();
			if (motorConfigured ()) {
				motor->rightForward ( NORMAL_SPEED );
			}
			break;

		case '3':
			showActive ();
			Serial.println ( F ( "\n3: FORWARD" ) );
			if (motorConfigured ()) {
				motor->forward ( NORMAL_SPEED );
			}
			break;

		case '4':
			showActive ();
			Serial.println ( F ( "\n4: LEFT BACKWARD" ) );
			if (motorConfigured ()) {
				motor->leftBack ( NORMAL_SPEED );
			}
			break;

		case '5':
			showActive ();
			Serial.println ( F ( "\n5: RIGHT BACKWARD" ) );
			if (motorConfigured ()) {
				motor->rightBack ( NORMAL_SPEED );
			}
			break;

		case '6':
			showActive ();
			Serial.println ( F ( "\n6: BACKWARD" ) );
			if (motorConfigured ()) {
				motor->back ( NORMAL_SPEED );
			}
			break;

		case '7': // 1) Fast forward accelerating
			showActive ();
			Serial.println ( F ( "\n7: ACCELERATION TEST" ) );
			if (motorConfigured ()) {
				motor->accelerationForward ();
			}
			break;

		case '8': // 4) Reverse Accelerating
			Serial.println ( F ( "\n8: DECELERATION TEST" ) );
			showActive ();
			if (motorConfigured ()) {
				motor->accelerationBackward ();
			}
			break;

		case '9':
			Serial.println ( F ( "\n9: MOTOR STATUS" ) );
			showActive ();
			if (motorConfigured ()) {
				motor->showMotorStatus ();
			}
			break;

		case '?':
			showMenu ();
			break;

		default:
			// wrong character! display the menu again!
			isValidInput = false;
			Serial.print ( F ( "Invalid command: [" ) );
			Serial.print ( cmd, HEX );
			Serial.println ( F ( "]" ) );
			break;

		}

	} while (isValidInput == true);

	delay ( 1000 );
}

bool motorConfigured () {
	if (!_motorConfigured) {
		Serial.println ( F ( "*** MOTOR TYPE NOT SELECTED ***" ) );
		return false;
	}
	return true;
}

void showActive () {
	Serial.print ( F ( "Left Motor Active: " ) );
	Serial.print ( ( motor->leftMotorActive )?"TRUE":"FALSE" );
	Serial.print ( F ( ", Right Motor Active: " ) );
	Serial.print ( ( motor->rightMotorActive )?"TRUE":"FALSE" );
	Serial.println ();
}
void showMenu () {
	// draw a menu on the serial port
	Serial.println ( "-----------------------------" );
	Serial.println ( "MENU:" );
	Serial.println ( F ( "T1) L298N" ) );
	Serial.println ( F ( "T2) L293D" ) );
	Serial.println ( F ( "T3) L9110" ) );
	Serial.println ( F ( "T4) Servo" ) );

	Serial.println ( F ( "\nL) Set Left Motor Active" ) );
	Serial.println ( F ( "R) Set Right Motor Active" ) );
	Serial.println ( F ( "F) Active Motor Forward" ) );
	Serial.println ( F ( "N) Neither motor Active" ) );
	Serial.println ( F ( "B) Active Motor Backward" ) );
	Serial.println ( F ( "S) Active Motor Stop" ) );
	Serial.println ( F ( "+) Active Motor Faster" ) );
	Serial.println ( F ( "-) Active Motor Slower" ) );
	Serial.println ( F ( "#) Toggle Pin Tracing" ) );
	Serial.println ( F ( "P) Active Motor Pin Values" ) );
	Serial.println ( F ( "M) Show Motor Status" ) );
	Serial.println ( F ( "X) Swap motors" ) );
	Serial.println ( F ( "Z) Flash LEDs" ) );

	Serial.println ( F ( "\n0) Stop" ) );
	Serial.println ( F ( "1) Left Motor Forward" ) );
	Serial.println ( F ( "2) Right Motor forward" ) );
	Serial.println ( F ( "3) Both Motors Forward" ) );
	Serial.println ( F ( "4) Left Motor Back" ) );
	Serial.println ( F ( "5) Right Motor Back" ) );
	Serial.println ( F ( "6) Both Motors Back" ) );
	Serial.println ( F ( "7) Accelerating forward" ) );
	Serial.println ( F ( "8) Accelerating Reverse" ) );
	Serial.println ( F ( "9) Motor Pin Setup" ) );
	Serial.println ( F ( "?) Show this menu" ) );
	Serial.println ( "-----------------------------" );
}

void unknownMotorType () {
	Serial.print ( F ( "Unknown Motor Type: " ) );
	Serial.println ( motorType );
}


