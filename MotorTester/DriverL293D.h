#pragma once
#include "Config.h"
#include "MotorDriverBase.h"

class DriverL293D : public MotorDriverBase {
public:
	char *_motorType = ( char* )"L293D";

	//	L293D Motor Variables
	// motor one
	int l293DLeftSpeedPin = L298N_ENA;
	int l293DLeftForwardPin = L298N_IN1;
	int l293DLeftBackwardPin = L298N_IN2;
	// motor two
	int l293DRightSpeedPin = L298N_ENB;
	int l293DRightForwardPin = L298N_IN3;
	int l293DRightBackwardPin = L298N_IN4;

	DriverL293D () {

	}

	void init ( ) {
		pinMode ( l293DLeftForwardPin, OUTPUT );
		pinMode ( l293DLeftBackwardPin, OUTPUT );
		pinMode ( l293DLeftSpeedPin, OUTPUT );
		pinMode ( l293DRightForwardPin, OUTPUT );
		pinMode ( l293DRightBackwardPin, OUTPUT );
		pinMode ( l293DRightSpeedPin, OUTPUT );

		Serial.println ( F ( "L9293D Setup Completed" ) );
	}

	void reset () {
		//	Reset pins used by L293D mode
		pinMode ( L293D_ENA, INPUT );
		pinMode ( L293D_ENB, INPUT );
		pinMode ( L293D_IN1, INPUT );
		pinMode ( L293D_IN2, INPUT );
		pinMode ( L293D_IN3, INPUT );
		pinMode ( L293D_IN4, INPUT );
	}

	/*****************************************************
	*
	*	MOTOR PRIMITIVES
	*
	*/

	//	Primitive Motor Methods
	void setLeftSpeed ( int speed ) {
		_leftSpeed = speed;
		Serial.print ( F ( "Setting left speed to " ) );
		Serial.println ( speed );

		if (swapMotors) {
			analogWrite ( L293D_ENB, speed );
		} else {
			analogWrite ( L293D_ENA, speed );
		}
	}

	void setRightSpeed ( int speed ) {
		_rightSpeed = speed;
		Serial.print ( F ( "Setting right speed to " ) );
		Serial.println ( speed );

		if (swapMotors) {
			analogWrite ( L298N_ENA, speed );
		} else {
			analogWrite ( L298N_ENB, speed );
		}
	}

	void leftStop () {
		_leftSpeed = 0;

		if (swapMotors) {
			digitalWrite ( L293D_IN1, LOW );
			digitalWrite ( L293D_IN2, LOW );
		} else {
			digitalWrite ( L293D_IN3, LOW );
			digitalWrite ( L293D_IN4, LOW );
		}

		leftDirection = STOPPED;
		setLeftSpeed ( 0 );
		showLeftPins ();
		signalStopped ();
	}

	void rightStop () {
		_rightSpeed = 0;

		if (swapMotors) {
			digitalWrite ( L293D_IN3, LOW );
			digitalWrite ( L293D_IN4, LOW );
		} else {
			digitalWrite ( L293D_IN1, LOW );
			digitalWrite ( L293D_IN2, LOW );
		}

		rightDirection = STOPPED;
		setRightSpeed ( 0 );
		showRightPins ();
		signalStopped ();
	}

	virtual void leftForward ( int speed ) {
		setSpeed ( 0 );
		// set the motor speed and direction
		_leftSpeed = speed;
		leftDirection = FORWARD;

		if (swapMotors) {
			digitalWrite ( L293D_IN1, HIGH );
			digitalWrite ( L293D_IN2, LOW );
		} else {
			digitalWrite ( L293D_IN3, HIGH );
			digitalWrite ( L293D_IN4, LOW );
		}

		setLeftSpeed ( speed );
		showLeftStatus ();
		signalForward ();
	}

	virtual void rightForward ( int speed ) {
		setSpeed ( 0 );
		// set the motor speed and direction
		_rightSpeed = speed;
		rightDirection = FORWARD;

		if (swapMotors) {
			digitalWrite ( L293D_IN3, HIGH );
			digitalWrite ( L293D_IN4, LOW );
		} else {
			digitalWrite ( L293D_IN1, HIGH );
			digitalWrite ( L293D_IN2, LOW );
		}

		setRightSpeed ( speed );
		showRightStatus ();
		signalForward ();
	}

	void leftBack ( int speed ) {
		setSpeed ( 0 );
		_leftSpeed = speed;
		leftDirection = BACKWARD;

		if (swapMotors) {
			digitalWrite ( L293D_IN1, LOW );
			digitalWrite ( L293D_IN2, HIGH );
		} else {
			digitalWrite ( L293D_IN3, LOW );
			digitalWrite ( L293D_IN4, HIGH );
		}

		setLeftSpeed ( speed );
		showLeftStatus ();
		signalBackward ();
	}

	void rightBack ( int speed ) {
		setSpeed ( 0 );
		_rightSpeed = speed;
		rightDirection = BACKWARD;

		if (swapMotors) {
			digitalWrite ( L293D_IN3, LOW );
			digitalWrite ( L293D_IN4, HIGH );
		} else {
			digitalWrite ( L293D_IN1, LOW );
			digitalWrite ( L293D_IN2, HIGH );
		}

		setRightSpeed ( speed );
		showRightStatus ();
		signalBackward ();
	}

	/*****************************************************
	*
	*	SHOW INFORMATION ABOUT MOTOR STATUS
	*
	*/
	void showMotorType () {
		Serial.print ( F ( "\nMotor Type: " ) );
		Serial.println ( _motorType );
	}

	void showRightPins () {
		Serial.print ( F ( "RIGHT:" ) );
		if (swapMotors) {
			showPin ( ( char* )"ENB", L293D_ENB, true );
			showPin ( ( char* )",IN3", L293D_IN3, false );
			showPin ( ( char* )",IN4", L293D_IN4, false );
		} else {
			showPin ( ( char* )"ENA", L293D_ENA, true );
			showPin ( ( char* )",IN1", L293D_IN1, false );
			showPin ( ( char* )",IN2", L293D_IN2, false );
		}
		Serial.println ();
	}

	void showLeftPins () {
		Serial.print ( F ( "LEFT:" ) );
		if (swapMotors) {
			showPin ( ( char* )"ENA", L293D_ENA, true );
			showPin ( ( char* )",IN1", L293D_IN1, false );
			showPin ( ( char* )",IN2", L293D_IN2, false );
		} else {
			showPin ( ( char* )"ENB", L293D_ENB, true );
			showPin ( ( char* )",IN3", L293D_IN3, false );
			showPin ( ( char* )",IN4", L293D_IN4, false );
		}
		Serial.println ();
	}

};