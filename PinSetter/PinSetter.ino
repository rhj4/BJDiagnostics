/*
	Name:       PinSetter.ino
	Created:	11/20/2018 8:30:02 PM
	Author:     THOR\bob
	Desc:		This utility program sets and reads digital pin values
*/
#include <ArduinoBoardTypes.h>

int activePin = 0;

static const int maxPin = ( strcmp ( BOARD, "Mega" ) == 0 )?54:22;

char pins [100] = {0};

void setup () {
	Serial.begin ( 115200 );
	for (int i = 0; i < 100; i++) {
		pins [i] = '?';
	}
	showPins ();

	menu ();
	Serial.println ( F ( "READY" ) );
}

void loop () {
	getCommand ();
	delay ( 500 );
}

void getCommand () {
	while (Serial.available ()) {

		char cmd = Serial.read ();
		switch (toLowerCase ( cmd )) {
		case 0x10:
		case 0x13:
		case 0xD:
		case 0xA:
		case 0xFF:
			break;

		case 'p':
			activePin = Serial.parseInt ();
			if (activePin > 0 && activePin <= maxPin) {
				if (pins [activePin] == '?') {
					pinMode ( activePin, OUTPUT );
				}
				pins [activePin] = 'a';
				Serial.print ( F ( "Active pin is " ) );
				Serial.println ( activePin );
			} else {
				showPins ();
				return;
			}
			break;

		case '+':
			digitalWrite ( activePin, HIGH );
			pins [activePin] = '+';
			showPin ( activePin );
			break;

		case '-':
			digitalWrite ( activePin, LOW );
			pins [activePin] = '-';
			showPin ( activePin );
			break;

		case '=':
			showPins ();
			break;

		case '?':
			menu ();
			return;

		default:
			Serial.print ( F ( "Invalid command [" ) );
			Serial.print ( cmd );
			Serial.print ( F ( "=" ) );
			Serial.print ( cmd, HEX );
			Serial.println ( F ( "]" ) );
		}
	}
}

void showPin ( int pin ) {
	Serial.print ( F ( "Pin " ) );
	Serial.print ( pin );
	Serial.print ( F ( " is " ) );
	Serial.println ( ( digitalRead ( pin ) )?"ON":"OFF" );
}

void menu () {
	Serial.println ( F ( "Commands are:" ) );
	Serial.println ( F ( "p<pin> - set <pin> as the active pin and show its value" ) );
	Serial.println ( F ( "+\tSet current pin to high" ) );
	Serial.println ( F ( "-\tSet current pin to Low" ) );
	Serial.println ( F ( "Commands may be strung together as in:" ) );
	Serial.println ( F ( "\tP5+P10-P9+" ) );
}

void showPins () {
	for (int i = 0; i < 100; i++) {
		if (pins [i] == '?') continue;

		Serial.print ( "PIN " );
		Serial.print ( i );
		Serial.print ( ' ' );

		if (pins [i] == 'a') {
			Serial.println ("ACTIVE");
		} else if (pins [i] == '+') {
			Serial.print ( "HIGH " );
			if (!digitalRead ( i )) {
				Serial.print ( "*** VALUE IS LOW ***" );
			}
		} else if (pins [i] == '-') {
			Serial.print ( "LOW " );
			if (digitalRead ( i )) {
				Serial.print ( "*** VALUE IS HIGH ***" );
			}
		} else {
			Serial.print ( pins [i] );
		}

		Serial.println ();
	}
}
