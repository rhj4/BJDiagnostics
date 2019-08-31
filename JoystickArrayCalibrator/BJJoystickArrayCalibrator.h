#pragma once
/********************************************************************
*
*	Name:	BJJoystickArrayCalibraor
*	Date:	August 21, 2019
*	Auth:	Bob Jones <bob@rhj4.com>
*	Desc:	Calibrates each joystick in array of joysticks
*
*	Notes:
*		Each joyStick has two inputs: X and Y values; however,
*		tThis handler takes only one of these values. Another
*		instance of this class is needed to read the other value.
*
*		There are three values that must be established for each
*		joyStick input. These values are generated during the
*		setup routine:
*			1)	minValue - lowest value this joyStick can read
*			2)	maxValue - highest value this joyStick can read
*			3)	midValue - value generated when joyStick is at rest
*
*********************************************************************/
#include "Config.h"
#include "BJJoystickArrayDefinitions.h"

joystickDefinition joySticks [ JOYSTICK_COUNT ] [ 13 ] = {
	{	JOYSTICK1X_PIN, JOYSTICK1Y_PIN,  JOYSTICK1_BUTTON, false,JOYSTICK1_CALIBRATED,
		JOYSTICK1_MIN_X, JOYSTICK1_MAX_X, JOYSTICK1_MID_X,
		JOYSTICK1_MIN_Y, JOYSTICK1_MAX_Y, JOYSTICK1_MID_Y,
		JOYSTICK1_SERVO_MOTOR_1, JOYSTICK1_SERVO_MOTOR_2},
	{	JOYSTICK2X_PIN,  JOYSTICK2Y_PIN,  JOYSTICK2_BUTTON,false,JOYSTICK2_CALIBRATED,
		JOYSTICK2_MIN_X, JOYSTICK2_MAX_X, JOYSTICK2_MID_X,
		JOYSTICK2_MIN_Y, JOYSTICK2_MAX_Y, JOYSTICK2_MID_Y,
		JOYSTICK2_SERVO_MOTOR_1, JOYSTICK2_SERVO_MOTOR_2 },
{	JOYSTICK3X_PIN,  JOYSTICK3Y_PIN,  JOYSTICK3_BUTTON,false,JOYSTICK3_CALIBRATED,
		JOYSTICK3_MIN_X, JOYSTICK3_MAX_X, JOYSTICK3_MID_X,
		JOYSTICK3_MIN_Y, JOYSTICK3_MAX_Y, JOYSTICK3_MID_Y,
		JOYSTICK3_SERVO_MOTOR_1, JOYSTICK3_SERVO_MOTOR_2},
};


class BJJoyStickArrayCalibrator {
private:
	uint16_t nextMotorNumber = STARTING_MOTOR_NUMBER;

	bool _paused = false;
	bool _tracing = JOYSTICK_TRACING;

protected:

public:
	BJJoyStickArrayCalibrator () {
		Serial.println ( F ( "Calibrator constructed" ) );
	}

	bool configure () {
		for (uint8_t i = 0; i < JOYSTICK_COUNT; i++) {
			pinMode ( ( uint8_t ) joySticks [ i ]->pinX, INPUT );
			pinMode ( ( uint8_t ) joySticks [ i ]->pinY, INPUT );
			pinMode ( ( uint8_t ) joySticks [ i ]->pinButton, INPUT_PULLUP );
			Serial.print ( F ( "Joystick #" ) );
			Serial.print ( i );
			Serial.print ( F ( " Configured with X on pin " ) );
			Serial.print ( joySticks [ i ]->pinX );
			Serial.print ( F ( " and Y on pin " ) );
			Serial.println ( joySticks [ i ]->pinY );
		}
		return true;

	}

	bool testButtons () {
#if TEST_BUTTONS
		bool pressed [ JOYSTICK_COUNT ] = {};
		for (uint8_t i = 0; i < JOYSTICK_COUNT; i++) {
			pressed [ i ] = false;
		}
		int unpressed = JOYSTICK_COUNT;

		Serial.print ( F ( "\nTESTING " ) );
		Serial.print ( JOYSTICK_COUNT );
		Serial.println ( F ( " BUTTONS" ) );

		Serial.println ( F ( "Press each button to test" ) );
		while (unpressed > 0) {

			for (uint8_t i = 0; i < JOYSTICK_COUNT; i++) {
				if (!pressed [ i ]) {
					if (buttonPressed ( i )) {
						_paused = false;
						pressed [ i ] = true;
						unpressed--;
					}
				}
			}
		}
		Serial.println ( F ( "Button tests completed" ) );
#endif /* TEST_BUTTONS */
		return true;
	}

	bool calibrateJoySticks () {
		for (uint8_t i = 0; i < JOYSTICK_COUNT; i++) {
			if (joySticks [ i ]->calibrated) {
				continue;
			}

			Serial.print ( F ( "\nCALIBRATING MIN/MAX VALUES FOR JOYSTICK #" ) );
			Serial.println ( i );
			if (getMinMaxValues ( i ) && getAtRestValue ( i ))  {
				showJoystickValues ( i );
				Serial.print ( F ( "Joystick #" ) );
				Serial.print ( i );
				Serial.println ( F ( " Calibrated" ) );
				assignMotors ( i );
			} else {
				Serial.print ( F ( "Joystick #" ) );
				Serial.print ( i );
				Serial.println ( F ( "Calibration failed" ) );
			}
		}
		return true;
	}

	bool getMinMaxValues ( uint8_t index ) {
		Serial.print ( F ( "\nCALIBRATING: MIN/MAX values for joystick #" ) );
		Serial.println ( index );
		Serial.print ( F ( "Initial X Min/Max/Mid values are: " ) );
		ShowCurrentValues ( index );
		Serial.print ( F ( "Reading X on pin " ) );
		Serial.print ( joySticks [ index ]->pinX );
		Serial.print ( F ( " and Y on pin " ) );
		Serial.println ( joySticks [ index ]->pinY );

		Serial.print ( F ( "Press any button to begin" ) );
		timerWait ( TIMEOUT_VALUE );

		Serial.println ( F ( "Move joyStick to its min and max points.\nPressany button to stop.\n" ) );

		buttonReset ();
		joyStickValues jsValues;
		jsValues.currentX = JOYSTICK_DEFAULT_MIN;
		jsValues.currentY = JOYSTICK_DEFAULT_MIN;
		jsValues.previousX = JOYSTICK_DEFAULT_MIN;
		jsValues.previousY = JOYSTICK_DEFAULT_MIN;

		while (!buttonPressed ( index )) {
			jsValues = readJoyStick ( index );

			//	Filter out noise
			if (filterValue ( index, jsValues )) {
				jsValues.currentX = jsValues.previousX;
				jsValues.currentY = jsValues.previousY;
				continue;
			}

			Serial.print ( F ( "X=" ) );
			Serial.print ( jsValues.currentX );

			// Set MAX values
			if (jsValues.currentX > joySticks [ index ]->maxX) {
				joySticks [ index ]->maxX = jsValues.currentX;
				Serial.print ( F ( " MaxVal X set to " ) );
				Serial.print ( jsValues.currentX );
			}

			//	Set MIN values
			if (jsValues.currentX < joySticks [ index ]->minX) {
				joySticks [ index ]->minX = jsValues.currentX;
				Serial.print ( F ( " MinVal X set to " ) );
				Serial.print ( jsValues.currentX );
			}

			Serial.print ( F ( ", Y=" ) );

			Serial.print ( jsValues.currentY );
			if (jsValues.currentY > joySticks [ index ]->maxY) {
				joySticks [ index ]->maxY = jsValues.currentY;
				Serial.print ( F ( " MaxVal Y set to " ) );
				Serial.print ( jsValues.currentY );
			}

			if (jsValues.currentY < joySticks [ index ]->minY) {
				joySticks [ index ]->minY = jsValues.currentY;
				Serial.print ( F ( " MinVal Y set to " ) );
				Serial.print ( jsValues.currentY );
			}

			Serial.println ();
			delay ( 100 );
		}

		buttonReset ( index );
		return true;
	}

	bool getAtRestValue ( uint8_t index ) {
		Serial.print ( F ( "\nCALIBRATING MID value for joystick #" ) );
		Serial.println ( index );
		uint32_t startTime = millis ();

		Serial.print ( F ( "Do not touch Joystick #" ) );
		Serial.println ( index );
		joyStickValues jsValues;
		uint32_t xMid = 0;
		uint32_t yMid = 0;
		int xCount = 0;
		int yCount = 0;

		while (( millis () - startTime < ( uint32_t ) TIMEOUT_VALUE )) {
			jsValues = readJoyStick ( index );

			//	Set X Mid value
			xMid += jsValues.currentX;
			++xCount;

			//	Set Y Mid value
			yMid += jsValues.currentY;
			++yCount;
		}

		Serial.print ( F ( "xMid=" ) ); Serial.print ( xMid ); Serial.print ( F ( "xCount=" ) ); Serial.println ( xCount );
		Serial.print ( F ( "yMid=" ) ); Serial.print ( yMid ); Serial.print ( F ( "yCount=" ) ); Serial.println ( yCount );

		//	Calculate average midpoint values
		joySticks [ index ]->midX = xMid / xCount;
		joySticks [ index ]->midY = yMid / yCount;
		Serial.print ( F ( "Joystick #" ) );
		Serial.print ( index );
		Serial.print ( F ( " Mid X set to " ) );
		Serial.print ( joySticks [ index ]->midX );
		Serial.print ( F ( " Mid Y set to " ) );
		Serial.println ( joySticks [ index ]->midY );
		return true;
	}

	void assignMotors ( uint8_t index ) {
		for (uint8_t i = 0; i < JOYSTICK_COUNT; i++) {
			if (joySticks [ i ]->motor1 == 0) {
				joySticks [ i ]->motor1 = nextMotorNumber++;
			}
			if (joySticks [ i ]->motor2 == 0) {
				joySticks [ i ]->motor2 = nextMotorNumber++;
			}
		}
	}

	bool filterValue ( uint8_t index, joyStickValues jsValues ) {
		//	Filter out noise
		return ( ( abs ( jsValues.currentX - jsValues.previousX ) < JOYSTICK_NOISE_FILTER )
				 && ( abs ( jsValues.currentY - jsValues.previousY ) < JOYSTICK_NOISE_FILTER ) );
	}

	void showResults () {
		Serial.println ( F ( "\nRESULTS: Add these lines to your Config.h file" ) );

		for (uint8_t i = 0; i < JOYSTICK_COUNT; i++) {
			Serial.println ();
			Serial.print ( F ( "#define JOYSTICK" ) );
			Serial.print ( i+1 );
			Serial.println ( F ( "_CALIBRATED true" ) );

			Serial.print ( F ( "#define JOYSTICK" ) );
			Serial.print ( i+1 );
			Serial.print ( F ( "X_PIN " ) );
			Serial.println ( joySticks [ i ]->pinX );

			Serial.print ( F ( "#define JOYSTICK" ) );
			Serial.print ( i + 1 );
			Serial.print ( F ( "Y_PIN " ) );
			Serial.println ( joySticks [ i ]->pinY );

			Serial.print ( F ( "#define JOYSTICK" ) );
			Serial.print ( i + 1 );
			Serial.print ( F ( "_BUTTON " ) );
			Serial.println ( joySticks [ i ]->pinButton );

			Serial.print ( F ( "#define JOYSTICK" ) );
			Serial.print ( i + 1 );
			Serial.print ( F ( "_MIN_X " ) );
			Serial.println ( joySticks [ i ]->minX );

			Serial.print ( F ( "#define JOYSTICK" ) );
			Serial.print ( i + 1 );
			Serial.print ( F ( "_MAX_X " ) );
			Serial.println ( joySticks [ i ]->maxX );

			Serial.print ( F ( "#define JOYSTICK" ) );
			Serial.print ( i + 1 );
			Serial.print ( F ( "_MID_X " ) );
			Serial.println ( joySticks [ i ]->midX );

			Serial.print ( F ( "#define JOYSTICK" ) );
			Serial.print ( i + 1 );
			Serial.print ( F ( "_MIN_Y " ) );
			Serial.println ( joySticks [ i ]->minY );

			Serial.print ( F ( "#define JOYSTICK" ) );
			Serial.print ( i + 1 );
			Serial.print ( F ( "_MAX_Y " ) );
			Serial.println ( joySticks [ i ]->maxY );

			Serial.print ( F ( "#define JOYSTICK" ) );
			Serial.print ( i + 1 );
			Serial.print ( F ( "_MID_Y " ) );
			Serial.println ( joySticks [ i ]->midY );

			Serial.print ( F ( "#define JOYSTICK" ) );
			Serial.print ( i + 1 );
			Serial.print ( F ( "_SERVO_MOTOR_1 " ) );
			Serial.println ( joySticks [ i ]->motor1 );

			Serial.print ( F ( "#define JOYSTICK" ) );
			Serial.print ( i + 1 );
			Serial.print ( F ( "_SERVO_MOTOR_2 " ) );
			Serial.println ( joySticks [ i ]->motor2 );
		}
	}

	void ShowCurrentValues ( uint8_t index ) {
		Serial.print ( joySticks [ index ]->minX );
		Serial.print ( '/' );
		Serial.print ( joySticks [ index ]->maxX );
		Serial.print ( '/' );
		Serial.print ( joySticks [ index ]->midX );

		Serial.print ( F ( "Initial Y Min/Max/Mid values are: " ) );
		Serial.print ( joySticks [ index ]->minY );
		Serial.print ( '/' );
		Serial.print ( joySticks [ index ]->maxY );
		Serial.print ( '/' );
		Serial.println ( joySticks [ index ]->midY );
	}

	void showJoystickValues ( uint8_t index ) {
		Serial.print ( F ( "Joystick #" ) );
		Serial.print ( index );
		Serial.print ( F ( " Min X =" ) );
		Serial.print ( joySticks [ index ]->minX );
		Serial.print ( F ( ", Max X =" ) );
		Serial.print ( joySticks [ index ]->maxX );
		Serial.print ( F ( ", Mid X =" ) );
		Serial.print ( joySticks [ index ]->midX );
		Serial.print ( F ( ", Min Y =" ) );
		Serial.print ( joySticks [ index ]->minY );
		Serial.print ( F ( ", Max Y=" ) );
		Serial.print ( joySticks [ index ]->maxY );
		Serial.print ( F ( ", Mid Y =" ) );
		Serial.println ( joySticks [ index ]->midY );
	}

	void buttonReset ( bool paused ) {
		_paused = paused;
	}

	void buttonReset () {
		_paused = false;
	}

	void timerWait ( uint32_t timeout ) {
		uint32_t startTime = millis ();
		bool pressed = false;
		while (!pressed && millis () - startTime > timeout) {
			for (uint8_t i; i < JOYSTICK_COUNT; i++) {
				if (buttonPressed ( i )) {
					pressed = true;
					return;
				}
			}
		}
	}

	bool buttonPressed ( uint8_t index ) {
		bool val = digitalRead ( joySticks [ index ]->pinButton );
		if (val == LOW) { // Button has been pressed
			if (val != joySticks [ index ]->buttonPrev) { // But has value changed?
				joySticks [ index ]->buttonPrev = val;
				Serial.print ( F ( "Button #" ) );
				Serial.print ( index );
				Serial.println ( F ( " Pressed" ) );
				if (_paused) {
					_paused = false;
					Serial.println ( F ( "Continuing..." ) );
					return true;
				}
				return true;
			}
		} else {
			joySticks [ index ]->buttonPrev = HIGH;
		}

		return false;
	}

	void showJoyStickPin ( uint8_t index ) {
		Serial.print ( F ( "Joystick #" ) );
		Serial.print ( index );
		Serial.print ( F ( " Button on pin " ) );
		Serial.println ( joySticks [ index ]->pinButton );
	}

	//	Return TRUE if the value is in the stop zone
	//	Stop zone is defined as the MID value +/- JOYSTICK_STOP_ZONE_SIZE 
	bool inStopZone ( uint16_t value, uint16_t midPoint ) {
		return ( ( midPoint - JOYSTICK_STOP_ZONE_SIZE < value )
				 && ( midPoint + JOYSTICK_STOP_ZONE_SIZE > value ) );
	}

	joyStickValues readJoyStick ( uint8_t index ) {
		uint16_t pinX = joySticks [ index ]->pinX;
		uint16_t pinY = joySticks [ index ]->pinY;
		uint16_t midX = joySticks [ index ]->midX;
		uint16_t midY = joySticks [ index ]->midY;

		uint16_t x = analogRead ( pinX );
		uint16_t y = analogRead ( pinY );

		//uint16_t deltaX = abs ( x - previousValues.currentX );
		//uint16_t deltaY = abs ( y - previousValues.currentX );

		joyStickValues newValues;
		newValues.currentX = x;
		newValues.currentY = y;
		//newValues.previousX = previousValues.currentX;
		//newValues.previousY = previousValues.currentY;

		//newValues.duplicateX = ( deltaX < JOYSTICK_NOISE_FILTER );
		//newValues.duplicateY = ( deltaY < JOYSTICK_NOISE_FILTER );

		newValues.inStopZoneX = inStopZone ( x, midX );
		newValues.inStopZoneY = inStopZone ( x, midX );

		if (_tracing) {
			showValue ( index, 'X', x, newValues.duplicateX );
			showValue ( index, 'Y', y, newValues.duplicateY );

		}

		return newValues;
	}

	void showValue ( uint8_t index, char axis, uint16_t value, bool duplicate ) {
		if (!duplicate) {
			Serial.print ( F ( "Joystick#" ) );
			Serial.print ( index );
			Serial.print ( F ( " Axis:" ) );
			Serial.print ( axis );
			Serial.print ( F ( " Val=" ) );
			Serial.println ( value );
		}
	}

	void showJoystickDefinition ( uint8_t index ) {
		Serial.print ( F ( "\nJoystick Index: " ) );
		Serial.println ( index );
		Serial.print ( F ( "pinX: " ) );
		Serial.print ( joySticks [ index ]->pinX );
		Serial.print ( F ( ", pinY: " ) );
		Serial.print ( joySticks [ index ]->pinY );
		Serial.print ( F ( ", pinButton: " ) );
		Serial.println ( joySticks [ index ]->pinButton );
		Serial.print ( F ( "minX: " ) );
		Serial.print ( joySticks [ index ]->minX );
		Serial.print ( F ( ", maxX: " ) );
		Serial.print ( joySticks [ index ]->maxX );
		Serial.print ( F ( ", midX: " ) );
		Serial.println ( joySticks [ index ]->midX );
		Serial.print ( F ( "minY: " ) );
		Serial.print ( joySticks [ index ]->minY );
		Serial.print ( F ( ", maxY: " ) );
		Serial.print ( joySticks [ index ]->maxY );
		Serial.print ( F ( ", midY: " ) );
		Serial.println ( joySticks [ index ]->midY );
		Serial.print ( F ( "motor1: " ) );
		Serial.print ( joySticks [ index ]->motor1 );
		Serial.print ( F ( ", motor2: " ) );
		Serial.println ( joySticks [ index ]->motor2 );
	}
};