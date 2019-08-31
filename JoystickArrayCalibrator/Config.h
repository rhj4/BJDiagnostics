#pragma once
/********************************************************************
*
*	Configuration values for use with servo arrays. Each joystick
*	will ultimately manage two servo motors. 
*
*/
#define DESCRIPTION					"JOYSTICK ARRAY CALIBRATOR "
#define VERSION						"V1.0"

#define JOYSTICK_TRACING			true
#define JOYSTICK_COUNT				3			//	Number of joysticks (each handles 2 servos)
#define STARTING_MOTOR_NUMBER		0			//	First assignable motor number
#define MOTOR_INCREMENT				1			//	Add to STARTING_MOTOR_NUMBER to get next motor number

#define JOYSTICK_STOP_ZONE_SIZE		199

#define TEST_BUTTONS				false		//	If true, test that buttons work

#define JOYSTICK_NOISE_FILTER		10			//	Filter out noisy values
#define TIMEOUT_VALUE				2000		//	Settling time when reading mid point values

//	Used when calibrating. If you don't know the min/max values, use these
#define JOYSTICK_DEFAULT_MIN		2000		//	Set MIN above maximum allowed value
#define JOYSTICK_DEFAULT_MAX		0			//	Set MAX to zero and let it grow

//	Default values if none known:
//#define JOYSTICK1_CALIBRATED		false
//#define JOYSTICK1X_PIN			A0
//#define JOYSTICK1Y_PIN			A1
//#define JOYSTICK1_BUTTON			22
//#define JOYSTICK1_MIN_X			2000
//#define JOYSTICK1_MAX_X			0
//#define JOYSTICK1_MID_X			0
//#define JOYSTICK1_MIN_Y			2000
//#define JOYSTICK1_MAX_Y			0
//#define JOYSTICK1_MID_Y			0
//#define JOYSTICK1_SERVO_MOTOR_1	0
//#define JOYSTICK1_SERVO_MOTOR_2	0
//
//#define JOYSTICK2_CALIBRATED		false
//#define JOYSTICK2X_PIN			A2
//#define JOYSTICK2Y_PIN			A3
//#define JOYSTICK2_BUTTON			24
//#define JOYSTICK2_MIN_X			2000
//#define JOYSTICK2_MAX_X			0
//#define JOYSTICK2_MID_X			0
//#define JOYSTICK2_MIN_Y			2000
//#define JOYSTICK2_MAX_Y			0
//#define JOYSTICK2_MID_Y			0
//#define JOYSTICK2_SERVO_MOTOR_1	0
//#define JOYSTICK2_SERVO_MOTOR_2	0
//
//#define JOYSTICK3_CALIBRATED		false
//#define JOYSTICK3X_PIN			A4
//#define JOYSTICK3Y_PIN			A5
//#define JOYSTICK3_BUTTON			26
//#define JOYSTICK3_MIN_X			2000
//#define JOYSTICK3_MAX_X			0
//#define JOYSTICK3_MID_X			0
//#define JOYSTICK3_MIN_Y			0
//#define JOYSTICK3_MAX_Y			0
//#define JOYSTICK3_MID_Y			0
//#define JOYSTICK3_SERVO_MOTOR_1	0
//#define JOYSTICK3_SERVO_MOTOR_2	0


//	Generated output values
#define JOYSTICK1_CALIBRATED true
#define JOYSTICK1X_PIN 54
#define JOYSTICK1Y_PIN 55
#define JOYSTICK1_BUTTON 22
#define JOYSTICK1_MIN_X 0
#define JOYSTICK1_MAX_X 1023
#define JOYSTICK1_MID_X 523
#define JOYSTICK1_MIN_Y 0
#define JOYSTICK1_MAX_Y 1023
#define JOYSTICK1_MID_Y 502
#define JOYSTICK1_SERVO_MOTOR_1 1
#define JOYSTICK1_SERVO_MOTOR_2 2

#define JOYSTICK2_CALIBRATED true
#define JOYSTICK2X_PIN 56
#define JOYSTICK2Y_PIN 57
#define JOYSTICK2_BUTTON 24
#define JOYSTICK2_MIN_X 0
#define JOYSTICK2_MAX_X 1023
#define JOYSTICK2_MID_X 512
#define JOYSTICK2_MIN_Y 0
#define JOYSTICK2_MAX_Y 1023
#define JOYSTICK2_MID_Y 514
#define JOYSTICK2_SERVO_MOTOR_1 3
#define JOYSTICK2_SERVO_MOTOR_2 4

#define JOYSTICK3_CALIBRATED true
#define JOYSTICK3X_PIN 58
#define JOYSTICK3Y_PIN 59
#define JOYSTICK3_BUTTON 26
#define JOYSTICK3_MIN_X 0
#define JOYSTICK3_MAX_X 1023
#define JOYSTICK3_MID_X 507
#define JOYSTICK3_MIN_Y 0
#define JOYSTICK3_MAX_Y 1023
#define JOYSTICK3_MID_Y 500
#define JOYSTICK3_SERVO_MOTOR_1 5
#define JOYSTICK3_SERVO_MOTOR_2 6