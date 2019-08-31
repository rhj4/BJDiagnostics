/*
    Name:       BJBufferTester.ino
    Created:	5/26/2018 2:47:08 PM
    Author:     THOR\bob

	Desc:		Dynamically allocate a buffer, add characters and strings
				to the buffer, show the contents of the buffer and its size
				and current length.

				Add markers at the begining and end of the buffer to make
				sure that the memory doesn't get corrupted.
*/

#include <BJMemory.h>
#include <BJBufferManager.h>

BJBufferManager *biff;	// Biff, the buffer manager...
BJMemory *memory;

void setup()
{
	Serial.begin(115200);

	biff = new BJBufferManager ( BUFFER_SIZE );
	memory = new BJMemory;
	memory->showAvailable();

	//	First test - allocate the buffer and make sure it is valid
	//	(i.e. the markers on either end of the buffer are in tact.
	Serial.print("Allocating Buffer...");
	if (!biff->isOK()) {
		Serial.println(F("*** BUFFER IS CORRUPTED ***"));
	}

	Serial.println(biff->isAllocated() ? "SUCCESS" : "FAILURE");

	memory->showAvailable();

	Serial.print("Len: "); Serial.println(biff->len());

#if INCLUDE_BUFFER_MANAGER_TESTS
	biff->test1();
	memory->showAvailable();

	biff->test2();
	memory->showAvailable();

	biff->~BJBufferManager();
	memory->showAvailable();

	Serial.println(F("Recreating Buffer Manager"));
	biff = new BJBufferManager;
	if (!biff->isOK()) {
		Serial.println(F("*** BUFFER IS CORRUPTED ***"));
	}
	memory->showAvailable();
	biff->begin(BUFFER_SIZE * 2);
	memory->showAvailable();
	biff->test1();
	biff->test2();
	memory->showAvailable();
	if (!biff->isOK()) {
		Serial.println(F("*** BUFFER IS CORRUPTED ***"));
	}

#endif
}

void loop()
{
}
