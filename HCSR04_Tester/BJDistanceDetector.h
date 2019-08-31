// BJDistanceDetector.h

#ifndef _BJDISTANCEDETECTOR_h
#define _BJDISTANCEDETECTOR_h

#include "arduino.h"

#include "Config.h"

class BJDistanceDetector {
public:
	char* name = "Distance Detector";

	bool tracing = TRACING;

	int lowMicros = DETECTOR_LOW_MICROS;
	int highMicros = DETECTOR_HIGH_MICROS;

	unsigned long duration, distanceCM, distanceIN;

	BJDistanceDetector () {

	}

	bool init () {
		if (!(DETECTOR_TRIGGER_PIN > 0)) {
			if (tracing) {
				Serial.println ( F ( "TRIGGER PIN MUST HAVE A POSITIVE VALUE" ) );
			}
			return false;
		}
		if (!(DETECTOR_ECHO_PIN > 0)) {
			if (TRACING) {
				Serial.println ( F ( "ECHO PIN MUST HAVE A POSITIVE VALUE" ) );
			}
			return false;
		}

		pinMode ( DETECTOR_TRIGGER_PIN, OUTPUT );
		pinMode ( DETECTOR_ECHO_PIN, INPUT );

		if (tracing) {
			Serial.print ( F ( "TRIGGER INITIALIZED ON PIN " ) );
			Serial.println ( DETECTOR_TRIGGER_PIN );

			Serial.print ( F ( "ECHO INITIALIZED ON PIN " ) );
			Serial.println ( DETECTOR_ECHO_PIN );
		}
		return true;
	}

	bool measureDistance () {
		digitalWrite ( DETECTOR_TRIGGER_PIN, LOW );  // Added this line
		delayMicroseconds ( lowMicros );
		digitalWrite ( DETECTOR_TRIGGER_PIN, HIGH );
		delayMicroseconds ( highMicros );
		digitalWrite ( DETECTOR_TRIGGER_PIN, LOW );

		//duration = pulseIn ( DETECTOR_ECHO_PIN, HIGH, DETECTOR_PULSEIN_TIMEOUT );
		duration = pulseIn ( DETECTOR_ECHO_PIN, HIGH );
		distanceCM = (float)( duration / DETECTOR_DIVISOR_CM );
		distanceIN = (float) ( duration / DETECTOR_DIVISOR_IN );
		delay ( DETECTOR_READ_DELAY );
		return ( distanceCM > 0 );
	}

	unsigned long getDistanceCM () {
		return distanceCM;
	}

	unsigned long getDistanceIN () {
		return distanceIN;
	}

	unsigned long getDuration () {
		return duration;
	}
};

#endif

