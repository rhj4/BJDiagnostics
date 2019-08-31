/********************************************************************
*
*	Name:	HCSR04 Tester
*	Author:	Bob Jones <bob@rhj4.com>
*	Version:1.0.0
*	Description:
*		Test HCSR04 Ultrasonic Accoustic Obstacle Detector both in
*		this code or in a separate class (BJDistanceDetector).
*
*		The code draws on a Config.h file to define pin settings
*		and other options.
*/
#include "Config.h"
#if USE_DETECTOR_CLASS
#include "BJDistanceDetector.h"
BJDistanceDetector *detector;
#endif

bool tracing = TRACING;

unsigned long duration, distanceCM, distanceIN;

int lowMicros = DETECTOR_LOW_MICROS;
int highMicros = DETECTOR_HIGH_MICROS;

void setup () {
	Serial.begin ( 115200 );
#if USE_DETECTOR_CLASS
	Serial.println ( F ( "Using Detector Class..." ) );
	detector = new BJDistanceDetector;
	if (!detector->init ()) {
		Serial.println ( F ( "Detector initialization failed" ) );
		exit ( -1 );
	}
#else 
	Serial.println ( F ( "Using Local Code..." ) );
	if (!DETECTOR_TRIGGER_PIN > 0) {
		if (TRACING) {
			Serial.println ( F ( "TRIGGER PIN MUST HAVE A POSITIVE VALUE" ) );
		}
		exit ( -1 );
	}
	if (!DETECTOR_ECHO_PIN > 0) {
		if (TRACING) {
			Serial.println ( F ( "ECHO PIN MUST HAVE A POSITIVE VALUE" ) );
		}
		exit ( -1 );
	}

	pinMode ( DETECTOR_TRIGGER_PIN, OUTPUT );
	pinMode ( DETECTOR_ECHO_PIN, INPUT );
#endif

	pinMode ( DETECTOR_CLOSE_PIN, OUTPUT );
	pinMode ( DETECTOR_NEAR_PIN, OUTPUT );
	pinMode ( DETECTOR_FAR_PIN, OUTPUT );

	testLEDs ();
	showConfig ();
}

void loop () {
	checkForCommand ();

#if USE_DETECTOR_CLASS
	if (detector->measureDistance ()) {
		if (DETECTOR_SHOW_CM) {
			distanceCM = detector->getDistanceCM ();
		} else {
			distanceIN = detector->getDistanceIN ();
		}

		duration = detector->getDuration ();
	}
#else 
	digitalWrite ( DETECTOR_TRIGGER_PIN, LOW );
	delayMicroseconds ( DETECTOR_LOW_MICROS );
	digitalWrite ( DETECTOR_TRIGGER_PIN, HIGH );
	delayMicroseconds ( DETECTOR_HIGH_MICROS );
	digitalWrite ( DETECTOR_TRIGGER_PIN, LOW );
	duration = pulseIn ( DETECTOR_ECHO_PIN, HIGH );
	distanceCM = (float) ( duration / DETECTOR_DIVISOR_CM );
	distanceIN = (float) ( duration / DETECTOR_DIVISOR_IN );
#endif

	if (tracing) {
		Serial.print ( F ( "Duration:" ) );
		Serial.print ( duration );
		Serial.print ( F ( ", Distance:" ) );
#if DETECTOR_SHOW_CM
		Serial.print ( distanceCM );
		Serial.print ( F ( "cm" ) );
#else
		Serial.print ( distanceIN );
		Serial.print ( F ( "inches" ) );
#endif		
	}

	if (distanceCM <= DETECTOR_CLOSE_CM) {  // This is where the LED On/Off happens
		turnPinOff ( DETECTOR_NEAR_PIN );
		turnPinOff ( DETECTOR_FAR_PIN );
		turnPinOn ( DETECTOR_CLOSE_PIN ); // When the Red condition is met, the Green LED should turn off
		if (tracing) {
			Serial.println ( F ( " CLOSE" ) );
		}
	} else if (distanceCM <= DETECTOR_NEAR_CM) {
		turnPinOff ( DETECTOR_CLOSE_PIN );
		turnPinOff ( DETECTOR_FAR_PIN );
		turnPinOn ( DETECTOR_NEAR_PIN );
		if (tracing) {
			Serial.println ( F ( " NEAR" ) );
		}
	} else if (distanceCM > DETECTOR_NEAR_CM) {
		turnPinOff ( DETECTOR_CLOSE_PIN );
		turnPinOff ( DETECTOR_NEAR_PIN );
		turnPinOn ( DETECTOR_FAR_PIN );
		if (tracing) {
			Serial.println ( F ( " FAR" ) );
		}
	} else {
		turnPinOff ( DETECTOR_CLOSE_PIN );
		turnPinOff ( DETECTOR_NEAR_PIN );
		turnPinOff ( DETECTOR_FAR_PIN );
		if (tracing) {
			Serial.println ( F ( " ???" ) );
		}

	}

	delay ( 100 );
}

void testLEDs () {
	flashPin ( DETECTOR_CLOSE_PIN, 10, 50 );
	flashPin ( DETECTOR_NEAR_PIN, 10, 50 );
	flashPin ( DETECTOR_FAR_PIN, 10, 50 );
}

static void turnPinOn ( int pin ) {
	digitalWrite ( pin, HIGH );
}

static void turnPinOff ( int pin ) {
	digitalWrite ( pin, LOW );
}

void flashPin ( int pin, int count, int duration ) {
	Serial.print ( F ( "Flashing pin " ) );
	Serial.print ( pin );
	Serial.print ( F ( " for " ) );
	Serial.print ( duration );
	Serial.print ( F ( "ms " ) );
	Serial.print ( count );
	Serial.println ( F ( " times" ) );

	for (int i = 0; i < count; i++) {
		turnPinOn ( pin );
		delay ( duration );
		turnPinOff ( pin );
		delay ( duration );
	}
}

void checkForCommand () {
	if (!Serial.available ()) {
		return;
	}

	char cmd;
	while (Serial.available ()) {
		cmd = Serial.read ();
		switch (cmd) {
		case 0x10:
		case 0x13:
			break;

		case '?':
			Serial.println ( F ( "Commands:" ) );
			Serial.println ( F ( "+ turns tracing on" ) );
			Serial.println ( F ( "- turns tracing off" ) );
			Serial.println ( F ( "l shortens delay before setting trigger" ) );
			Serial.println ( F ( "L lengthens delay before setting trigger" ) );
			Serial.println ( F ( "h decreases delay before checking echo" ) );
			Serial.println ( F ( "H increases delay before checking echo" ) );
			Serial.println ( F ( "c shows configuration" ) );
			Serial.println ( F ( "f flashes close, near and far LEDs" ) );
			Serial.println ( F ( "? prints this list." ) );
			break;

		case '+':
			tracing = true;
			break;

		case '-':
			tracing = false;
			break;

		case 'l':
			Serial.print ( F ( "LOW Micros set to " ) );
#if USE_DETECTOR_CLASS
			Serial.println ( --detector->lowMicros );
#else
			Serial.println ( --lowMicros );
#endif
			break;

		case 'L':
			Serial.print ( F ( "LOW Micros set to " ) );
#if USE_DETECTOR_CLASS
			Serial.println ( ++detector->lowMicros );
#else
			Serial.println ( --lowMicros );
#endif
			break;

		case 'h':
			Serial.print ( F ( "HIGH Micros set to " ) );
#if USE_DETECTOR_CLASS
			Serial.println ( --detector->highMicros );
#else
			Serial.println ( --highMicros );
#endif
			break;

		case 'H':
			Serial.print ( F ( "HIGH Micros set to " ) );
#if USE_DETECTOR_CLASS
			Serial.println ( ++detector->highMicros );
#else
			Serial.println ( ++highMicros );
#endif
			break;

		case 'c':
		case 'C':
			showConfig ();
			break;

		case 'f':
		case 'F':
			testLEDs ();
			break;
		}
	}

#if USE_DETECTOR_CLASS
	detector->tracing = tracing;
#endif

}

void showConfig () {
	Serial.print ( F ( "\nHCSR04 Test Platform on " ) );
#if TRIKE2
	Serial.print ( F ( "TRIKE2 " ) );
#elif WHACKO
	Serial.print ( F ( "WHACKO " ) );
#elif HCSR04
	Serial.print ( F ( "HCSR04 TESTER " ) );
#endif

#if USE_DETECTOR_CLASS
	Serial.println ( F ( "Using DistanceDetector class" ) );
#else
	Serial.println ( F ( "Using code in this sketch" ) );
#endif

	Serial.print ( F ( "Detector Trigger Pin: " ) );
	Serial.print ( DETECTOR_TRIGGER_PIN );
	Serial.println ( F ( " sends signal" ) );
	Serial.print ( F ( "Detector Echo Pin:" ) );
	Serial.print ( DETECTOR_ECHO_PIN );
	Serial.println ( F ( " hears echo" ) );

	Serial.print ( "Detector Close Pin: " );
	Serial.print ( DETECTOR_CLOSE_PIN );
	Serial.print ( F ( " lights if distance less than " ) );
	Serial.println ( DETECTOR_CLOSE_CM );

	Serial.print ( "Detector Near Pin: " );
	Serial.print ( DETECTOR_NEAR_PIN );
	Serial.print ( F ( " lights if distance less than " ) );
	Serial.println ( DETECTOR_NEAR_CM );

	Serial.print ( "Detector FAR Pin: " );
	Serial.print ( DETECTOR_FAR_PIN );
	Serial.print ( F ( " lights if distance more than " ) );
	Serial.println ( DETECTOR_FAR_CM );

	Serial.print ( F ( "Showing results In " ) );
#if DETECTOR_SHOW_CM
	Serial.println ( F ( "Centemeters (CM)" ) );
#else
	Serial.println ( F ( "Inches (IN)" ) );
#endif
	Serial.print ( F ( "Current Values: " ) );
	Serial.print ( F ( "DURATION=" ) );
	Serial.print ( duration );
	Serial.print ( F ( ", DISTANCE=" ) );
#if DETECTOR_SHOW_CM
	Serial.print ( distanceCM );
	Serial.println ( F ( "cm" ) );
#else
	Serial.print ( distanceIN );
	Serial.println ( F ( "\"" ) );
#endif
	Serial.println ();
}