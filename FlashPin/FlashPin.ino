int fromPin = 4;
int toPin = 11;
int delayMS = 400;

void setup () {
	for (int i = fromPin; i <= toPin; i++) {
		pinMode ( i, OUTPUT );
	}
}

void loop () {
	for (int i = fromPin; i <= toPin; i++) {
		Serial.println ( i );
		digitalWrite ( i, HIGH );
		delay ( delayMS );
		digitalWrite ( i, LOW );
		delay ( delayMS  * 2 );
	}
}