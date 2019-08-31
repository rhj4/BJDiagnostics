#pragma once
/********************************************************************
*
*	This configuration implements three separate but connected 
*	Adafruit NeoPixel strips:
*
*	1) Visible Dashboard:4 LEDs mounted on front of top deck:
*		@ 0: Heartbeat ( red ) and Transport Active ( blue )
*		@ 1: Direction:
*			Forward: White
*			Stopped:Off ( black )
*			Backing:blinking Red
*		@ 2: Distance:
*			Close: blinking bright red
*			 Near:blinking yellow
*			 Far:bright green
*		@ 3: Reserved for future use
*	2) Front Direction Signals:3 LEDs mounted across front of lower deck:
*		@ 4 - 6: Direction
*			Forward: Bright White
*			Stopped:Off
*			Backing:Blinking bright red
*			Left Turn:Red arrow pointing left
*			Right Turn:Red arrow pointing right
*	3) Rear Direction Signals:3 LEDs mounted across back of lower deck:
*		@7-9: Direction
*			Forward: light red
*			Stopped:Off
*			Backing:Blinking bright red
*			Left Turn:Red arrow pointing left
*			Right Turn:Red arrow pointing right
*
********************************************************************/


#define AUTOSTART			false		//	Automatically start when plugged in
#define TRACING				true		//	Diagnostic tracing
#define DEMO_COUNT			10			//	Number of demo cycles before going blank
#define TIMER_PIN			10			//	If > 0, use timer interrupts

#define COLOR_OFF			strip.Color ( 0,0,0 )
#define COLOR_BLUE			strip.Color ( 0,0,255 )
#define COLOR_GREEN			strip.Color ( 0, 255, 0 )
#define COLOR_LIGHT_BLUE	strip.Color ( 0, 155, 255 )
#define COLOR_RED			strip.Color ( 255, 0, 0 )
#define COLOR_LIGHT_RED		strip.Color ( 127, 0, 0 )
#define COLOR_PURPLE		strip.Color ( 255, 0, 255 )
#define COLOR_YELLOW		strip.Color ( 255, 255, 0 )
#define COLOR_WHITE			strip.Color ( 255,255,255 )

//	Effects
#define EFFECT_NONE			0x00		//	No special effects
#define EFFECT_BLINK		0x01		//	Blink pin
#define EFFECT_BRIGHT		0x02		//	Set brightness to high
#define EFFECT_DIM			0x04		//	Set brightness to low
#define EFFECT_R2L			0x08		//	light Right to left
#define EFFECT_L2R			0x10		//	Light Left to Right

//	Brightness
#define BRIGHTNESS_OFF		0x00
#define BRIGHTNESS_DIM		0x10
#define BRIGHTNESS_HIGH		0xFF

//	Pause durations
#define STOPPING_PAUSE		100
#define TURNING_PAUSE		100
#define FORWARD_PAUSE		100

//	Iteration (Repeat) Counts
#define TURNING_REPEAT		10
#define STOPPING_REPEAT		10
#define FORWARD_REPEAT		1


#define DASHBOARD_INDEX		0
#define DASHBOARD_LENGTH	4
#define FRONT_BUMPER_INDEX	4
#define FRONT_BUMPER_LENGTH	3
#define REAR_BUMPER_INDEX	7
#define REAR_BUMPER_LENGTH	3
#define STRIP_FIRST_PIN		DASHBOARD_INDEX	//	Starting index into LED strip
#define STRIP_LENGTH		DASHBOARD_LENGTH + FRONT_BUMPER_LENGTH + REAR_BUMPER_LENGTH

//	DASHBOARD DEFINITION
#define HEARTBEAT_INDEX		DASHBOARD_INDEX
#define HEARTBEAT_COLOR		COLOR_LIGHT_RED
#define HEARTBEAT_EFFECT	EFFECT_BLINK

#define TRANSPORT_INDEX		HEARTBEAT_INDEX
#define TRANSPORT_COLOR		COLOR_LIGHT_BLUE

#define DIRECTION_INDEX		DASHBOARD_INDEX + 1
#define DIRECTION_STOPPED	0
#define DIRECTION_FORWARD	1
#define DIRECTION_BACKING	2
#define DIRECTION_TURNING	3

#define DIR_STOPPED_COLOR	COLOR_OFF
#define DIR_STOPPED_EFFECT	EFFECT_DIM
#define DIR_FORWARD_COLOR	COLOR_WHITE
#define DIR_FORWARD_EFFECT	EFFECT_BRIGHT
#define DIR_BACKING_COLOR	COLOR_RED
#define DIR_BACKING_EFFECT	EFFECT_BLINK
#define DIR_TURNING_COLOR	COLOR_RED
#define DIR_TURNING_EFFECT	EFFECT_BRIGHT + EFFECT_BLINK

#define DISTANCE_INDEX		DASHBOARD_INDEX + 2
#define DISTANCE_CLOSE		0
#define DISTANCE_NEAR		1
#define DISTANCE_FAR		2
#define DIST_CLOSE_COLOR	COLOR_RED
#define DIST_CLOSE_EFFECT	EFFECT_BLINK + EFFECT_BRIGHT
#define DIST_NEAR_COLOR		COLOR_YELLOW
#define DIST_NEAR_EFFECT	EFFECT_BLINK + EFFECT_BRIGHT
#define DIST_FAR_COLOR		COLOR_GREEN
#define DIST_FAR_EFFECT		EFFECT_NONE

//	FRONT BUMPER DEFINITION
#define FRONT_BUMPER_INDEX	DASHBOARD_INDEX + DASHBOARD_LENGTH
#define FRONT_FWD_COLOR		COLOR_WHITE
#define FRONT_FWD_EFFECT	EFFECT_NONE

#define FRONT_BACK_COLOR	COLOR_RED
#define FRONT_BACK_EFFECT	EFFECT_BRIGHT + EFFECT_BLINK

#define FRONT_LEFT_COLOR	COLOR_RED
#define FRONT_LEFT_EFFECT	EFFECT_BRIGHT + EFFECT_BLINK + EFFECT_R2L

#define FRONT_RIGHT_COLOR	COLOR_RED
#define FRONT_RIGHT_EFFECT	EFFECT_BRIGHT + EFFECT_BLINK + EFFECT_L2R

#define FRONT_STOP_COLOR	COLOR_OFF
#define FRONT_STOP_EFFECT	EFFECT_NONE

//	REAR BUMPER DEFINITION
#define REAR_BUMPER_INDEX	DASHBOARD_INDEX + DASHBOARD_LENGTH + FRONT_BUMPER_LENGTH

#define REAR_FWD_COLOR		COLOR_RED
#define REAR_FWD_EFFECT		EFFECT_NONE

#define REAR_BACK_COLOR		COLOR_RED
#define REAR_BACK_EFFECT	EFFECT_BRIGHT + EFFECT_BLINK

#define REAR_LEFT_COLOR		COLOR_RED
#define REAR_LEFT_EFFECT	EFFECT_BRIGHT + EFFECT_BLINK + EFFECT_R2L

#define REAR_RIGHT_COLOR	COLOR_RED
#define REAR_RIGHT_EFFECT	EFFECT_BRIGHT + EFFECT_BLINK + EFFECT_L2R

#define REAR_STOP_COLOR		COLOR_OFF
#define REAR_STOP_EFFECT	EFFECT_NONE

#define NEOPIXEL_PIN		5			//	PIN to connect strip to
#define NEOPIXEL_LED_COUNT	STRIP_LENGTH

//	Command Definitions
#define NEOPIXEL_DEMO		0
#define NEOPIXEL_FORWARD	1
#define NEOPIXEL_BACKWARD	2
#define NEOPIXEL_TURN_LEFT	3
#define NEOPIXEL_TURN_RIGHT	4
#define NEOPIXEL_STOP		5
#define NEOPIXEL_OFF		6
#define NEOPIXEL_CLOSE		7
#define NEOPIXEL_NEAR		8
#define NEOPIXEL_FAR		9
#define NEOPIXEL_ACTIVE		10
#define NEOPIXEL_INACTIVE	11

