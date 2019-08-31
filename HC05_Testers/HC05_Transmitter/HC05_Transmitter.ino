/*
    Name:       HC05_Transmitter.ino
    Created:	8/17/2018 4:32:52 PM
    Author:     THOR\bob
	Description:	
		This program works with HC05_Receiver to perform a simple
		test of HC05 over Software Serial.
*/
#include <SoftwareSerial.h>
SoftwareSerial driver(3, 4);
#define LED 9
#define BUFF_SIZE 20
char buffer[BUFF_SIZE] = {};
int index = 0;

void setup()
{
	Serial.begin(115200);
	driver.begin(9600);
	Serial.println(F("Enter any text in Serial Monitor"));
	clearBuffer();

	pinMode(LED, OUTPUT);
	digitalWrite(LED, HIGH);
	delay(2000);
	digitalWrite(LED, LOW);
}

void loop()
{
	if (Serial.available()) {
		digitalWrite(LED, HIGH);
		while (Serial.available()) {
			char data = Serial.read();
			if (data >= 0x20) {
				if (index == BUFF_SIZE -1) {
					writeBuffer();
					Serial.println();
				}
				buffer[index++] = data;
				buffer[index] = 0x00;
				Serial.print(data);
			}
		}
		writeBuffer();
		digitalWrite(LED, LOW);
	}

	delay(100);
}

void writeBuffer() {
	int len = driver.write(buffer);
	Serial.println();
	Serial.print(len);
	Serial.print(':');
	Serial.println(buffer);
	clearBuffer();
}

void clearBuffer() {
	for (int i = 0; i < BUFF_SIZE; i++) {
		buffer[i] = 0x00;
	}
	index = 0;
}