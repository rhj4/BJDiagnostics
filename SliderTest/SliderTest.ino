/*******************************************************************
*
*	Name:       SliderTest.ino
*   Created:	9/6/2018 9:42:07 PM
*   Author:     Bob Jones <bob@rhj4.com>
*
*	This shetch tests pairs of sliders to determine their minimum
*	and maximum values and the midpoint of the two.
*
*/

#include "Config.h"

class slider {
private:
	int _pin;
	bool _invert;

	char _name [6];

	int testFor = DEFAULT_MAX;

	int lastValue = 0;
	int currentValue = 0;

	int max = 0;
	int min = testFor;

public:
	//	Constructor
	slider ( int pin, bool invert, char* name ) {
		_pin = pin;
		_invert = invert;
		strcpy ( _name, name );

		pinMode ( _pin, INPUT );

		Serial.print ( _name );
		Serial.print ( F ( " Slider on pin " ) );
		Serial.print ( _pin );
		if (_invert) Serial.print ( F ( " INVERTED" ) );
		Serial.println ();
	}

	char* getName () {
		return _name;
	}

	bool calibrate () {
		Serial.print ( F ( "CALIBRATING " ) );
		Serial.print ( _name );
		Serial.println ( F ( " SLIDER" ) );

		getMin ();
		getMax ();
		showMinMax ();
		return true;
	}

	int read (bool showMinMax) {
		currentValue = (int) analogRead ( _pin);
#if AUTO_RESET_MAX
		checkForRangeOverflow ();
#endif

		if (_invert) {
			currentValue = abs ( max - currentValue );
		}

		if (abs(currentValue - lastValue) > SLIDER_SLOP) {
			Serial.print ( _name );
			if (showMinMax) {
				Serial.print ( F ( " Min: " ) );
				Serial.print ( min );
				Serial.print ( F ( ", Max: " ) );
				Serial.print ( max );
				Serial.print ( F ( ", Current: " ) );
			}
			Serial.print ( ' ' );
			Serial.println ( currentValue );
			lastValue = currentValue;
		}

		delay ( READ_LOOP_DELAY );
		return currentValue;
	}


	void getMin () {
		Serial.print ( F ( "\nPut " ) );
		Serial.print ( _name );
		Serial.println(F(" slider in its MIN position" ) );
		int value;
		do {
			value = analogRead ( _pin );
			Serial.println ( value );
			delay ( 250 );
		} while (value > MIN_SLIDER_VALUE);

		do {
			read (true);
			if (lastValue != currentValue) {
				lastValue = currentValue;
				Serial.print ( _name );
				Serial.print ( F ( " Min: " ) );
				Serial.print ( min );
				Serial.print ( F ( ", Current:" ) );
				Serial.println ( currentValue );
				if (currentValue < min) {
					min = currentValue;
				}
			}
			delay ( 100 );
		} while (min > 0);
	}

	void getMax () {
		int loops = 10;
		lastValue = testFor;

		Serial.print ( F ( "\nPut " ) );
		Serial.print ( _name );
		Serial.println(F(" slider in its MAX position" ) );
		int value;
		do {
			value = analogRead ( _pin );
			Serial.println ( value );
			delay ( 250 );
		} while (value < MAX_SLIDER_VALUE);

		do {
			read (true);
			if (lastValue != currentValue) {
				lastValue = currentValue;
				Serial.print ( _name );
				Serial.print ( F ( " Max: " ) );
				Serial.print ( max );
				Serial.print ( F ( ", Test For:" ) );
				Serial.print ( testFor );
				Serial.print ( F ( ", Current:" ) );
				Serial.println ( currentValue );
				if (currentValue > max) {
					max = currentValue;
				}
			}

			if (--loops <= 0) {
				loops = 10;
				testFor -= 100;
			}
			delay ( 100 );
		} while (max < testFor);
	}

	void showMinMax (bool showMid = false) {
		Serial.print ( _name );
		Serial.print ( F ( " Min = " ) );
		Serial.print ( min );
		Serial.print ( F ( ", Max = " ) );
		Serial.print ( max );
		if (showMid) {
			Serial.print ( F ( ", Mid = " ) );
			Serial.print ( ( max - min ) / 2 );
		}
		Serial.println ();
	}

#if AUTO_RESET_MAX
	void checkForRangeOverflow () {
		if (currentValue > max) {
			max = currentValue;
			Serial.print ( F ( "RESETTING " ) );
			Serial.print ( _name );
			Serial.print(F(" MAX TO " ) );
			Serial.println ( max );
		}
		if (currentValue < min) {
			min = currentValue;
			Serial.print ( F ( "RESETTING " ) );
			Serial.print ( _name );
			Serial.print ( F ( " MIN TO " ) );
			Serial.println ( min );
		}
	}
#endif
};


#if TEST_LEFT_SLIDER
slider leftSlider ( LEFT_SLIDER_PIN, LEFT_SLIDER_INVERTED, ( char* ) "LEFT" );
#endif 

#if TEST_RIGHT_SLIDER
slider rightSlider ( RIGHT_SLIDER_PIN, RIGHT_SLIDER_INVERTED, ( char* ) "RIGHT" );
#endif

void setup()
{
	Serial.begin ( 115200 );
	while (!Serial.availableForWrite ()) {}

	pinMode ( LED_PIN, OUTPUT );

#if CALIBRATE_SLIDERS
#if TEST_LEFT_SLIDER
	if ( !leftSlider.calibrate ()) {
		Serial.println ( "CALIBRATION FAILURE" );
		exit ( -1 );
	}
	Serial.print ( leftSlider.getName () );
	Serial.println ( F ( " Slider Calibrated" )) ;
#endif

#if TEST_RIGHT_SLIDER
	if (!rightSlider.calibrate ()) {
		Serial.println ( "CALIBRATION FAILURE" );
		exit ( -1 );
	}
	Serial.print ( rightSlider.getName () );
	Serial.println ( F ( " Slider Calibrated" ) );
#endif
#endif /* CALIBRATE_SLIDERS */

	Serial.println ( F ( "\nStarting Loop" ) );
}

void loop () {
#if TEST_LEFT_SLIDER
	leftSlider.read (false);
#endif

#if TEST_RIGHT_SLIDER
	rightSlider.read (false);
#endif
	if (Serial.available ()) {
#if TEST_LEFT_SLIDER
		leftSlider.showMinMax( true );
#endif

#if TEST_RIGHT_SLIDER
		rightSlider.showMinMax( true);
#endif

		while (Serial.available ()) {
			char byte = Serial.read ();
		}
	}
	delay ( MAIN_LOOP_DELAY );
}
