#define POT1_PIN	A0
#define POT2_PIN	A1
#define POT3_PIN	A2
#define DESCRIPTION "Potentiometer Test" 
#define VERSION		"V1.0" 

uint16_t value;

void setup () {
	Serial.begin ( 115200 );
	while (!Serial.availableForWrite ()) {}

	Serial.print ( F ( DESCRIPTION ) );
	Serial.println ( F ( VERSION ) );

	pinMode ( POT1_PIN, INPUT );
	pinMode ( POT2_PIN, INPUT );
	pinMode ( POT3_PIN, INPUT );
}

void loop () {
	checkPot (POT1_PIN);
	checkPot ( POT2_PIN );
	checkPot ( POT3_PIN );
	delay ( 100 );
}

void checkPot (uint8_t pin ) {
	value = analogRead ( pin );
	Serial.print ( F ( "PIN: " ) );
	Serial.print ( pin );
	Serial.print ( '=' );
	Serial.println ( value );
}