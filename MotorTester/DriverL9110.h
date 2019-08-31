#pragma once
#include "Config.h"
#include "MotorDriverBase.h"

class DriverL9110 : public MotorDriverBase {
public:
	char *_motorType = ( char* )"L9110";

	//	L9110 Motor Variables

	// motor one
	int l9110_ib1 = L9110_MOTOR_A_PWM;
	int l9110_ia1 = L9110_MOTOR_A_DIR;
	// motor two
	int l9110_ib2 = L9110_MOTOR_B_PWM;
	int l9110_ia2 = L9110_MOTOR_B_DIR;

	void init ( ) {

		//	motor one
		pinMode ( L9110_MOTOR_A_PWM, OUTPUT );
		pinMode ( L9110_MOTOR_A_DIR, OUTPUT );
		// motor two
		pinMode ( L9110_MOTOR_B_PWM, OUTPUT );
		pinMode ( L9110_MOTOR_B_DIR, OUTPUT );

		Serial.println ( F ( "L9110 Setup Completed" ) );
	}

	void reset () {
		//	Set pins used by L9110 mode
		pinMode ( L9110_MOTOR_A_PWM, INPUT );
		pinMode ( L9110_MOTOR_A_DIR, INPUT );
		pinMode ( L9110_MOTOR_B_PWM, INPUT );
		pinMode ( L9110_MOTOR_B_DIR, INPUT );
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

		if (leftDirection == STOPPED) {
			if (swapMotors) {
				digitalWrite ( L9110_MOTOR_B_PWM, LOW );
			} else {
				digitalWrite ( L9110_MOTOR_A_PWM, LOW );
			}
		} else if (leftDirection == FORWARD) {
			if (swapMotors) {
				analogWrite ( L9110_MOTOR_B_PWM, 255 - speed ); // PWM speed = fast
			} else {
				analogWrite ( L9110_MOTOR_A_PWM, 255 - speed ); // PWM speed = fast
			}
		} else if (leftDirection == BACKWARD) {
			if (swapMotors) {
				analogWrite ( L9110_MOTOR_B_PWM, speed ); // PWM speed = fast
			} else {
				analogWrite ( L9110_MOTOR_A_PWM, speed ); // PWM speed = fast
			}
		}
	}

	void setRightSpeed ( int speed ) {
		_rightSpeed = speed;
		Serial.print ( F ( "\nSetting right speed to " ) );
		Serial.println ( speed );

		if (rightDirection == STOPPED) {
			if (swapMotors) {
				digitalWrite ( L9110_MOTOR_A_PWM, LOW );
			} else {
				digitalWrite ( L9110_MOTOR_B_PWM, LOW );
			}
		} else if (rightDirection == FORWARD) {
			if (swapMotors) {
				analogWrite ( L9110_MOTOR_A_PWM, 255 - speed ); // PWM speed = fast
			} else {
				analogWrite ( L9110_MOTOR_B_PWM, 255 - speed ); // PWM speed = fast
			}
		} else if (rightDirection == BACKWARD) {
			if (swapMotors) {
				analogWrite ( L9110_MOTOR_A_PWM, speed ); // PWM speed = fast
			} else {
				analogWrite ( L9110_MOTOR_B_PWM, speed ); // PWM speed = fast
			}
		}
	}

	void leftStop () {
		_leftSpeed = 0;
		leftDirection = STOPPED;

		if (swapMotors) {
			digitalWrite ( L9110_MOTOR_B_DIR, LOW );
		} else {
			digitalWrite ( L9110_MOTOR_A_DIR, LOW );
		}

		setLeftSpeed ( 0 );
		showLeftStatus ();
	}

	void rightStop () {
		_rightSpeed = 0;
		rightDirection = STOPPED;

		if (swapMotors) {
			digitalWrite ( L9110_MOTOR_A_DIR, LOW );
		} else {
			digitalWrite ( L9110_MOTOR_B_DIR, LOW );
		}

		setRightSpeed ( 0 );
		showRightStatus ();
	}

	virtual void leftForward ( int speed ) {
		// set the motor speed and direction
		_leftSpeed = speed;
		leftDirection = FORWARD;

		if (swapMotors) {
			digitalWrite ( L9110_MOTOR_B_DIR, HIGH ); // direction = forward
		} else {
			digitalWrite ( L9110_MOTOR_A_DIR, HIGH ); // direction = forward
		}

		setLeftSpeed ( speed );
		showLeftStatus ();
	}

	virtual void rightForward ( int speed ) {
		// set the motor speed and direction
		_rightSpeed = speed;
		rightDirection = FORWARD;

		if (swapMotors) {
			digitalWrite ( L9110_MOTOR_A_DIR, HIGH ); // direction = forward
		} else {
			digitalWrite ( L9110_MOTOR_B_DIR, HIGH ); // direction = forward
		}

		setRightSpeed ( speed );
		Serial.println ( F ( "\nSET RIGHT SPEED: AFTER" ) );
		showRightStatus ();
		Serial.println ( F ( "\nSHOW RIGHT STATUS: AFTER" ) );
	}

	void leftBack ( int speed ) {
		_leftSpeed = speed;
		leftDirection = BACKWARD;

		if (swapMotors) {
			digitalWrite ( L9110_MOTOR_B_DIR, LOW ); // direction = reverse      
		} else {
			digitalWrite ( L9110_MOTOR_A_DIR, LOW ); // direction = reverse      
		}

		setLeftSpeed ( speed );
		delay ( 100 );
		showLeftStatus ();
		delay ( 100 );
	}

	void rightBack ( int speed ) {
		_rightSpeed = speed;
		rightDirection = BACKWARD;

		if (swapMotors) {
			digitalWrite ( L9110_MOTOR_A_DIR, LOW ); // direction = reverse      
		} else {
			digitalWrite ( L9110_MOTOR_B_DIR, LOW ); // direction = reverse      
		}

		setRightSpeed ( speed );
		showRightStatus ();
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
			showPin ( ( char* )"PWM", L9110_IB2, false );
			showPin ( ( char* )",DIR", L9110_IA2, false );
		} else {
			showPin ( ( char* )"PWM", L9110_IB1, false );
			showPin ( ( char* )",DIR", L9110_IA1, false );
		}
		Serial.println ( F ( "\nSHOW RIGHT PINS DONE" ) );
	}

	void showLeftPins () {
		Serial.print ( F ( "LEFT:" ) );
		if (swapMotors) {
			showPin ( ( char* )"PWM", L9110_IB1, false );
			showPin ( ( char* )",DIR", L9110_IA1, false );
		} else {
			showPin ( ( char* )"PWM", L9110_IB2, false );
			showPin ( ( char* )",DIR", L9110_IA2, false );
		}
		Serial.println (F("\nSHOW LEFT PINS DONE"));
	}
};