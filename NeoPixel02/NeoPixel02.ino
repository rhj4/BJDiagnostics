/*
	Name:       NeoPIxel01.ino
	Created:	12/13/2018 9:47:06 AM
	Author:     Bob Jones <bob@rhj4.com>
	Description:
		This is a test platform for the NeoPixel code and a development
		vehicle for defining the use of NeoPixels in NanoBots.
*/

#include <Adafruit_NeoPixel.h>

#define NEOPIXEL_PIN		5			//	PIN to connect strip to
#define NEOPIXEL_LED_COUNT	4

Adafruit_NeoPixel strip = Adafruit_NeoPixel ( NEOPIXEL_LED_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800 );

bool tracing = true;
uint32_t color;

void setup () {
	Serial.begin ( 115200 );
	pinMode ( NEOPIXEL_PIN, OUTPUT );
	strip.begin ();
	strip.show ();
	strip.clear ();
	color = strip.Color ( 255, 255, 0 );
	test ();
}

void loop () {
}


void setStrip ( uint16_t startingIndex, uint16_t len, uint32_t color) {
	if (tracing) {
		Serial.print ( F ( "Setting " ) );
		Serial.print ( len );
		Serial.print ( F ( " pins starting with " ) );
		Serial.print ( startingIndex );
		Serial.print ( F ( " to " ) );
		showColor ( color );
		Serial.println ();
	}

	for (uint16_t pin = startingIndex; pin < startingIndex + len; pin++) {
		strip.setPixelColor ( pin, color );
	}
	strip.show ();
}

void showColor ( uint32_t color ) {
	uint8_t red = (int8_t) ( color >> 16 );
	uint8_t green = (int8_t) ( color >> 8 );
	uint8_t blue = (int8_t) color;
	Serial.print ( F ( "RED:" ) ); Serial.print ( red );
	Serial.print ( F ( ", GREEN:" ) ); Serial.print ( green );
	Serial.print ( F ( ", BLUE:" ) ); Serial.print ( blue );
}

uint32_t getColor ( uint8_t pin ) {
	return strip.getPixelColor ( pin );
}

uint8_t getBrightness ( ) {
	return strip.getBrightness ( );
}

void setBrightness ( uint8_t brightness ) {
	strip.setBrightness ( brightness );
}

void test ( ) {
	int repeat = 10;
	while (--repeat > 0) {
		uint16_t pin = 4;
		uint16_t index = 0;
		uint32_t color = strip.Color ( 255, 255, 0 );
		while (index++ < 4) {
			strip.setPixelColor ( pin-- - 1, color);
			strip.show ();
			delay ( 100 );
		}
		setStrip ( 0, 4, strip.Color(0,0,0));
	}

	setStrip ( 0, 4, strip.Color(255,255,0));
}

