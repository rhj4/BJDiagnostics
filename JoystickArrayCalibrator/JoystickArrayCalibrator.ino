/********************************************************************
*
*	Name:	BJJoystickArrayCalibrator
*	Date:	August 21, 2019
*	Auth:	Bob Jones <bob@rhj4.com>
*	Desc:	Calibrates each joystick in array of joysticks.
*			For use when generating a Getter that will control
*			an array of Joysticks, each of which manages two
*			Servo motors. 
*
*	Notes:	This program will generate considerable output on the
*			Serial Monitor. The final output will be a series of
*			"#define" statements that you can copy from this code
*			to your NanoBots Config.h file.
*
*			You can validate the output of this program by copying
*			it back into the Config.h file that controls this code.
*			If everything is OK, this program will simply generate
*			the same defines that it just read.
*/
#include "Config.h"
#include <BJJoystickArrayDefinitions.h>
#include "BJJoystickArrayCalibrator.h"

BJJoyStickArrayCalibrator *calibrator;

void setup () {
	Serial.begin ( 115200 );
	Serial.print ( DESCRIPTION );
	Serial.println ( VERSION );

	calibrator = new BJJoyStickArrayCalibrator ();
	if (calibrator->configure ()
		 && calibrator->testButtons ()
		 && calibrator->calibrateJoySticks ()) {
		calibrator->showResults ();
	}

}

void loop () {
}