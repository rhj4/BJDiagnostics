/********************************************************************
*
*	Name:			HC-05 AT Utility
*	Version:		1.0.0
*	Created On:		5/18/18
*	Author:			Bob Jones <bob@rhj4.com>
*	Description:	This utility program performs two highly useful
*	tasks:
*		1)	Puts an HC-05 Module into AT Command mode
*			and sends and receives AT commands from the HC-05
*		2)	Pairs two HC-05 modules
*
*	The process of putting a module into AT mode is a bit tricky.
*	First, the power must be cut to the module. This is accomplished
*	using the handy small switch at the feet of the module.
*
*	With the power off, we set the ENABLE (EN_1) pin to HIGH.
*	Then we must send a simple command ("AT\r\n") to the module.
*	If all is right and the stars align, the module will send back
*	"OK" and we are in business.
*
*	We automate this by setting the SWITCH_PIN to HIGH when the
*	switch is connected. When SWITCH_PIN goes HIGH, we catch this
*	in the loop and send the AT command and check for an OK response.
*
*	After that, we can send AT commands simply by entering them in
*	the Serial monitor window.
*
*	When completed, or if bored and wanting to do something else,
*	simply turn the switch off and we will exit AT Command Mode,
*	the Green light will go off and you may kiss the frog or the
*	princess... your choice.
*
*	LED Indicators:
*
*	There are three LED indicators that can be implemented as
*	three separate LEDs or as a single RGB LED:
*
*		RED - the switch is disconnected
*		BLUE - switch connected (ENA_1 is HIGH)
*		GREEN - in AT Command Mode
*	Update History:
*
*
*/

//	TODO: figure out how to add the switch state detector
//	TODO: Test to confirm that closing the switch sets ENA HIGH
//	TODO: Finish wiring the board
//	TODO: How does HC05 pairing happen?

#include "Config.h"
#include "BJHC05Utility.h"


BJHC05Utility *hc05;

void setup () {
	Serial.begin ( 115200 );
	Serial.println ( F ( "Performing HC05 AT Utility Setup" ) );

	hc05 = new BJHC05Utility;
	hc05->setup ();
	hc05->showHelp ();

	Serial.println ( "\nEnter any Command:" );
	delay ( 500 );
}

void loop () {

	hc05->loop ();

	delay ( LOOP_WAIT );
}

