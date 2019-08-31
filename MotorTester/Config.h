#pragma once
#define NAME				"Motor Tester"
#define VERSION				"2.0.0"

#define MOTOR_TYPE_UNKNOWN	0
#define MOTOR_TYPE_L298N	1
#define MOTOR_TYPE_L293D	2
#define MOTOR_TYPE_L9110	3
#define MOTOR_TYPE_SERVO	4

#define MOTOR_TYPE_DEFAULT	MOTOR_TYPE_SERVO

#define MOTOR_TYPE_PIN		4

#define L9110_IB1			5 // D5 --> Motor B Input A --> MOTOR B +
#define L9110_IA1			7 // D7 --> Motor B Input B --> MOTOR B -

#define L9110_IB2			6 // D6 --> Motor A Input A --> MOTOR A +
#define L9110_IA2			8 // D8 --> Motor A Input B --> MOTOR A -

// functional connections
#define L9110_MOTOR_A_PWM	L9110_IB1 // Motor B PWM Speed
#define L9110_MOTOR_A_DIR	L9110_IA1 // Motor B Direction

#define L9110_MOTOR_B_PWM	L9110_IB2 // Motor B PWM Speed
#define L9110_MOTOR_B_DIR	L9110_IA2 // Motor B Direction

// the actual values for "fast" and "slow" depend on the motor
#define L9110_PWM_SLOW		50  // arbitrary slow speed PWM duty cycle
#define L9110_PWM_FAST		200 // arbitrary fast speed PWM duty cycle
#define L9110_DIR_DELAY		1000 // brief delay for abrupt motor changes

//	Left Motor
#define L298N_ENA				5		//	White
#define L298N_IN1				9		//	Gray
#define L298N_IN2				10		//	Purple
//	Right Motor
#define L298N_ENB				6		//	Yellow
#define L298N_IN3				7		//	Blue
#define L298N_IN4				8		//	Green

//	Left Motor
#define L293D_ENA				5		//	White
#define L293D_IN1				7		//	Gray
#define L293D_IN2				8		//	Purple
//	Right Motor
#define L293D_ENB				6		//	Yellow
#define L293D_IN3				9		//	Blue
#define L293D_IN4				10		//	Green

#define MIN_SPEED				15
#define MAX_SPEED				255
#define NORMAL_SPEED			200
#define INCREMENT				5
#define DECREMENT				5

#define SWAP_MOTORS				false

#define DELAY					1000
#define LOOP_DELAY				200

#define STOPPED 0
#define FORWARD 1
#define BACKWARD 2

#define L298N_STOPPED_PIN			16
#define L298N_FORWARD_PIN			18
#define L298N_BACKWARD_PIN			17

#define L293D_STOPPED_PIN			16
#define L293D_FORWARD_PIN			18
#define L293D_BACKWARD_PIN			17

#define L9110_STOPPED_PIN			16
#define L9110_FORWARD_PIN			17
#define L9110_BACKWARD_PIN			18

#define LEFT_SERVO_PIN				5
#define RIGHT_SERVO_PIN				6