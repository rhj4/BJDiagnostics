/*******************************************************************
*	Name:       JoystickTest.ino
*   Created:	9/8/2018 11:24:14 AM
*   Author:     Bob Jones <bob@rhj4.com>
*
*	This diagnostic program tests reading a joystick
*	X and Y values and the joystick's button.
*
********************************************************************/
#include "Config.h"

int x;
int y;
int prevX, prevY;
int buttonState = 0;
int prevButtonState = 0;
bool paused = false;

int buttonJS = JOYSTICK_BUTTON_PIN;

#if JOYSTICK_SHIELD
int buttonA = JOYSTICK_BUTTON_A_PIN;
int buttonB = JOYSTICK_BUTTON_B_PIN;
int buttonC = JOYSTICK_BUTTON_C_PIN;
int buttonD = JOYSTICK_BUTTON_D_PIN;
int buttonE = JOYSTICK_BUTTON_E_PIN;
int buttonF = JOYSTICK_BUTTON_F_PIN;
#endif /* JOYSTICK_SHIELD */

bool invertX = JOYSTICK_INVERT_X;
bool invertY = JOYSTICK_INVERT_Y;
bool swapXY = JOYSTICK_SWAP_X_Y;

int minX = JOYSTICK_MAX_X + 1;
int maxX = JOYSTICK_MIN_X - 1;
int minY = JOYSTICK_MAX_Y + 1;
int maxY = JOYSTICK_MIN_Y - 1;
int midX = 0;
int midY = 0;

unsigned long reading = 0;

void setup () {

	Serial.begin ( 115200 );

	//	Stall while Serial eats his cherios 
	while (!Serial.availableForWrite ()) {}

	pinMode ( JOYSTICK_X_PIN, INPUT );
	pinMode ( JOYSTICK_Y_PIN, INPUT );
	pinMode ( JOYSTICK_BUTTON_PIN, INPUT_PULLUP );

	pinMode ( JOYSTICK_BUTTON_PIN, INPUT_PULLUP );
	digitalWrite ( JOYSTICK_BUTTON_PIN, HIGH );

#if JOYSTICK_SHIELD
#if JOYSTICK_BUTTON_A_PIN > 0
	pinMode ( JOYSTICK_BUTTON_A_PIN, INPUT_PULLUP );
	digitalWrite ( JOYSTICK_BUTTON_A_PIN, HIGH );
#endif

#if JOYSTICK_BUTTON_B_PIN > 0
	pinMode ( JOYSTICK_BUTTON_B_PIN, INPUT_PULLUP );
	digitalWrite ( JOYSTICK_BUTTON_B_PIN, HIGH );
#endif

#if JOYSTICK_BUTTON_C_PIN > 0
	pinMode ( JOYSTICK_BUTTON_C_PIN, INPUT_PULLUP );
	digitalWrite ( JOYSTICK_BUTTON_C_PIN, HIGH );
#endif

#if JOYSTICK_BUTTON_D_PIN > 0
	pinMode ( JOYSTICK_BUTTON_D_PIN, INPUT_PULLUP );
	digitalWrite ( JOYSTICK_BUTTON_D_PIN, HIGH );
#endif

#if JOYSTICK_BUTTON_E_PIN > 0
	pinMode ( JOYSTICK_BUTTON_E_PIN, INPUT_PULLUP );
	digitalWrite ( JOYSTICK_BUTTON_E_PIN, HIGH );
#endif

#if JOYSTICK_BUTTON_F_PIN > 0
	pinMode ( JOYSTICK_BUTTON_F_PIN, INPUT_PULLUP );
	digitalWrite ( JOYSTICK_BUTTON_F_PIN, HIGH );
#endif
#endif /* JOYSTICK_SHIELD */

	readMidValues ();

	Serial.println ( F ( "\nJoystick Tester" ) );
	Serial.print ( F ( "Joystick X on pin " ) );
	Serial.println ( JOYSTICK_X_PIN );
	Serial.print ( F ( "Joystick Y on pin " ) );
	Serial.println ( JOYSTICK_Y_PIN );
	Serial.print ( F ( "Joystick Button on pin " ) );
	Serial.println ( JOYSTICK_BUTTON_PIN );

#if JOYSTICK_SWAP_X_Y
	Serial.println ( F ( "X and Y values will be swapped" ) );
#endif /* JOYSTICK_SWAP_X_Y */

#if JOYSTICK_INVERT_X
	Serial.println ( F ( "X value will be MAX(X) - X" ) );
#endif /* JOYSTICK_INVERT_X */

#if JOYSTICK_INVERT_Y
	Serial.println ( F ( "Y value will be MAX(Y) - Y" ) );
#endif /* JOYSTICK_INVERT_Y */

	Serial.println ( F ( "Press and hold joystick button to read values" ) );
}

void loop () {
	if (!readJoystick ()) {
		delay ( 100 );
		return;
	}

	readJoystickButton ();
	if (paused) {
		showMinMax ();
		while (paused) {
			delay ( 100 );
			readJoystickButton ();
		}
		paused = false;
	} else {
		showJoystickValues ();
	}

#if JOYSTICK_SHIELD
	readJoystickExtraButtons ();
#endif /* JOYSTICK_SHIELD */

	delay ( 100 );
}

void readMidValues () {
	unsigned long xValues = 0;
	unsigned long yValues = 0;
	for (int i = 0; i < JOYSTICK_CALIBRATION_LOOPS; i++) {
		readJoystick ();
		xValues += x;
		yValues += y;
	}

	midX = xValues / JOYSTICK_CALIBRATION_LOOPS;
	midY = yValues / JOYSTICK_CALIBRATION_LOOPS;
}

bool readJoystick () {
	if (swapXY) {
		y = analogRead ( JOYSTICK_X_PIN );
		x = analogRead ( JOYSTICK_Y_PIN );
	} else {
		x = analogRead ( JOYSTICK_X_PIN );
		y = analogRead ( JOYSTICK_Y_PIN );
	}

	x = ( invertX ) ? abs ( JOYSTICK_MAX_X - x ) : x;
	y = ( invertY ) ? abs ( JOYSTICK_MAX_Y - y ) : y;

	if (x < minX) minX = x;
	if (x > maxX) maxX = x;
	if (y < minY) minY = y;
	if (y > maxY) maxY = y;

	return ( ( abs ( x - prevX ) > JOYSTICK_SLOP )
			 || ( abs ( y - prevY ) > JOYSTICK_SLOP ) );
}

bool readJoystickButton () {
	buttonState = digitalRead ( JOYSTICK_BUTTON_PIN );
	if (buttonState != prevButtonState) {
		prevButtonState = buttonState;
		if (buttonState == LOW) {
			if (paused) {
				paused = false;
				return true;
			}
			paused = true;
			return false;
		}
	}

	return false;
}

#if JOYSTICK_SHIELD
void readJoystickExtraButtons () {
	if (buttonA > 0) readExtraButton ( buttonA );
	if (buttonB > 0) readExtraButton ( buttonB );
	if (buttonC > 0) readExtraButton ( buttonC );
	if (buttonD > 0) readExtraButton ( buttonD );
	if (buttonE > 0) readExtraButton ( buttonE );
	if (buttonF > 0) readExtraButton ( buttonF );
}

bool readExtraButton ( int pin ) {
	if (digitalRead ( pin ) == LOW) {
		Serial.print ( F ( "Button on pin " ) );
		Serial.print ( pin );
		Serial.println ( F ( " pressed" ) );
		while (digitalRead ( pin ) == LOW) {}
		return true;
	}
	return false;
}
#endif /* JOYSTICK_SHIELD */

void showJoystickValues () {
	Serial.print ( ++reading );
	Serial.print ( "\tX=" );
	Serial.print ( x );
	Serial.print ( ", Y=" );
	Serial.println ( y );
	if (buttonState == 0) {
		showMinMax ();
	}
}

void showMidValues () {
	Serial.print ( F ( " X-Mid = " ) );
	Serial.print ( midX );
	Serial.print ( F ( " Y-Mid = " ) );
	Serial.println ( midY );
}

void showMinMax () {
	Serial.print ( F ( "\nMin X=" ) ); Serial.print ( minX );
	Serial.print ( F ( " Max X=" ) ); Serial.print ( maxX );
	Serial.print ( F ( " Min Y=" ) ); Serial.print ( minY );
	Serial.print ( F ( " Max Y=" ) ); Serial.print ( maxY );
	showMidValues ();
}
