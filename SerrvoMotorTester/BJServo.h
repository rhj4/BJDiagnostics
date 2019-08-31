#pragma once
#include "Config.h"

class BJServo {
public:
	int lastAngle = 0;

	Servo servo;
	BJServo () {
		servo.attach ( SERVO_PIN );
		if (!servo.attached ()) {
		}
	}

	void attach ( int pin ) {
		servo.attach ( SERVO_PIN );
	}

	void detach () {
		servo.detach ();
	}

	bool attached () {
		return servo.attached ();
	}

	bool configure ( char* name, int pin, int centerPoint, int minPoint, int maxPoint ) {
		strcpy ( _name, name );
		_centerPoint = centerPoint;
		_minPoint = minPoint;
		_maxPoint = maxPoint;

		Serial.print ( F ( "Configuring " ) );
		Serial.print ( name );
		Serial.print ( F ( " on pin" ) );
		Serial.println ( pin );

		servo.attach ( pin );
		if (servo.attached ()) {
			_configured = true;
			wiggle ();
			center ();
			Serial.print ( F ( "Servo configured on pin " ) );
			Serial.println ( pin );
			return true;
		}

		Serial.println ( F ( "Configuration Failed" ) );
		return false;
	}

	char* name () {
		return _name;
	}

	void center () {
		Serial.println ( F ( "Centering" ) );
		point ( _centerPoint );
	}

	void point ( int degrees ) {
		if (degrees == lastAngle) return;
		lastAngle = degrees;
		Serial.print ( F ( "Pointing " ) );
		Serial.println ( degrees );
		if (degrees >= _minPoint && degrees <= _maxPoint) {
			servo.write ( degrees );
			delay ( SERVO_DELAY );
		}
		int dir = servo.read ();
		int delta = ( abs ( dir - degrees ) > SERVO_SLOP );
		if (delta > 0) {
			Serial.print ( F ( "VARIANCE: " ));
			Serial.print ( delta );
			Serial.println ( F ( " DEGREES" ) );
		}
	}

	int read () {
		return servo.read ();
	}

	void sweep () {
		sweep ( _minPoint, _maxPoint, SERVO_SWEEP_INCREMENT, SERVO_PAUSE_DELAY );
	}

	void sweep ( int from, int to, int increment, uint16_t pause) {
		Serial.print ( F ( "Sweeping from " ) );
		Serial.print ( from );
		Serial.print ( F ( " to " ) );
		Serial.print ( to );
		Serial.println ();

		for (int i = from; i <= to; i += increment) {
			point ( i );
			delay ( pause );

		}
		for (int i = to; i >= from; i -= increment) {
			point ( i );
		}
	}

	void sweep ( int from, int to ) {
		sweep ( from, to, SERVO_SWEEP_INCREMENT, SERVO_PAUSE_DELAY );
	}

	void fastSweep (  ) {
		sweep ( _minPoint, _maxPoint, SERVO_SWEEP_FAST_INCREMENT, SERVO_PAUSE_DELAY );
	}

	void rotate ( bool direction) {
		while (!Serial.available()) {
			if (direction) {
				point ( 0 );
				delay ( 2000 );
			} else {
				point ( 180 );
				delay ( 2000 );
			}
		}
		point ( 1 );
		Serial.println ( F ( "Enter command:" ) );
	}

	bool test () {
		wiggle ();
		return true;
	}

protected:
	void wiggle () {
		servo.write ( 75 );
		delay ( 100 );
		servo.write ( 105 );
		delay ( 100 );
		center ();
	}

private:
	bool _configured = false;
	char _name [5];
	int _centerPoint = 90;
	int _minPoint = SERVO_MIN_POINT;
	int _maxPoint = SERVO_MAX_POINT;
};
