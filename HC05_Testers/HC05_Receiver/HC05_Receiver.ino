/*
    Name:       HC05_Receiver.ino
    Created:	8/17/2018 4:28:07 PM
    Author:     Bob Jones <bob@rhj4.com>

	This sketch tests HC05 Bluetooth transfer protocols.
	It operates in two modes: Transmitter and Receiver.
	The Config.h file defines which role this code will assume.
*/
#include "Config.h"

#include <SoftwareSerial.h>
#if ROLE_IS_TRANSMITTER
SoftwareSerial driver ( 4, 3 );
#elif ROLE_IS_RECEIVER
SoftwareSerial driver ( 4, 3 );
#endif /* ROLE_IS_ */

#if SUPPORT_IC2_DISPLAY
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display ( OLED_RESET );
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
#endif /* SUPPORT_IC2_DISPLAY */

char buffer [BUFFER_SIZE];
int index = 0;

void setup()
{
	Serial.begin(115200);
	driver.begin(9600);
	clearBuffer ();

#if SUPPORT_IC2_DISPLAY
	display.begin ( SSD1306_SWITCHCAPVCC, 0x3C );  // initialize with the I2C addr 0x3C (for the 128x32)
// Clear the buffer.
	display.clearDisplay ();
	// text display tests
	display.setTextSize ( 1 );
	display.setTextColor ( WHITE );
	display.setCursor ( 0, 0 );

#if  ROLE_IS_TRANSMITTER 
	display.println ( "NanoBot HC05 Transmitter Test" );
#elif ROLE_IS_RECEIVER
	display.println ( "NanoBot HC05 Receiver Test" );
#endif /* ROLE_IS_TRANSMITTER */

	display.setTextColor ( BLACK, WHITE ); // 'inverted' text
	display.setTextColor ( WHITE );
	display.display ();
	delay ( AFTER_SETUP_PAUSE );
	display.clearDisplay ();
	display.display ();
	display.setCursor ( 0, 0 );
	display.ssd1306_command ( SSD1306_ACTIVATE_SCROLL );
#endif /* */

#if ROLE_IS_TRANSMITTER
	Serial.println(F("NanoBots "))
	Serial.println ( F ( "Enter your message in serial window..." ) );
#elif ROLE_IS_RECEIVER
	Serial.println ( F ( "Waiting for messages from afar..." ) );
#endif /* ROLE_IS_ */
}

void loop()
{
#if ROLE_IS_TRANSMITTER
	sendData ();
#elif ROLE_IS_RECEIVER
	getData ();
#endif /* ROLE_IS_TRANSMITTER */ 

	delay(LOOP_DELAY);
}

#if ROLE_IS_TRANSMITTER
void sendData () {
	if (Serial.available ()) {

#if ACTIVITY_LED > 0
		digitalWrite ( ACTIVITY_LED, HIGH );
#endif /* ACTIVITY_LED > 0 */

		while (Serial.available ()) {
			char data = Serial.read ();
			if (data >= 0x20) {
				if (index == BUFFER_SIZE - 1) {
					writeBuffer ();
					Serial.println ();
				}
				buffer [index++] = data;
				buffer [index] = 0x00;
				Serial.print ( data );
			}
		}
		writeBuffer ();
#if ACTIVITY_LED > 0
		digitalWrite ( ACTIVITY_LED, LOW );
#endif /* ACTIVITY_LED > 0 */
	}

}
#elif ROLE_IS_RECEIVER
void getData () {
	if (driver.available ()) {
		while (driver.available ()) {
			char data = driver.read ();
			if (data >= 0x20) {
				buffer [index++] = data;
			}
		}

		writeBuffer ();
	}
}
#endif /* ROLE_IS_... */

void writeBuffer () {
	int len = driver.write ( buffer );
	Serial.println ();
	Serial.print ( len );
	Serial.print ( ':' );
	Serial.println ( buffer );
	clearBuffer ();

#if SUPPORT_IC2_DISPLAY
	display.println ( buffer );
	display.display ();
#endif /* SUPPORT_IC2_DISPLAY */
}

void clearBuffer () {
	for (int i = 0; i < BUFFER_SIZE; i++) {
		buffer [i] = 0x00;
	}
	index = 0;
}