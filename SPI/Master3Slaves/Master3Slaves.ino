#include <SPI.h>
#include "Config.h"

char buffer [100] = { 0 };
volatile byte pos;
volatile bool processIt;

void setup () {
	Serial.begin ( 115200 );

	pinMode ( SCK, OUTPUT );
	pinMode ( MOSI, OUTPUT );
	pinMode ( SS, OUTPUT );

	digitalWrite ( SCK, LOW );
	digitalWrite ( MOSI, LOW );
	digitalWrite ( SS, HIGH );

#if ROLE_IS_MASTER

	//	Configure pins that connect to slaves
	pinMode ( MASTER_CS_RED_PIN, OUTPUT );
	pinMode ( MASTER_CS_GREEN_PIN, OUTPUT );
	pinMode ( MASTER_CS_BLUE_PIN, OUTPUT );

	pinMode ( MASTER_RED_LED_PIN, OUTPUT );
	pinMode ( MASTER_GREEN_LED_PIN, OUTPUT );
	pinMode ( MASTER_BLUE_LED_PIN, OUTPUT );

	flashPin ( MASTER_RED_LED_PIN );
	flashPin ( MASTER_GREEN_LED_PIN );
	flashPin ( MASTER_BLUE_LED_PIN );

	delay ( 500 );

	digitalWrite ( MASTER_CS_RED_PIN, HIGH );
	digitalWrite ( MASTER_CS_GREEN_PIN, HIGH );
	digitalWrite ( MASTER_CS_BLUE_PIN, HIGH );

	SPI.begin ();

	Serial.println ( F ( "Enter something..." ) );

#else
	SPCR != bit ( SPE );
	pinMode ( MISO, OUTPUT );

#if ROLE_IS_SLAVE_RED
	pinMode ( SLAVE_RED_CS_PIN, OUTPUT );
	pinMode ( SLAVE_RED_LED_PIN, OUTPUT );

	flashPin ( SLAVE_RED_CS_PIN );
	flashPin ( SLAVE_RED_LED_PIN );

#elif ROLE_IS_SLAVE_GREEN

	pinMode ( SLAVE_GREEN_CS_PIN, OUPUT );
	pinMode ( SLAVE_GREEN_LED_PIN, OUPUT );
	flashPin ( SLAVE_GREEN_CS_PIN );
	flashPin ( SLAVE_GREEN_LED_PIN );

#elif ROLE_IS_SLAVE_BLUE

	pinMode ( SLAVE_BLUE_CS_PIN, OUPUT );
	pinMode ( SLAVE_BLUE_LED_PIN, OUPUT );
	flashPin ( SLAVE_BLUE_CS_PIN );
	flashPin ( SLAVE_BLUE_LED_PIN );

#endif /* ROLE */

	pos = 0;
	processIt = false;
	SPI.attachInterrupt ();
#endif /* MASTER */
}

void flashPin ( int pin ) {
	digitalWrite ( pin, LOW );
	for (int i = 0; i < FLASH_TIMES; i++) {
		digitalWrite ( pin, HIGH );
		delay ( FLASH_DURATION );
		digitalWrite ( pin, LOW );
		delay ( FLASH_DURATION );
	}
}

void loop () {
#if ROLE_IS_MASTER
	int len = commandWaiting ();
	if (len > 0) {
		digitalWrite ( SS, LOW );
		int index = 0;
		while (index < len) {
			SPI.transfer( buffer [index++] );
		}
		digitalWrite ( SS, HIGH );
	}
#else 
	if (processIt) {
		buffer [pos] = 0;
		Serial.print ( F ( "RCVD: [" ) );
		Serial.print ( buffer );
		Serial.println ( F ( "]" ) );
		pos = 0;
		processIt = false;
	}
#endif /* ROLE */
}

int commandWaiting () {
	if (!Serial.available ()) return 0;

	clearCommandBuffer ();

	int index = 0;
	while (Serial.available ()) {
		char data = (char) Serial.read ();
		buffer [index++] = data;
		//		buffer [index] = 0x00;
		if (index == 100) {
			return 100;
		}
	}
	return index;
}

void clearCommandBuffer () {
	for (int i = 0; i < 100; i++) { buffer [i] = 0x00; }
}

ISR ( SPI_SC_vect ) {
	byte c = SPDR;		// Grab byte from SPI Data Register
	if (pos < ( sizeof ( buffer ) - 1 )) {
		buffer [pos++] = c;
	}

	if (c == '\n') {
		processIt = true;
	}
}

