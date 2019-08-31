#pragma once
/*******************************************************************
*
*	Joystick Calibration and Testing Tool Configuration Values
*
********************************************************************/

#define JOYSTICK_X_PIN					A0
#define JOYSTICK_Y_PIN					A1

#define JOYSTICK_SHIELD					false

#if JOYSTICK_SHIELD
#define JOYSTICK_BUTTON_A_PIN			2
#define JOYSTICK_BUTTON_B_PIN			3
#define JOYSTICK_BUTTON_C_PIN			4
#define JOYSTICK_BUTTON_D_PIN			5
#define JOYSTICK_BUTTON_E_PIN			6
#define JOYSTICK_BUTTON_F_PIN			7
#endif /* JOYSTICK_SHIELD */
#define JOYSTICK_BUTTON_PIN				22
#define JOYSTICK_SLOP					10			// Noise filter

#define JOYSTICK_MAX_X					1023
#define JOYSTICK_MIN_X					0
#define JOYSTICK_MAX_Y					1023
#define JOYSTICK_MIN_Y					0

#define JOYSTICK_CALIBRATION_LOOPS		50

//	Set to true to interpret X values as Max_X - X
#define JOYSTICK_INVERT_X	false

//	Set to true to interpret Y values as Max_Y - Y
#define JOYSTICK_INVERT_Y	true

//	Set to true to read X from Y pin and Y from X pin
#define JOYSTICK_SWAP_X_Y	false

