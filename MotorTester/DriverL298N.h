#pragma once
#include "Config.h"
#include "MotorDriverBase.h"

class DriverL298N : public MotorDriverBase {
public:
	char *_motorType = (char*)"L298N";

	//	L298N Motor Variables
#if SWAP_MOTORS
	// Left motor
	int l298NLeftSpeedPin = L298N_ENB;
	int l298NLeftForwardPin = L298N_IN3;
	int l298NLeftBackwardPin = L298N_IN4;
	// Right motor 
	int l298NRightSpeedPin = L298N_ENA;
	int l298NRightForwardPin = L298N_IN1;
	int l298NRightBackwardPin = L298N_IN2;
#else
	// Left motor
	int l298NLeftSpeedPin = L298N_ENA;
	int l298NLeftForwardPin = L298N_IN1;
	int l298NLeftBackwardPin = L298N_IN2;
	// Right motor 
	int l298NRightSpeedPin = L298N_ENB;
	int l298NRightForwardPin = L298N_IN3;
	int l298NRightBackwardPin = L298N_IN4;
#endif

	DriverL298N () {}

	void init ( ) {
		pinMode ( l298NLeftForwardPin, OUTPUT );
		pinMode ( l298NLeftBackwardPin, OUTPUT );
		pinMode ( l298NLeftSpeedPin, OUTPUT );
		pinMode ( l298NRightForwardPin, OUTPUT );
		pinMode ( l298NRightBackwardPin, OUTPUT );
		pinMode ( l298NRightSpeedPin, OUTPUT );

		Serial.println ( F ( "L9298N Setup Completed" ) );
	}

	void reset () {
		//	Reset Pins used by L298N mode
		pinMode ( L298N_ENA, INPUT );
		pinMode ( L298N_IN1, INPUT );
		pinMode ( L298N_IN2, INPUT );
		pinMode ( L298N_ENB, INPUT );
		pinMode ( L298N_IN3, INPUT );
		pinMode ( L298N_IN4, INPUT );
	}

	/*****************************************************
	*
	*	MOTOR PRIMITIVES
	*
	*/
	//	Primitive Motor Methods
	void setLeftSpeed ( int speed ) {
		_leftSpeed = speed;
		Serial.print ( F ( "\nSetting left speed to " ) );
		Serial.println ( speed );
		digitalWrite ( l298NLeftSpeedPin, speed );
	}

	void setRightSpeed ( int speed ) {
		_rightSpeed = speed;
		Serial.print ( F ( "\nSetting right speed to " ) );
		Serial.println ( speed );
		digitalWrite ( l298NRightSpeedPin, speed );
	}

	void leftStop () {
		_leftSpeed = 0;
		leftDirection = STOPPED;

		digitalWrite ( l298NLeftForwardPin, LOW );
		digitalWrite ( l298NLeftBackwardPin, LOW );

		setLeftSpeed ( 0 );
		showLeftStatus ();
		signalStopped ();
	}

	void rightStop () {
		_rightSpeed = 0;
		rightDirection = STOPPED;

		digitalWrite ( l298NRightForwardPin, LOW );
		digitalWrite ( l298NRightBackwardPin, LOW );

		setRightSpeed ( 0 );
		showRightStatus ();
		signalStopped ();
	}

	void leftForward ( int speed ) {
		// set the motor speed and direction
		setLeftSpeed ( 0 );

		_leftSpeed = speed;
		leftDirection = FORWARD;

		digitalWrite ( l298NLeftForwardPin, HIGH);
		digitalWrite ( l298NLeftBackwardPin, LOW );
		setLeftSpeed ( speed );
		showLeftStatus ();
		signalForward ();
	}

	void rightForward ( int speed ) {
		// set the motor speed and direction
		setRightSpeed ( 0 );

		_rightSpeed = speed;
		rightDirection = FORWARD;

		digitalWrite ( l298NRightForwardPin, HIGH );
		digitalWrite ( l298NRightBackwardPin, LOW );

		setRightSpeed ( speed );
		showRightStatus ();
		signalForward ();
	}

	void leftBack ( int speed ) {
		setLeftSpeed ( 0 );
		_leftSpeed = speed;
		leftDirection = BACKWARD;

		digitalWrite ( l298NLeftForwardPin, LOW );
		digitalWrite ( l298NLeftBackwardPin, HIGH );

		setLeftSpeed ( speed );
		showLeftStatus ();
		signalBackward ();
	}

	void rightBack ( int speed ) {
		setRightSpeed ( 0 );
		_rightSpeed = speed;
		rightDirection = BACKWARD;

		digitalWrite ( l298NRightForwardPin, LOW);
		digitalWrite ( l298NRightBackwardPin, HIGH );

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
		Serial.print ( F ( "\nMOTOR TYPE: " ) );
		Serial.println ( _motorType );
	}

	void showRightPins () {
		Serial.print ( F ( "RIGHT:" ) );
		showPin ( ( char* )"ENB", l298NRightSpeedPin, true );
		showPin ( ( char* )",IN3", l298NRightForwardPin, false );
		showPin ( ( char* )",IN4", l298NRightBackwardPin, false );
		Serial.println ();
	}

	void showLeftPins () {
		Serial.print ( F ( "LEFT:" ) );
		showPin ( ( char* )"ENA", l298NLeftSpeedPin, true );
		showPin ( ( char* )",IN1", l298NLeftForwardPin, false );
		showPin ( ( char* )",IN2", l298NLeftBackwardPin, false );
		Serial.println ();
	}
};
