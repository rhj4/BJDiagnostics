#pragma once
#include "Config.h"
#include "MotorDriverBase.h"
#include <Servo.h>

class DriverServo : public MotorDriverBase {
public:
	char *_motorType = ( char* )"Servo";

	//	Servo Motor Variables

	Servo leftServo;
	Servo rightServo;

#define CENTER 90
#define POINT_FORWARD 180
#define POINT_STOPPED 90
#define POINT_BACKWARD 0

	void init ( ) {

		//	motor one
		leftServo.attach ( LEFT_SERVO_PIN, 1000, 2000 );
		if (!leftServo.attached ()) {
			Serial.println ( F ( "LEFT SERVO NOT ATTACHED" ) );
		} else {
			leftServo.write ( 45 );
			delay ( 1000 );
			leftServo.write ( 135 );
			delay ( 1000 );
			leftServo.write ( 90 );

		}
		rightServo.attach ( RIGHT_SERVO_PIN, 1000, 2000 );
		if (!rightServo.attached ()) {
			Serial.println ( F ( "RIGHT SERVO NOT ATTACHED" ) );
		} else {
			rightServo.write ( 135 );
			delay ( 1000 );
			rightServo.write ( 45 );
			delay ( 1000 );
			rightServo.write ( 90 );
		}

		Serial.println ( F ( "T4) Servo" ) );
		Serial.println ( F ( "Servo Setup Completed" ) );
	}

	void reset () {
		leftServo.detach ();
		rightServo.detach ();
		delay ( 100 );
		leftServo.attach ( LEFT_SERVO_PIN );
		rightServo.attach ( RIGHT_SERVO_PIN );
	}

	/*****************************************************
	*
	*	MOTOR PRIMITIVES
	*
	*/
	//	Primitive Motor Methods
	void setLeftSpeed ( int speed ) {
	}

	void setRightSpeed ( int speed ) {
	}

	void leftStop () {
		_leftSpeed = 0;
		leftDirection = POINT_STOPPED;
		signal ( (char*)"Left stop", leftDirection );

		if (swapMotors) {
			leftServo.write( leftDirection );
		} else {
			rightServo.write ( leftDirection );
		}

		//setLeftSpeed ( 0 );
		//showLeftStatus ();
	}

	void rightStop () {
		_rightSpeed = 0;
		rightDirection = POINT_STOPPED;
		signal ( ( char* )"Right stop", rightDirection );

		if (swapMotors) {
			rightServo.write ( rightDirection );
		} else {
			leftServo.write ( rightDirection );
		}

		//setRightSpeed ( 0 );
		//showRightStatus ();
	}

	virtual void leftForward ( int speed ) {
		// set the motor speed and direction
		_leftSpeed = speed;
		leftDirection = POINT_FORWARD;
		signal ( ( char* )"Left Forward", leftDirection );

		if (swapMotors) {
			leftServo.write ( leftDirection );
		} else {
			rightServo.write ( leftDirection );
		}


		//setLeftSpeed ( speed );
		//showLeftStatus ();
	}

	virtual void rightForward ( int speed ) {
		// set the motor speed and direction
		_rightSpeed = speed;
		rightDirection = POINT_FORWARD;
		signal ( ( char* )"Right Forward: ", rightDirection );

		if (swapMotors) {
			rightServo.write ( rightDirection );
		} else {
			leftServo.write ( rightDirection );
		}

		//setRightSpeed ( speed );
		//Serial.println ( F ( "\nSET RIGHT SPEED: AFTER" ) );
		//showRightStatus ();
		//Serial.println ( F ( "\nSHOW RIGHT STATUS: AFTER" ) );
	}

	void leftBack ( int speed ) {
		_leftSpeed = speed;
		leftDirection = POINT_BACKWARD;
		signal ( ( char* )"Left Backward", leftDirection );

		if (swapMotors) {
			rightServo.write ( leftDirection );
		} else {
			leftServo.write ( leftDirection );
		}

		//setLeftSpeed ( speed );
		//delay ( 100 );
		//showLeftStatus ();
		//delay ( 100 );
	}

	void rightBack ( int speed ) {
		_rightSpeed = speed;
		rightDirection = POINT_BACKWARD;
		signal ( ( char* )"Right Backward", rightDirection );

		if (swapMotors) {
			leftServo.write ( rightDirection );
		} else {
			rightServo.write ( rightDirection );
		}

		//setRightSpeed ( speed );
		//showRightStatus ();
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
		Serial.println ( RIGHT_SERVO_PIN);
	}

	void showLeftPins () {
		Serial.print ( F ( "LEFT:" ) );
		Serial.println ( LEFT_SERVO_PIN );
	}
	
	void signal (char* label, int dir) {
		Serial.print ( label );
		Serial.println ( dir );
	}
};