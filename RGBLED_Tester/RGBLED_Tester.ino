/*
	Name:       RGBLED_Tester.ino
	Created:	10/15/2018 6:20:59 PM
	Author:     THOR\bob
*/

#define DIST_FAR		A0
#define DIST_NEAR		A3
#define DIST_CLOSE		A2
#define WIFI_PIN		A1
#define POWER_PIN		A7
#define DIR_FORWARD		A6
#define DIR_BACKWARD	A5
#define DIR_STOPPED		A4

bool waitForIt = false;
bool tracePins = false;

void setup () {
	Serial.begin ( 115200 );
	pinMode ( DIST_CLOSE, OUTPUT );
	pinMode ( DIST_NEAR, OUTPUT );
	pinMode ( DIST_FAR, OUTPUT );
	pinMode ( POWER_PIN, OUTPUT );
	pinMode ( WIFI_PIN, OUTPUT );
	pinMode ( DIR_FORWARD, OUTPUT );
	pinMode ( DIR_BACKWARD, OUTPUT );
	pinMode ( DIR_STOPPED, OUTPUT );

	//while (!Serial.available ()) {
	//	flashPin ( DIST_CLOSE, 10, 50 );
	//	flashPin ( DIST_NEAR, 10, 50 );
	//	flashPin ( DIST_FAR, 10, 50 );
	//	flashPin ( POWER_PIN, 10, 50 );
	//	flashPin ( WIFI_PIN, 10, 50 );
	//	flashPin ( DIR_FORWARD, 10, 50 );
	//	flashPin ( DIR_BACKWARD, 10, 50 );
	//	flashPin ( DIR_STOPPED, 10, 50 );
	//}

	showMenu ();
}
void showMenu () {
	Serial.println ( F ( "c = Close (RED)" ) );
	Serial.println ( F ( "n = Near (BLUE)" ) );
	Serial.println ( F ( "f = Far (GREEN)" ) );
	Serial.println ( F ( "> = FORWARD (GREEN)" ) );
	Serial.println ( F ( "< = Backward (RED)" ) );
	Serial.println ( F ( "s = Stopped (BLUE)" ) );
	Serial.println ( F ( "p = Toggle Power Pin (RED)" ) );
	Serial.println ( F ( "w = Toggle WiFi Pin (BLUE)" ) );
	Serial.println ( F ( "t = Toggle Pin Tracing" ) );
	Serial.println ( F ( "l = Light Show" ) );
	Serial.println ( F ( "+ = All On" ) );
	Serial.println ( F ( "- = All off" ) );
	Serial.println ( F ( "a = Flash All Pins and hold" ) );
	Serial.println ( F ( "x = toggle Wait state" ) );
	Serial.println ( F ( "0..8 = toggle A<n>" ) );
	Serial.println ( F ( "? = Show this menu" ) );
}

void turnOn ( int pin ) {
	if (tracePins) {
		Serial.print ( '+' );
		Serial.println ( pin );
	}
	digitalWrite ( pin, HIGH );
}

void turnOff ( int pin ) {
	if (tracePins) {
		Serial.print ( '-' );
		Serial.println ( pin );
	}
	digitalWrite ( pin, LOW );
}

void loop () {
	if (Serial.available ()) {
		char cmd = Serial.read ();
		switch (cmd) {
		case 0x10:
		case 0x13:
			return;

		case 't':
			tracePins = !tracePins;
			Serial.print ( F ( "Trace Pins " ) );
			Serial.println ( ( tracePins )?"ENABLED":"DISABLED" );
			break;

		case 'c':
			allOff ();
			Serial.println ( F ( "CLOSE" ) );
			waitForIt = true;
			return;

		case 'n':
			allOff ();
			flashPin ( DIST_NEAR, 10, 50 );
			turnOn ( DIST_NEAR );
			Serial.println ( F ( "NEAR" ) );
			waitForIt = true;
			return;

		case 'l':
			lightShow ();
			allOff ();
			break;
		case 'f':
			allOff ();
			flashPin ( DIST_NEAR, 10, 50 );
			turnOn ( DIST_NEAR );
			Serial.println ( F ( "FAR" ) );
			waitForIt = true;
			return;

		case '>':
			allOff ();
			flashPin ( DIR_FORWARD, 10, 50 );
			turnOn ( DIR_FORWARD );
			Serial.println ( F ( "FORWARD" ) );
			waitForIt = true;
			return;

		case '<':
			allOff ();
			flashPin ( DIR_BACKWARD, 10, 50 );
			turnOn ( DIR_BACKWARD );
			Serial.println ( F ( "BACKWARD" ) );
			waitForIt = true;
			return;

		case 's':
			allOff ();
			flashPin ( DIR_STOPPED, 10, 50 );
			turnOn ( DIR_STOPPED );
			Serial.println ( F ( "STOPPED" ) );
			waitForIt = true;
			return;

		case 'p':
			allOff ();
			flashPin ( POWER_PIN, 10, 50 );
			turnOn ( POWER_PIN );
			Serial.println ( F ( "POWER" ) );
			waitForIt = true;
			return;

		case 'w':
			allOff ();
			flashPin ( WIFI_PIN, 10, 50 );
			turnOn ( WIFI_PIN );
			Serial.println ( F ( "WIFI" ) );
			waitForIt = true;
			return;

		case 'a':
			Serial.println ( F ( "CLOSE" ) );
			flashPin ( DIST_CLOSE, 10, 50 );
			delay ( 1000 );

			Serial.println ( F ( "NEAR" ) );
			flashPin ( DIST_NEAR, 10, 50 );
			delay ( 1000 );

			Serial.println ( F ( "FAR" ) );
			flashPin ( DIST_NEAR, 10, 50 );
			delay ( 1000 );

			Serial.println ( F ( "FORWARD" ) );
			flashPin ( DIR_FORWARD, 10, 50 );
			delay ( 1000 );

			Serial.println ( F ( "BACKWARD" ) );
			flashPin ( DIR_BACKWARD, 10, 50 );
			delay ( 1000 );

			Serial.println ( F ( "STOPPED" ) );
			flashPin ( DIR_STOPPED, 10, 50 );
			delay ( 1000 );

			Serial.println ( F ( "POWER" ) );
			flashPin ( POWER_PIN, 10, 50 );
			delay ( 1000 );

			Serial.println ( F ( "WIFI" ) );
			flashPin ( WIFI_PIN, 10, 50 );
			delay ( 1000 );

			waitForIt = true;
			return;

		case '-':
			allOff ();
			waitForIt = true;
			return;

		case '+':
			allOn ();
			waitForIt = true;
			return;

		case 'x':
			waitForIt = !waitForIt;
			return;

		case '?':
			showMenu ();
			return;

		default:
			if (cmd >= '0' && cmd <= '7') {
				int pin = (int) cmd - 0x30 + 14;
				allOff ();
				flashPin ( pin, 10, 50 );
				turnOn ( pin );
				return;
			}
			Serial.print ( "??? [" );
			Serial.print ( (int)cmd, HEX );
			Serial.println ( F ( "]" ) );
			break;
		}
	}
	
	if (waitForIt) {
		return;
	}

	//turnOn ( DIST_CLOSE );
	//delay ( 500 );
	//turnOff ( DIST_CLOSE );
	//turnOn ( DIST_FAR );
	//delay ( 500 );
	//turnOff ( DIST_FAR );
	//turnOn ( DIST_NEAR );
	//delay ( 500 );
	//turnOff ( DIST_NEAR );
	//delay ( 500 );
	//turnOn ( DIR_BACKWARD );
	//delay ( 500 );
	//turnOff ( DIR_BACKWARD );
	//delay ( 500 );
	//turnOn ( DIR_STOPPED );
	//delay ( 500 );
	//turnOff ( DIR_STOPPED );
	//delay ( 500 );
	//turnOn ( WIFI_PIN );
	//delay ( 500 );
	//turnOff ( WIFI_PIN );
	//delay ( 500 );
	//turnOn ( POWER_PIN );
	//delay ( 500 );
	//turnOff ( POWER_PIN );
	delay ( 1000 );
}

void allOff () {
	turnOff ( DIST_CLOSE );
	turnOff ( DIST_FAR );
	turnOff ( DIST_NEAR );
	turnOff ( DIR_FORWARD );
	turnOff ( DIR_BACKWARD );
	turnOff ( DIR_STOPPED );
	turnOff ( POWER_PIN);
	turnOff ( WIFI_PIN );
	Serial.println ( F ( "ALL OFF" ) );
}

void allOn () {
	Serial.println ( F ( "ALL ON" ) );
	flashPin ( DIST_NEAR, 10, 50 );
	turnOn ( DIST_NEAR );

	flashPin ( DIST_NEAR, 10, 50 );
	turnOn ( DIST_NEAR );

	flashPin ( DIST_NEAR, 10, 50 );
	turnOn ( DIST_NEAR );

	flashPin ( DIR_FORWARD, 10, 50 );
	turnOn ( DIR_FORWARD );

	flashPin ( DIR_BACKWARD, 10, 50 );
	turnOn ( DIR_BACKWARD );

	flashPin ( DIR_STOPPED, 10, 50 );
	turnOn ( DIR_STOPPED );

	flashPin ( POWER_PIN, 10, 50 );
	turnOn ( POWER_PIN );

	flashPin ( WIFI_PIN, 10, 50 );
	turnOn ( WIFI_PIN );
}

static void flashPin ( int pin, int count, int duration ) {
	Serial.print ( F ( "Flashing pin " ) );
	Serial.print ( pin );
	Serial.print ( F ( " for " ) );
	Serial.print ( duration );
	Serial.print ( F ( "ms " ) );
	Serial.print ( count );
	Serial.println ( F ( " times" ) );

	for (int i = 0; i < count; i++) {
		turnOn ( pin );
		delay ( duration );
		turnOff ( pin );
		delay ( duration );
	}
}

void lightShow () {
	while (!Serial.available ()) {
		for (int i = 0; i < 8; i++) {
			flashPin ( i+14, 5, 50 );
		}
	}
}