#pragma once
#include "Config.h"
class MotorDriverBase {
public:
	bool swapMotors = SWAP_MOTORS;
	bool tracePins = false;

	int _leftSpeed = 0;
	int _rightSpeed = 0;
	bool leftMotorActive = false;
	bool rightMotorActive = false;

	int leftDirection = STOPPED;
	int rightDirection = STOPPED;
	int forwardPin = 0;
	int backwardPin = 0;
	int stoppedPin = 0;

	virtual void init ( ) = 0;

	//	Primitive Motor Methods
	virtual void setLeftSpeed ( int speed ) = 0;
	virtual void leftStop () = 0;
	virtual void leftForward ( int speed ) = 0;
	virtual void leftBack ( int speed ) = 0;
	virtual void showLeftPins () = 0;

	virtual void setRightSpeed ( int speed ) = 0;
	virtual void rightStop () = 0;
	virtual void rightForward ( int speed ) = 0;
	virtual void rightBack ( int speed ) = 0;
	virtual void showRightPins () = 0;

	virtual void showMotorType () = 0;

	void setup () {
		if (stoppedPin > 0) {
			pinMode ( stoppedPin, OUTPUT );
			flashPin ( stoppedPin );
		}
		if (forwardPin > 0) {
			pinMode ( forwardPin, OUTPUT );
			flashPin ( forwardPin );
		}
		if (backwardPin > 0) {
			pinMode ( backwardPin, OUTPUT );
			flashPin ( backwardPin );
		}
	}

	void flashAll () {
		flashPin ( forwardPin );
		flashPin ( backwardPin );
		flashPin ( stoppedPin );
	}
	void signalStopped () {
		if (stoppedPin > 0) {
			Serial.println ( F ( "Signaling Stopped" ) );
			turnPinOff ( forwardPin );
			turnPinOff ( backwardPin );
			flashPin ( stoppedPin );
			turnPinOn ( stoppedPin );
		}
	}

	void signalForward () {
		if (forwardPin > 0) {
			Serial.println ( F ( "Signaling Forward" ) );
			turnPinOff ( stoppedPin );
			turnPinOff ( backwardPin );
			turnPinOn ( forwardPin );
		}
	}
	
	void signalBackward () {
		if (backwardPin > 0) {
			Serial.println ( F ( "Signaling Backward" ) );
			turnPinOff ( forwardPin );
			turnPinOff ( stoppedPin );
			flashPin ( backwardPin );
			turnPinOn ( backwardPin );
		}

	}

	void turnPinOn ( int pin ) {
		if (tracePins) {
			Serial.print ( F ( "Setting PIN " ) );
			Serial.print ( pin );
			Serial.println ( F ( " HIGH" ) );
		}
		digitalWrite ( pin, HIGH );
	}

	void turnPinOff ( int pin ) {
		digitalWrite ( pin, LOW );
	}

	void flashPin ( int pin ) {
		flashPin ( pin, 5, 50 );
	}

	void flashPin ( int pin, int count, int duration ) {
		for (int i = 0; i < count; i++) {
			turnPinOn ( pin );
			delay ( duration );
			turnPinOff ( pin );
			delay ( duration );
		}
	}

	//	Compound Motor Methods
	void stop () {
		Serial.println ( F ( "STOP" ) );
		leftStop ();
		rightStop ();
	}

	void forward ( int speed ) {
		Serial.println ( "FORWARD..." );
		if (leftMotorActive) {
			leftForward ( speed );
		} else if (rightMotorActive) {
			rightForward ( speed );
		} else {
			leftForward ( speed );
			rightForward ( speed );
		}
	}

	void back ( int speed ) {
		Serial.println ( "BACK" );
		if (leftMotorActive) {
			leftBack ( speed );
		} else if (rightMotorActive) {
			rightBack ( speed );
		} else {
			leftBack ( speed );
			rightBack ( speed );
		}
	}

	void setSpeed ( int speed ) {
		setLeftSpeed ( speed );
		setRightSpeed ( speed );
	}

	void faster ( int increment ) {
		leftFaster ( increment );
		rightFaster ( increment );
	}

	void slower ( int decrement ) {
		leftSlower ( decrement );
		rightSlower ( decrement );
	}

	void leftFaster ( int increment ) {
		_leftSpeed += increment;
		if (_leftSpeed > MAX_SPEED) {
			_leftSpeed = MAX_SPEED;
		}
		setLeftSpeed ( _leftSpeed );
	}

	void rightFaster ( int increment ) {
		_rightSpeed += increment;
		if (_rightSpeed > MAX_SPEED) {
			_rightSpeed = MAX_SPEED;
		}
		setRightSpeed ( _rightSpeed );
	}

	void leftSlower ( int decrement ) {
		_leftSpeed -= decrement;
		if (_leftSpeed < MIN_SPEED) {
			_leftSpeed = MIN_SPEED;
		}
		setLeftSpeed ( _leftSpeed );
	}

	void rightSlower ( int decrement ) {
		_rightSpeed -= decrement;
		if (_rightSpeed < MIN_SPEED) {
			_rightSpeed = MIN_SPEED;
		}
		setRightSpeed ( _rightSpeed );
	}

	void accelerationForward () {
		Serial.println ( "Accelerating forward..." );

		// always stop motors briefly before abrupt changes
		stop ();
		delay ( DELAY );
		forward ( MIN_SPEED );

		// set the motor speed and direction
		for (int i = MIN_SPEED; i <= MAX_SPEED; i += INCREMENT) {
			if (leftMotorActive) {
				setLeftSpeed ( i );
			} else if (rightMotorActive) {
				setRightSpeed ( i );
			} else {
				setSpeed ( i );
			}

			if (Serial.available ()) {
				Serial.read ();
				break;
			}

			delay ( LOOP_DELAY );
		}
		stop ();
	}


	void accelerationBackward () {
		Serial.println ( "Accelerating Reverse..." );

		// always stop motors briefly before abrupt changes
		stop ();
		delay ( 100 );
		back ( MIN_SPEED );
		for (int i = MAX_SPEED; i >= MIN_SPEED; i -= DECREMENT) {
			if (leftMotorActive) {
				setLeftSpeed ( i );
			} else if (rightMotorActive) {
				setRightSpeed ( i );
			} else {
				setSpeed ( i );
			}
			if (Serial.available ()) {
				Serial.read ();
				break;
			}

			delay ( LOOP_DELAY );
		}

		stop ();
	}

	void showLeftSpeed () {
		Serial.print ( F ( "LEFT DIRECTION = " ) );
		showDirection ( leftDirection );
		Serial.print ( F ( ", SPEED = " ) );
		Serial.println ( _leftSpeed );
	}

	void showRightSpeed () {
		Serial.print ( F ( "RIGHT DIRECTION = " ) );
		showDirection ( rightDirection );
		Serial.print ( F ( ", SPEED = " ) );
		Serial.println ( _rightSpeed );
	}

	void showSpeed () {
		showLeftSpeed ();
		showRightSpeed ();
	}

	void showDirection ( int direction ) {
		switch (direction) {
		case FORWARD:
			Serial.print ( F ( "FORWARD" ) );
			break;
		case BACKWARD:
			Serial.print ( F ( "BACKWARD" ) );
			break;
		case STOPPED:
			Serial.print ( F ( "STOPPED" ) );
			break;
		}
		delay ( 100 );	// Settling time while I chase a moth.
	}

	void showStatus () {
		if (leftMotorActive) {
			showLeftStatus ();
		} else if (rightMotorActive) {
			showRightStatus ();
		} else {
			showLeftStatus ();
			showRightStatus ();
		}
	}

	void showPin ( char* label, int pin, bool showAnalog ) {
		Serial.print ( label );
		Serial.print ( ": PIN " );
		Serial.print ( pin );
		Serial.print ( '=' );
		//if (showAnalog) {
		//	Serial.print ( analogRead ( pin ) );
		//} else {
			Serial.print ( ( digitalRead ( pin ) )?"HIGH":"LOW" );
//		}
	}

	void showLeftStatus () {
		showLeftSpeed ();
		showLeftPins ();
		delay ( 1000 );
		Serial.println ( F ( "\nSHOW LEFT STATUS DONE" ) );
	}

	void showRightStatus () {
		showRightSpeed ();
		showRightPins ();
		Serial.println ( F ( "SHOW RIGHT STATUS DONE" ) );
		delay ( 1000 );
	}

	void showMotorStatus () {
		showMotorType ();
		showLeftStatus ();
		showRightStatus ();
		Serial.println ();
	}


};