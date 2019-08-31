#pragma once

//	Different robots have different configurations.
//	Capture each, give it a name and set its values.
#define TRIKE2						false
#define WHACKO						false
#define ROMEO						true
#define HCSR04						false

#define DETECTOR_SHOW_CM			true
#define DETECTOR_SHOW_INCHES		false

#define DETECTOR_LOW_MICROS			2
#define DETECTOR_HIGH_MICROS		10
#define DETECTOR_PULSEIN_TIMEOUT	100	// Microseconds to wait before timing out
#define DETECTOR_READ_DELAY			100
#define DETECTOR_DIVISOR_CM			58
#define DETECTOR_DIVISOR_IN			148
#if TRIKE2
#define DETECTOR_TRIGGER_PIN		3
#define DETECTOR_ECHO_PIN			2
//
#define DETECTOR_CLOSE_PIN			A2
#define DETECTOR_NEAR_PIN			A3
#define DETECTOR_FAR_PIN			A4
#elif WHACKO
#define DETECTOR_TRIGGER_PIN		21
#define DETECTOR_ECHO_PIN			20

#define DETECTOR_CLOSE_PIN			26
#define DETECTOR_NEAR_PIN			24
#define DETECTOR_FAR_PIN			22
#elif ROMEO
#define DETECTOR_TRIGGER_PIN		3
#define DETECTOR_ECHO_PIN			2

#define DETECTOR_CLOSE_PIN			22
#define DETECTOR_NEAR_PIN			23
#define DETECTOR_FAR_PIN			24

#elif HCSR04
#define DETECTOR_TRIGGER_PIN		21
#define DETECTOR_ECHO_PIN			20

#define DETECTOR_CLOSE_PIN			22
#define DETECTOR_NEAR_PIN			23
#define DETECTOR_FAR_PIN			24
#endif


#define DETECTOR_CLOSE_CM			10
#define DETECTOR_NEAR_CM			30
#define DETECTOR_FAR_CM				100

#define TRACING						true
#define USE_DETECTOR_CLASS			false
