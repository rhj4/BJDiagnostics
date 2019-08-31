/*
	Name:       NeoPIxel01.ino
	Created:	12/13/2018 9:47:06 AM
	Author:     Bob Jones <bob@rhj4.com>
	Description:
		This is a test platform for the NeoPixel code and a development
		vehicle for defining the use of NeoPixels in NanoBots.
*/

#include "Config.h"
#include <Adafruit_NeoPixel.h>
#include <TimerOne.h>

Adafruit_NeoPixel strip = Adafruit_NeoPixel ( NEOPIXEL_LED_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800 );
Adafruit_NeoPixel dashboard = Adafruit_NeoPixel ( DASHBOARD_LENGTH, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800 );
Adafruit_NeoPixel frontBumper = Adafruit_NeoPixel ( FRONT_BUMPER_LENGTH, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800 );
Adafruit_NeoPixel rearBumper = Adafruit_NeoPixel ( REAR_BUMPER_LENGTH, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800 );


bool autoStart = AUTOSTART;
bool tracing = TRACING;
int demoCount = DEMO_COUNT;
bool doHeartBeat = false;
bool heartBeatOn = false;
bool transportActive = false;

void setup () {
	Serial.begin ( 115200 );
	pinMode ( NEOPIXEL_PIN, OUTPUT );
	strip.begin ();
	strip.show ();
	strip.clear ();

	if (TIMER_PIN > 0) {
		pinMode ( TIMER_PIN, OUTPUT );
		Timer1.initialize ( 1000000 );
		Timer1.attachInterrupt ( heartBeatTimer );
	}

	showMenu ();
}

//	Interrupt routing - gets called once every second
void heartBeatTimer () {
	doHeartBeat = !doHeartBeat;
}

//	Called by TimerISR
void heartBeat () {
	if (heartBeatOn) {
		if (transportActive) {
			setStrip ( HEARTBEAT_INDEX, 1, HEARTBEAT_COLOR | TRANSPORT_COLOR, EFFECT_NONE );
		} else {
			setStrip ( HEARTBEAT_INDEX, 1, HEARTBEAT_COLOR, EFFECT_NONE );
		}
	} else {
		setStrip ( HEARTBEAT_INDEX, 1, COLOR_OFF, EFFECT_NONE );
	}
	heartBeatOn = !heartBeatOn;
	doHeartBeat = false;
}

void transport ( bool status ) {
	uint8_t color = strip.getPixelColor ( HEARTBEAT_INDEX );

	color = color || ( status ) ? TRANSPORT_COLOR : HEARTBEAT_COLOR;
	setStrip ( HEARTBEAT_INDEX, 1, color, HEARTBEAT_EFFECT );
}


void loop () {
	if (doHeartBeat) {
		heartBeat ();
	}

	int cmd = getCommand ();
	if (cmd > -1) {
		doCommand ( cmd );
	} else if (autoStart) {
		demo ();
	}
}

void showMenu () {
	Serial.println ( F ( "\nNeo Pixle Menu:" ) );
	Serial.println ( F ( "1 - Signal Forward" ) );
	Serial.println ( F ( "2 - Signal Backing" ) );
	Serial.println ( F ( "3 - Signal Left" ) );
	Serial.println ( F ( "4 - Signal Right" ) );
	Serial.println ( F ( "5 - Signal Stopping" ) );
	Serial.println ( F ( "6 - Clear All" ) );
	Serial.println ( F ( "7 - Distance CLOSE" ) );
	Serial.println ( F ( "8 - Distance NEAR" ) );
	Serial.println ( F ( "9 - Distance FAR" ) );
	Serial.println ( F ( "10 - Transport Active" ) );
	Serial.println ( F ( "11 - Transport Inactive" ) );
	Serial.println ( F ( "0 - Demo" ) );
	Serial.println ( F ( "? - This Menu" ) );
}

int getCommand () {
	if (Serial.available ()) {
		if (Serial.peek () == '?') {
			showMenu ();
			Serial.read ();
			return -1;
		}

		return Serial.parseInt ();
	}

	return -1;
}

void doCommand ( int cmd ) {
	if (tracing) {
		Serial.print ( F ( "\nCMD=" ) );
		Serial.println ( cmd );
	}
	switch (cmd) {
	case NEOPIXEL_DEMO:
		if (demoCount > 0) {
			demoCount--;
			demo ();
		}
		break;

	case NEOPIXEL_FORWARD:
		signalForward ( );
		break;

	case NEOPIXEL_BACKWARD:
		signalBackward ( );
		break;

	case NEOPIXEL_TURN_LEFT:
		signalLeft ( );
		break;

	case NEOPIXEL_TURN_RIGHT:
		signalRight ( );
		break;

	case NEOPIXEL_STOP:
		signalStopping ( );
		break;

	case NEOPIXEL_OFF:
		if (tracing) {
			Serial.println ( F ( "ALL OFF" ) );
		}
		strip.clear ();
		break;

	case NEOPIXEL_CLOSE:
		setDashboardDistance ( DISTANCE_CLOSE );
		break;

	case NEOPIXEL_NEAR:
		setDashboardDistance ( DISTANCE_NEAR );
		break;

	case NEOPIXEL_FAR:
		setDashboardDistance ( DISTANCE_FAR );
		break;

	case NEOPIXEL_ACTIVE:
		transportActive = true;
		transport ( transportActive );
		break;

	case NEOPIXEL_INACTIVE:
		transportActive = false;
		transport ( transportActive );
		break;

	default:
		Serial.println ( F ( "Enter a number from 1..5" ) );
		break;
	}
}

void demo () {
	Serial.println ( F ( "Moving Forward..." ) );
	doCommand ( NEOPIXEL_FORWARD );
	delay ( 1000 );

	Serial.println ( F ( "Turning LEFT..." ) );
	doCommand ( NEOPIXEL_TURN_LEFT );
	delay ( 1000 );

	Serial.println ( F ( "Turning RIGHT..." ) );
	doCommand ( NEOPIXEL_TURN_RIGHT );
	delay ( 1000 );

	Serial.println ( F ( "Moving Backward..." ) );
	doCommand ( NEOPIXEL_BACKWARD );
	delay ( 1000 );

	Serial.println ( F ( "Stopping..." ) );
	doCommand ( NEOPIXEL_STOP );
	delay ( 1000 );

	Serial.println ( F ( "Turning Off the Lights..." ) );
	doCommand ( NEOPIXEL_OFF);
	delay ( 4000 );
}

void setStrip ( uint16_t startingIndex, uint16_t len, uint32_t color, uint8_t effect ) {
	if (tracing) {

		Serial.print ( F ( "Setting " ) );
		Serial.print ( len );
		Serial.print ( F ( " pins starting with " ) );
		Serial.print ( startingIndex );
		Serial.print ( F ( " to " ) );
		showColor ( color );
		Serial.print ( F ( ", EFFECT:" ) );
		showEffect ( effect );
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

void showEffect ( uint8_t effect ) {
	uint8_t blink = (uint8_t) ( effect & EFFECT_BLINK );
	uint8_t bright = (uint8_t) ( effect & EFFECT_BRIGHT );
	uint8_t dim = (uint8_t) ( effect & EFFECT_DIM );
	uint8_t R2L = (uint8_t) ( effect & EFFECT_R2L );
	uint8_t L2R = (uint8_t) ( effect & EFFECT_L2R );

	Serial.print ( effect );
	if (blink) Serial.print ( F ( " BLINK" ) );
	if (bright) Serial.print ( F ( " BRIGHT" ) );
	if (dim) Serial.print ( F ( " DIM" ) );
	if (R2L) Serial.print ( F ( " R2L" ) );
	if (L2R) Serial.print ( F ( " L2R" ) );
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

void signalForward () {
	if (tracing) {
		Serial.println ( F ( "Signaling FORWARD" ) );
	}

	setDashboardDirection ( DIRECTION_FORWARD );

	setStrip ( FRONT_BUMPER_INDEX, FRONT_BUMPER_LENGTH, FRONT_FWD_COLOR, FRONT_FWD_EFFECT );
	setStrip ( REAR_BUMPER_INDEX, REAR_BUMPER_LENGTH, REAR_FWD_COLOR, REAR_FWD_EFFECT );
}

void signalBackward () {
	if (tracing) {
		Serial.println ( F ( "Signaling BACKING" ) );
	}

	setDashboardDirection ( DIRECTION_BACKING );

	setStrip ( FRONT_BUMPER_INDEX, FRONT_BUMPER_LENGTH, FRONT_BACK_COLOR, FRONT_BACK_COLOR );
	setStrip ( REAR_BUMPER_INDEX, REAR_BUMPER_LENGTH, REAR_BACK_COLOR, REAR_BACK_EFFECT );
}

void signalStopping () {
	if (tracing) {
		Serial.println ( F ( "Signaling STOPPING" ) );
	}

	setDashboardDirection ( DIRECTION_STOPPED );

	setStrip ( FRONT_BUMPER_INDEX, FRONT_BUMPER_LENGTH, FRONT_STOP_COLOR, FRONT_STOP_EFFECT );
	int pin = 0;
	uint16_t repeat = STOPPING_REPEAT;
	while (repeat-- > 0) {
		setStrip ( FRONT_BUMPER_INDEX, FRONT_BUMPER_LENGTH, FRONT_STOP_COLOR, FRONT_STOP_EFFECT );
		delay ( STOPPING_PAUSE );
		setStrip ( REAR_BUMPER_INDEX, REAR_BUMPER_LENGTH, REAR_STOP_COLOR, REAR_STOP_EFFECT );
		delay ( STOPPING_PAUSE );
	}
}

void signalLeft ( ) {
	if (tracing) {
		Serial.println ( F ( "Signaling LEFT" ) );
	}

	setDashboardDirection ( DIRECTION_TURNING );

	int repeat = TURNING_REPEAT;
	while (--repeat > 0) {
		uint16_t pin = FRONT_BUMPER_LENGTH;
		uint16_t index = 0;
		while (index++ < FRONT_BUMPER_LENGTH) {
			strip.setPixelColor ( FRONT_BUMPER_INDEX + pin - 1, FRONT_LEFT_COLOR );
			strip.setPixelColor ( REAR_BUMPER_INDEX + pin-- - 1, REAR_LEFT_COLOR );
			strip.show ();
			delay ( TURNING_PAUSE );
		}
		setStrip ( FRONT_BUMPER_INDEX, FRONT_BUMPER_LENGTH, COLOR_OFF, EFFECT_NONE);
		setStrip ( REAR_BUMPER_INDEX, REAR_BUMPER_LENGTH, COLOR_OFF, EFFECT_NONE );
	}

	setStrip ( FRONT_BUMPER_INDEX, FRONT_BUMPER_LENGTH, FRONT_LEFT_COLOR, FRONT_LEFT_EFFECT );
	setStrip ( REAR_BUMPER_INDEX, REAR_BUMPER_LENGTH, REAR_LEFT_COLOR, REAR_LEFT_EFFECT );
}

void signalRight ( ) {
	if (tracing) {
		Serial.println ( F ( "Signaling RIGHT" ) );
	}

	setDashboardDirection ( DIRECTION_TURNING );

	int repeat = TURNING_REPEAT;
	while (--repeat > 0) {
		int pin = 0;
		uint16_t index = 0;
		while (index++ < FRONT_BUMPER_LENGTH) {
			strip.setPixelColor ( FRONT_BUMPER_INDEX + pin, FRONT_RIGHT_COLOR );
			strip.setPixelColor ( REAR_BUMPER_INDEX + pin++, REAR_RIGHT_COLOR );
			strip.show ();
			delay ( TURNING_PAUSE );
		}
		setStrip ( FRONT_BUMPER_INDEX, FRONT_BUMPER_LENGTH, COLOR_OFF, EFFECT_NONE );
		setStrip ( REAR_BUMPER_INDEX, REAR_BUMPER_LENGTH, COLOR_OFF, EFFECT_NONE );
	}

	setStrip ( FRONT_BUMPER_INDEX, FRONT_BUMPER_LENGTH, FRONT_RIGHT_COLOR, FRONT_RIGHT_EFFECT );
	setStrip ( REAR_BUMPER_INDEX, REAR_BUMPER_LENGTH, REAR_RIGHT_COLOR, REAR_RIGHT_EFFECT );
}

void setDashboardHeartBeat () {}
void setDashboardTransport () {
}

void setDashboardDirection ( uint8_t direction ) {
	switch (direction) {
	case DIRECTION_STOPPED:
		setStrip ( DIRECTION_INDEX, 1, DIR_STOPPED_COLOR, DIR_STOPPED_EFFECT );
		break;
	case DIRECTION_FORWARD:
		setStrip ( DIRECTION_INDEX, 1, DIR_FORWARD_COLOR, DIR_FORWARD_EFFECT );
		break;
	case DIRECTION_BACKING:
		setStrip ( DIRECTION_INDEX, 1, DIR_BACKING_COLOR, DIR_BACKING_EFFECT );
		break;
	case DIRECTION_TURNING:
		setStrip ( DIRECTION_INDEX, 1, DIR_TURNING_COLOR, DIR_TURNING_EFFECT );
		break;
	default:
		break;
	}
}

void setDashboardDistance ( uint8_t distance ) {
	switch (distance) {
	case DISTANCE_CLOSE:
		if (tracing) {
			Serial.println ( F ( "DISTANCE: CLOSE" ) );
		}
		setStrip ( DISTANCE_INDEX, 1, DIST_CLOSE_COLOR, DIST_CLOSE_EFFECT );
		break;
	case DISTANCE_NEAR:
		if (tracing) {
			Serial.println ( F ( "DISTANCE: NEAR" ) );
		}
		setStrip ( DISTANCE_INDEX, 1, DIST_NEAR_COLOR, DIST_NEAR_EFFECT );
		break;

	case DISTANCE_FAR:
		if (tracing) {
			Serial.println ( F ( "DISTANCE: FAR" ) );
		}
		setStrip ( DISTANCE_INDEX, 1, DIST_FAR_COLOR, DIST_FAR_EFFECT );
		break;

	default:
		setStrip ( DISTANCE_INDEX, 1, COLOR_OFF,EFFECT_NONE );
		break;
	}
	strip.show ();
}

