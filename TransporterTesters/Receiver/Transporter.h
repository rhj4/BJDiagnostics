// Transporter.h

#ifndef _TRANSPORTER_h
#define _TRANSPORTER_h
#include "arduino.h"
#include "Config.h"

class Transporter
{
protected:
	 bool _tracing = TRACING;
	 char message [ MESSAGE_SIZE ] = {};
	 char dataBuffer [ DATA_BUFFER_SIZE ] = {};

public:
	 Transporter () {
#if BOARD_TYPE_MEGA
		 Serial.print ( F ( "MEGA " ) );
#else
		 Serial.print ( F ( "UNO / NANO " ) );
#endif 

#if PROTOCOL_WIFI
		 Serial.print ( F ( "WIFI " ) );
#elif PROTOCOL_BLUETOOTH
		 Serial.print ( F ( "BLUETOOTH " ) );
#elif PROTOCOL_WIRED
		 Serial.print ( F ( "WIRED " ) );
#endif
#if ROLE_IS_GETTER
		 Serial.print ( F ( "GETTER " ) );
#else
		 Serial.print ( F ( "PUTTER " ) );
#endif

		 Serial.println ( NAME );
	 }

	 virtual bool configure () = 0;
	 virtual bool getDataToSend () = 0;
	 virtual void sendDataBuffer () = 0;
	 virtual bool messageIsCompleted () = 0;
 	 virtual bool showStatusLED () = 0;

	 void clearDataBuffer () {
		 for (unsigned int i = 0; i < sizeof ( dataBuffer ); i++) {
			 dataBuffer [ i ] = 0x00;
		 }
	 }

	 void showDataBuffer ( char *dataBuffer, String label ) {
		 if (!_tracing) return;

		 Serial.print ( "dataBuffer: " );
		 Serial.print ( label );
		 Serial.print ( F ( " [" ) );
		 Serial.print ( dataBuffer );
		 Serial.println ( F ( "]" ) );
		 //	dumpString(dataBuffer, sizeof(dataBuffer));
	 }

	 void clearMessage () {
		 for (unsigned int i = 0; i < sizeof ( message ); i++) {
			 message [ i ] = 0x00;
		 }
	 }

	 void turnOn ( int pin, bool trace ) {
		 digitalWrite ( pin, HIGH );
		 if (trace && _tracing) {
			 Serial.print ( F ( "TURNING ON PIN " ));
			 Serial.println ( pin );
		 }
	 }

	 void turnOn ( int pin ) {
		 turnOn ( pin, true );
	 }

	 void turnOff ( int pin, bool trace ) {
		 digitalWrite ( pin, LOW );
		 if (trace && _tracing) {
			 Serial.print ( F ( "TURNING OFF PIN " ) );
			 Serial.println ( pin );
		 }
	 }

	 void turnOff ( int pin ) {
		 turnOff ( pin, true );
	 }

	 void flash ( int pin, unsigned int count, unsigned long duration ) {
		 for (unsigned int i = 0; i < count; i++) {
			 turnOn ( pin, false );
			 delay ( duration );
			 turnOff ( pin, false );
			 delay ( duration );
		 }
	 }

	 void flash ( int pin ) {
		 flash ( pin, FLASH_COUNT, FLASH_DURATION );
	 }
};


#endif

