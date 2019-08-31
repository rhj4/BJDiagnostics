#include "config.h"
/*******************************************************************
*
*	Joystick Shield test program that tests Joystic and all buttons
*
********************************************************************/

int oldX = -1;
int oldY = -1;

bool buttonJSValue = false;
bool buttonAValue = false;
bool buttonBValue = false;
bool buttonCValue = false;
bool buttonDValue = false;
bool buttonEValue = false;
bool buttonFValue = false;

void setup () {
	Serial.begin ( 115200 );
	Serial.println ( F ( "Joystick Shield Tester" ) );

}

void loop () {
	readJoystick ();
	readButtons ();
}


void readJoystick () {
	int x = analogRead ( JOYSTICK_X_PIN );
	int y = analogRead ( JOYSTICK_Y_PIN );
	if (abs(x - oldX) < X_SLOP && abs(y - oldY) < Y_SLOP) return;

	oldX = x;
	oldY = y;

	Serial.print ( F ( "X=" ) );
	Serial.print ( x );
	Serial.print ( F ( ", Y=" ) );
	Serial.println ( y );
}

void readButtons () {
	buttonJSValue = getValue ( JOYSTICK_BTN_PIN, buttonJSValue );

	buttonAValue = getValue ( JOYSTICK_A_PIN, buttonAValue );
	buttonBValue = getValue ( JOYSTICK_B_PIN, buttonBValue );
	buttonCValue = getValue ( JOYSTICK_C_PIN, buttonCValue );
	buttonDValue = getValue ( JOYSTICK_D_PIN, buttonDValue );
	buttonEValue = getValue ( JOYSTICK_E_PIN, buttonEValue );
	buttonFValue = getValue ( JOYSTICK_F_PIN, buttonFValue );
}

bool getValue ( int pin, bool oldValue ) {
	bool value = readButton ( pin );
	if (value != oldValue) {
		reportPinValue ( pin, value );
	}
	return value;
}

void reportPinValue ( int pin, bool value ) {
	Serial.print ( F ( "Button on PIN " ) );
	Serial.print ( pin );
	Serial.print ( F ( " IS " ) );
	Serial.println ( ( value ) ? "UP" : "DOWN" );
}

bool readButton ( int button ) {
	return digitalRead ( button );
}
