#pragma once
#include <RF24_config.h>
#include <RF24.h>
#include <printf.h>
#include <nRF24L01.h>
#include "Transporter.h"

class WiFi: public Transporter {
private:
	unsigned int packetsReceived = 0;
	unsigned int packetsSent = 0;
	unsigned int packetDataSent = 0;
	unsigned int packetDataReceived = 0;

	byte* listeningAddress;
	bool previousRadioStatus = false;
	bool _listening = false;
	bool _messageComplete = false;

	unsigned int messageIndex = 0;
	unsigned long lastPacketNumber = 0;

protected:

public:
	//	Constructor
	WiFi (): Transporter () {
	}
	//	Data structure that will be passed back and forth between
	//	Client and server.
#define COMMAND_SIZE	32 - sizeof(int) -sizeof(int) - sizeof(uint8_t)

	struct dataStruct {
		unsigned int packetNumber;			//	Used to find missing packets
		unsigned int length;				//	Length in bytes of command
		uint8_t subPacket;					//	Used with messages > 25 bytes long
		char command [ COMMAND_SIZE ] = {};	//	Command string to be executed
	} commandData;

	//	Enables tracking of missing packets
	unsigned int lastPacketNumberSent = 0;
	unsigned int dataSent = 0;

	//	Radio Declaration
	RF24 radio = RF24 ( WIFI_CE_PIN, WIFI_CSN_PIN );

	bool radioOK = false;

	byte transmitter [ 6 ] = WIFI_REMOTE_ADDRESS;
	byte receiver [ 6 ] = WIFI_ROBOT_ADDRESS;

	unsigned long receiveTimeout = WIFI_RECEIVE_TIMEOUT;
	unsigned int packetNumber = 0;
	uint8_t subPacketNumber = 0;

	bool configure () {
		radioOK = true;
		Serial.println ( F ( "\nInitializing WiFi Radio" ) );
		Serial.print ( F ( "CE PIN: " ) );
		Serial.print ( WIFI_CE_PIN );
		Serial.print ( F ( ", CSN PIN: " ) );
		Serial.println ( WIFI_CSN_PIN );

#if CONNECTED_ACTIVE_LED > 0
		pinMode ( CONNECTED_ACTIVE_LED, OUTPUT );
#endif

		if (!radio.begin ()) {
			Serial.println ( F ( "*** Radio BEGIN failed ***" ) );
			return false;
		}

		//	Set the transmit power to lowest available to prevent 
		//	power supply related issues
		Serial.print ( F ( "WIFI_POWER_LEVEL=" ) );
		Serial.println ( WIFI_POWER_LEVEL );
		Serial.print ( F ( "WIFI_DATA_RATE=" ) );
		Serial.println ( WIFI_DATA_RATE );
		Serial.print ( F ( "WIFI_DATA_CHANNEL=" ) );
		Serial.println ( WIFI_DATA_CHANNEL );

		radio.setPALevel ( WIFI_POWER_LEVEL );

		Serial.print ( F ( "Radio PA Level set to: " ) );
		Serial.println ( radio.getPALevel () );

		if (radio.getPALevel () != WIFI_POWER_LEVEL) {
			Serial.println ( F ( "Radio power level not set" ) );
			radioOK = false;
		}

		// Set the speed of the transmission 
		radio.setDataRate ( WIFI_DATA_RATE );
		Serial.print ( F ( "Radio Data Rate set to: " ) );
		Serial.println ( radio.getDataRate () );

		if (radio.getDataRate () != WIFI_DATA_RATE) {
			Serial.println ( F ( "Radio Data Rate not set" ) );
			radioOK = false;
		}

		// Use a channel unlikely to be used by Wifi, Microwave ovens etc
		radio.setChannel ( WIFI_DATA_CHANNEL );
		Serial.print ( F ( "Radio Channel set to: " ) );
		Serial.println ( radio.getChannel () );

		if (radio.getChannel () != WIFI_DATA_CHANNEL) {
			Serial.println ( F ( "Radio Data Channel not set" ) );
			radioOK = false;
		}

		if (!radioOK) {
			Serial.println ( F ( "Radio Initialization FAILED" ) );
			return false;
		}

		// Open a writing and reading pipe on each radio, with opposite addresses
		setPipes ();

		if (!checkRadioStatus ()) {
			return false;
		}

		startListening ();

		radioOK = showStatusLED ();

		return radioOK;
	}

	void setPipes () {
#if ROLE_IS_GETTER
		Serial.println ( F ( "Role is GETTER" ) );
		setWritingPipe ( receiver );
		setReadingPipe ( transmitter );
		listeningAddress = transmitter;
#elif ROLE_IS_PUTTER
		Serial.println ( F ( "Role is PUTTER" ) );
		setWritingPipe ( transmitter );
		setReadingPipe ( receiver );
		listeningAddress = receiver;
#endif /* ROLE_IS_ */
	}

	void setWritingPipe ( byte address [ 6 ] ) {
		radio.openWritingPipe ( address );
		showAddress ( "Writing Pipe", address );
		delay ( 10 );
	}

	void setReadingPipe ( byte address [ 6 ] ) {
		radio.openReadingPipe ( 1, address );
		showAddress ( "Reading Pipe", address );
	}

	void startListening () {
		//		if (listening) return;

		_listening = true;
		radio.startListening ();

		if (LISTENING_LED > 0) {
			turnOn ( LISTENING_LED, false );
		}

		if (_tracing) {
			showAddress ( "STARTED LISTENING ON ", listeningAddress );

			if (radio.available ()) {
				Serial.println ( F ( "DATA AVAILABLE" ) );
			}
		}
	}

	void stopListening () {
		if (_listening) {
			_listening = false;
		}
		radio.stopListening ();

		if (LISTENING_LED > 0) {
			turnOff ( LISTENING_LED, false );
		}

		if (_tracing) {
			showAddress ( "STOPPED LISTENING ON ", listeningAddress );
		}
	}

	bool checkRadioStatus () {
		bool currentStatus = radio.isValid ();
		if (!currentStatus) {
			Serial.println ( F ( "\n*** Radio is NOT VALID ***" ) );
			previousRadioStatus = false;
			return false;
		}

		if (currentStatus == previousRadioStatus) {
			return true;
		}

		previousRadioStatus = currentStatus;
		Serial.println ( "\nRadio Status OK" );

		radio.startListening ();
		return true;
	}

	bool showStatusLED () {
		bool valid = radio.isValid ();
		if (!valid) {
			Serial.println ( F ( "RADIO NOT VALID" ) );
			return false;
		}

		bool OK = valid && !radio.failureDetected;
		if (OK == previousRadioStatus) return OK;
		previousRadioStatus = OK;
		if (radioOK != OK) {
			radioOK = OK;
#if CONNECTED_ACTIVE_LED > 0
			Serial.print ( F ( "Radio Status: " ) );
			if (radioOK) {
				Serial.println ( F ( "OK" ) );
				turnOn ( CONNECTED_ACTIVE_LED, true );
			} else {
				Serial.println ( "NOT WORKING" );
				turnOff ( CONNECTED_ACTIVE_LED, true );
			}

			radioOK = false;
#endif
		}
		return radioOK;
	}

	void showAddresses () {
#if ROLE_IS_PUTTER
		showAddress ( "Transmitter", transmitter );
		showAddress ( "Receiver", receiver );
#else
		showAddress ( "Transmitter", receiver );
		showAddress ( "Receiver", transmitter );
#endif /* ROLE_IS_ */

	}

	void showAddress ( String label, byte address [ 6 ] ) {
		Serial.print ( label );
		Serial.print ( F ( " [" ) );
		for (unsigned int i = 0; i < 6; i++) {
			if (address [ i ] > 0x20) {
				Serial.print ( ( char ) address [ i ] );
			}
		}
		Serial.println ( F ( "]" ) );
	}

	//#if ROLE_IS_GETTER
	//		//	Attempt to retrieve command packets from aremote putter
	//		//	Aggregate all sub-packets into a single command string
	//		//	Display that command string on the Serial monitor
	//#elif ROLE_IS_PUTTER
	//		//	Acquire a command string from the Serial Monitor
	//		//	Format the string into one or more command packets
	//		//	Transmit the command packets to the remote getter
	//		//	Check for a response
	//		//	Display results
	//#endif /* ROLE_IS_ */

	bool dataAvailable () {
		return radio.available ();
	}

	bool getSentData () {
		// Go and read the data and put it into that variable
		while (radio.available ()) {
			radio.read ( &commandData, sizeof ( commandData ) );
			int len = strlen ( commandData.command );
			if (len == 0) {
				break;
			}

			if (_tracing) {
				showCommand ( "\nReceived: " );
			}

			//	Check for problems in the packet
			validatePacket ();

			//	Assemble the full message
			if (!appendToMessage ( commandData.command )) {
				Serial.println ( F ( "APPEND TO MESSAGE FAILED" ) );
				return false;
			}

			if (_tracing) {
				showCommand ( "Appending" );
			}

			_messageComplete = ( commandData.subPacket == 0xff );
			if (_messageComplete) {
				Serial.println ( F ( "Completed" ) );
			} else {
				Serial.println ( F ( "PARTIAL" ) );
			}


			showCommand ( "Packet: " );
			clearCommandData ();
#if WIFI_SEND_ACK
			// First, stop listening so we can talk
			stopListening ();

			//	Acknowledge command
			strcpy ( commandData.command, "ACK" );
			commandData.subPacket = 0x00;	//	Indicate acknowledgement
			commandData.length = strlen ( commandData.command );

			radio.write ( &commandData, sizeof ( commandData ) );

			// Now, resume listening so we catch the next packets.
			startListening ();

			if (_tracing) {
				Serial.print ( "Sent response [" );
				Serial.print ( commandData.command );
				Serial.println ( F ( "]" ) );
			}
#endif
		}

		return true;
	}

	void validatePacket () {
		if (strlen ( commandData.command ) == 0) return;

		if (strlen ( commandData.command ) != commandData.length) {
			Serial.print ( "\nERROR: commandData.length: " );
			Serial.print ( commandData.length );
			Serial.print ( ", strlen:" );
			Serial.println ( strlen ( commandData.command ) );
		}

		if (lastPacketNumber + 1 != commandData.packetNumber) {
			Serial.print ( "\nEXPECTING packet# " );
			Serial.print ( lastPacketNumber + 1 );
			Serial.print ( ", Received #" );
			Serial.println ( commandData.packetNumber );
		}

		lastPacketNumber = commandData.packetNumber;
	}

	bool appendToMessage ( char* data ) {
		unsigned int len = strlen ( data );
		if (len == 0) {
			return false;
		}

		for (unsigned int i = 0; i < len; i++) {
			if (strlen ( message ) < MESSAGE_SIZE) {
				message [ messageIndex++ ] = data [ i ];
				message [ messageIndex ] = 0x00;
			} else {
				Serial.print ( F ( "*** MESSAGE BUFER OVERFLOW ***" ) );
				return false;
			}
		}
		return true;
	}

	void showMessage () {
		if (strlen ( message ) > 0) {
			Serial.print ( F ( "Received Message: [" ) );
			Serial.print ( message );
			Serial.println ( F ( "]" ) );
			clearMessage ();
		}
	}

	void clearMessage () {
		messageIndex = 0;
		for (int i = 0; i < MESSAGE_SIZE; i++) {
			message [ i ] = 0x00;
		}
	}

	bool messageIsCompleted () {
		return _messageComplete;
	}

	//	Read data from the serial monitor into the data buffer
	bool getDataToSend () {
		if (!Serial.available ()) return false;

		clearDataBuffer ();
		byte data;
		unsigned int index = 0;
		while (Serial.available ()) {
			data = Serial.read ();
			if (data < 0x20) continue;
			if (index < ( DATA_BUFFER_SIZE - 1 )) {
				dataBuffer [ index++ ] = data;
				dataBuffer [ index ] = 0x00;
			} else {
				Serial.println ( F ( "\n*** TOO MUCH DATA ***" ) );
				dumpString ( dataBuffer, DATA_BUFFER_SIZE );
				clearDataBuffer ();
				return false;
			}
			delay ( 1 );
		}

		if (strlen ( dataBuffer ) == 0) return false;

		if (_tracing) {
			Serial.print ( F ( "\nSending " ) );
			Serial.print ( strlen ( dataBuffer ) );
			Serial.print ( F ( " bytes in " ) );
			showDataBuffer ( "" );
		}

		delay ( 10 );
		return true;
	}

	void sendDataBuffer () {
		// Ensure we have stopped listening (even if we're not) or we won't be able to transmit
		stopListening ();
		if (SENDING_LED > 0) {
			turnOn ( SENDING_LED, false );
		}

		prepareCommandData ();
		while (subPacketReady ()) {

			if (!radio.write ( &commandData, sizeof ( commandData ) )) {
				showCommand ( ( String )"Write failed" );
			} else {
				showCommand ( ( String )"Data Sent: " );
			}

#if WIFI_SEND_ACK
			// Now listen for a response
			startListening ();
			// But we won't listen for long, 200 milliseconds is enough
			unsigned long started_waiting_at = millis ();

			// Loop here until we get indication that some data is ready for us to read (or we time out)
			while (!radio.available ()) {

				// Oh dear, no response received within our timescale
				if (millis () - started_waiting_at > 500) {
					Serial.println ( "No response received - timeout!" );
					if (SENDING_LED > 0) {
						turnOff ( SENDING_LED, false );
					}

					return;
				}
			}

			// Now read the data that is waiting for us in the nRF24L01's buffer
			radio.read ( &commandData, sizeof ( commandData ) );
			showCommand ( ( String )"Data Received: " );
			if (SENDING_LED > 0) {
				turnOff ( SENDING_LED, false );
			}
//			startListening ();
#endif
		}

		//	Get ready for next command
		prepareCommandData ();
		startListening ();
	}

	//	If the data to be sent is larger than the size of
	//	commandData.command, then break the data into packets
	bool subPacketReady () {
		//	see if data buffer is empty
		if (strlen ( dataBuffer ) == 0) {
			return false;
		}

		commandData.packetNumber = ++packetNumber;
		commandData.subPacket = 1;

		//	If contents of data buffer will fit into one packet
		//	then send it and clear the data buffer.
		if (strlen ( dataBuffer ) < sizeof ( commandData.command )) {
			strcpy ( commandData.command, dataBuffer );
			commandData.length = strlen ( commandData.command );
			commandData.subPacket = 0xFF;
			clearDataBuffer ( );
			return true;
		}

		//	Get command data
		for (unsigned int i = 0; i < sizeof ( commandData.command ) - 1; i++) {
			commandData.command [ i ] = dataBuffer [ i ];
			commandData.command [ i + 1 ] = 0x00; // Terminate the string
		}

		commandData.length = strlen ( commandData.command );
		dataSent = strlen ( commandData.command );

		//	Compress the data buffer by sliding bytes to the
		//	beginning of the buffer
		for (unsigned int i = 0; i < sizeof ( dataBuffer ); i++) {
			dataBuffer [ i ] = dataBuffer [ dataSent++ ];
			if (dataSent > sizeof ( dataBuffer )) break;
		}

		commandData.subPacket = ++subPacketNumber;

		return true;
	}

	void clearCommandData () {
		//	clear the command 
		for (unsigned int i = 0; i < sizeof ( commandData.command ); i++) {
			commandData.command [ i ] = 0x00;
		}
	}

	void prepareCommandData () {
		clearCommandData ();
		commandData.length = 0;
		commandData.subPacket = 0;
		commandData.packetNumber = 0;

		subPacketNumber = 0;
	}

	void showCommandData () {
		if (!_tracing) return;

		// Show user what we sent and what we got back
		Serial.print ( "Packet #:" );
		Serial.print ( commandData.packetNumber );
		Serial.print ( ", subPacket#:" );
		Serial.print ( commandData.subPacket );
		Serial.print ( ", length:" );
		Serial.print ( commandData.length );
		Serial.print ( "\ndata: [" );
		Serial.print ( commandData.command );
		Serial.println ( "]" );
	}

	void showCommand ( String label ) {
		if (!_tracing) return;

		// Show user what we sent and what we got back
		Serial.print ( label );
		Serial.print ( F ( " packet #: " ) );
		Serial.print ( commandData.packetNumber );
		Serial.print ( F ( ", subPacket#:" ) );
		Serial.print ( commandData.subPacket );
		Serial.print ( F ( ", Length: " ) );
		Serial.print ( commandData.length );
		Serial.print ( F ( ", data:\n[" ) );
		Serial.print ( commandData.command );
		Serial.println ( F ( "]" ) );
	}

	void dumpString ( String buffer, unsigned int size ) {
		if (!_tracing) return;

		unsigned int col0 = 0;
		char letters [ COLS + 1 ] = {};
		for (unsigned int i = 1; i <= size; i++) {
			printHexByte ( buffer [ i - 1 ] );
			if (( i >= COLS && i % COLS == 0 ) || ( i > size - 1 )) {
				Serial.print ( '\t' );
				unsigned int k = 0;
				for (unsigned int j = col0; j < col0 + COLS; j++) {
					letters [ k++ ] = ( buffer [ j ] < 0x20 ) ? '.' : ( char ) buffer [ j ];
				}
				letters [ COLS ] = 0;
				col0 = i + 1;
				Serial.println ( letters );
			} else {
				Serial.print ( ' ' );
			}
		}
		Serial.println ();
	}

	void printHexByte ( byte b ) {
		if (b < 0X10) Serial.print ( '0' );
		Serial.print ( b, HEX );
		Serial.print ( ' ' );
	}

	void showInfo () {
		Serial.println ( F ( " RADIO CONFIGURATION:" ) );

		Serial.print ( F ( "Power Level: " ) );
		Serial.println ( radio.getPALevel () );

		Serial.print ( F ( "Data Rate: " ) );
		Serial.println ( radio.getDataRate () );

		Serial.print ( F ( "Channel: " ) );
		Serial.println ( radio.getChannel () );

		Serial.print ( F ( "Payload Size: " ) );
		Serial.println ( radio.getPayloadSize () );

		showAddresses ();

#if WIFI_SEND_ACK
		Serial.println ( F ( "ACK Enabled" ) );
#else
		Serial.println ( F ( "ACK Disabled" ) );
#endif

		checkRadioStatus ();
	}
};