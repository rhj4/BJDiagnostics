/***********************************************************
*
*	HC05 Tester
*
************************************************************/
#define POWER_LED		2
#define STATE_LED	3
#define STATE_PIN	4
#define POWER_PIN	5
#define FLASH_DELAY	50
#define FLASH_TIMES	10
#define BAUD_RATE	38400

bool state = false;
bool power = false;

void setup () {
	Serial.begin ( BAUD_RATE );
	pinMode ( POWER_LED, OUTPUT );
	pinMode ( STATE_LED, OUTPUT );
	flash ( POWER_LED );
	flash ( STATE_LED );

	pinMode ( STATE_PIN, INPUT );
	pinMode ( POWER_PIN, INPUT );
	Serial.println ( F ( "HC05 Tester" ) );
}

void loop () {
	checkForResponse ();
	checkPower ();
	checkState ();
	checkEnable ();
}

void checkForResponse () {
	if (Serial.available()) {
		Serial.print ( '>' );
		while (Serial.available()) {
			char data = Serial.read ();
			Serial.print ( data );
		}
	}
}

void checkPower () {
	bool currentPower = digitalRead ( POWER_PIN );
	if (currentPower != power) {
		power = currentPower;
		if (power == HIGH) {
			turnOn ( POWER_LED );
			delay ( FLASH_DELAY );
		} else {
			turnOff ( POWER_LED );
		}
		Serial.print ( F ( "POWER: " ) );
		Serial.println ( ( power ) ? "ON" : "OFF" );
	}
}

void checkState () {
	bool currentState = digitalRead ( 4 );
	if (currentState != state) {
		state = currentState;
		if (state) {
			turnOn ( STATE_LED );
			delay ( FLASH_DELAY );
		} else {
			turnOff ( STATE_LED );
		}
		Serial.print ( F ( "STATE: " ) );
		Serial.println ( ( state ) ? "ON" : "OFF" );
	}
}

void checkEnable () {

}

void flash ( int pin ) {
	for (int i = 0; i < FLASH_TIMES; i++) {
		turnOn ( pin );
		delay ( FLASH_DELAY );
		turnOff ( pin );
		delay ( FLASH_DELAY );
	}
}

void turnOn ( int pin ) {
	digitalWrite ( pin, HIGH );
}

void turnOff ( int pin ) {
	digitalWrite ( pin, LOW );
}

