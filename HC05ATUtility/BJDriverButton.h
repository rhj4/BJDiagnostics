/*******************************************************************
*
*	Name:			BJDriverButton
*	Version:		1.0.0
*	Created On:		2/16/2019
*	Author:			Bob Jones <bob@rhj4.com>
*	Description:	This class implements access to various types
*	of buttons and provides four modes of operation for each button:
*
*		1)	IMMEDIATE: Read the button and return its value as soon as
*			the value changes;
*		2)	RAW: Read the button and return its value
*		3)	SIMPLE: Notify on button press and on button release
*		4)	TOGGLE: Toggle an internal switch after completion
*			of a button press and release.
*		5)	MULTI: Cycle through a specified number of states,
*			changingthe current state each time a button is pressed.
*
*
*********************************************************************/
#ifndef _BJDriverButton_h
#define _BJDriverButton_h


class BJDriverButton {
public:
	//	This value is used by the BUTTON_MODE_TOGGLE code
	//	to determine where we are in the process of toggling
	//	a button's switch value. It can have these states:
	enum TogglingState {
		TogglingStateInactive = 0,
		TogglingStateUnPressed = 1,
		TogglingStatePressed = 2,
		TogglingStateCompleted = 3,
	};

	//	A button can be pressed or released (unpressed).
	//	It's state can also be unknown. 
	enum ButtonState {
		ButtonStateUnknown = -1,
		ButtonStateReleased = 0,
		ButtonStatePressed = 1,
	};

	enum ButtonValueState {
		ButtonValueUndefined = 0,
		ButtonValueDefined = 1,
		ButtonValueChanged = 2,
	};

private:
	//	Name of this button - established by constructor
	char *_name;

	//	Digital pin assigned to button
	int _pin = 0;
	int _ledPin = 0;

	TogglingState _togglingState = TogglingStateInactive;

	ButtonState _currentButtonState = ButtonStateUnknown;
	ButtonState _previousButtonState = ButtonStateUnknown;

	ButtonValueState _buttonValueState = ButtonValueDefined;

	//	Assigned value of pin. Any positive value is OK.
	//	-1 indicates that the value of this button has changed.
	int _value = -1;

	//	mode of this button - established in configure method
	int _mode = 0;

	//	Number of states that a MULTI_STATE button can assume
	//	Only used for MODE_MULTI_STATE
	int _states = 0;

	//	If true lots of diagnistics will appear like fireflies
	//	on a Summer evening back East.
	bool _tracing = TRACING;

	//	Set to true on completion of "configured" method
	bool _configured = false;

	//	Array of module names - established at run time
	char *_modeName [ 5 ] = {};

	//	Array (configured in Constructor) of State Names
	char *_togglingStateName [ 4 ] = {};

	char *_buttonStateName [ 3 ] = {};

	unsigned long _lastDebounceTime = 0;
	unsigned long _debounceDelay = DEBOUNCE_DELAY;

	void _initializeModeNames () {
		_modeName [ BUTTON_MODE_IMMEDIATE ] = ( char* )"IMMEDIATE";
		_modeName [ BUTTON_MODE_RAW ] = ( char* )"RAW";
		_modeName [ BUTTON_MODE_SIMPLE ] = ( char* )"SIMPLE";
		_modeName [ BUTTON_MODE_TOGGLE ] = ( char* )"TOGGLE";
		_modeName [ BUTTON_MODE_MULTI_STATE ] = ( char* )"MULTI";
	}

	void _initializeButtonStates () {
		_togglingStateName [ 0 ] = ( char* ) "Inactive";
		_togglingStateName [ 1 ] = ( char* ) "UnPressed";
		_togglingStateName [ 2 ] = ( char* ) "Pressed";
		_togglingStateName [ 3 ] = ( char* ) "Completed";
	}

	void _initializeButtonStateNames () {
		_buttonStateName [ 0 ] = ( char* ) "Unknown";
		_buttonStateName [ 1 ] = ( char* ) "Released";
		_buttonStateName [ 2 ] = ( char* ) "Pressed";
	}

	void invalidValueExit ( char* badValueName, int badValue ) {
		Serial.print ( _name );
		Serial.print ( F ( " has invalid " ) );
		Serial.print ( badValueName );
		Serial.print ( F ( " value: [" ) );
		Serial.print ( badValue );
		Serial.println ( F ( "]" ) );

		//	Allow Serial Printing time to complete
		delay ( 1000 );

		//	Exit this program
		exit ( -1 );
	}

public:
	//	Constructor
	BJDriverButton ( char* name ) {
		_name = name;

		_initializeModeNames ();
		_initializeButtonStates ();

		Serial.print ( _name );
		Serial.print ( F ( "Created @" ) );
		Serial.println ( ( int ) this, HEX );
	}

	void configure ( int pin, int ledPin, int mode ) {
		configure ( pin, ledPin, mode, -1 );
	}

	void configure ( int pin, int ledPin, int mode, int states ) {
		if (pin < 1 || pin > 19) {
			invalidValueExit ( ( char* )"PIN", pin );
		}

		if (ledPin < 1 || ledPin > 19) {
			invalidValueExit ( ( char* )"LED PIN", pin );
		}

		if (mode < BUTTON_MODE_IMMEDIATE || mode > BUTTON_MODE_MULTI_STATE) {
			invalidValueExit ( ( char* )"MODE", mode );
		}

		if (mode == BUTTON_MODE_MULTI_STATE) {
			if (states < 3 || states > MAX_BUTTON_STATE) {
				invalidValueExit ( ( char* )"STATES", states );
			}
		}

		_pin = pin;
		_ledPin = ledPin;
		_mode = mode;
		_states = states;

		pinMode ( _pin, INPUT_PULLUP );
		pinMode ( _ledPin, OUTPUT );

		if (_tracing) {
			Serial.print ( _name );
			Serial.print ( getModeName () );
			if (_mode == BUTTON_MODE_MULTI_STATE) {
				Serial.print ( F ( ", STATES:" ) );
				Serial.print ( _states );
			}
			Serial.println ();
		}

		reset ();
		_configured = true;
	}

	//	Read the button's current state
	ButtonState readButton ( int _pin ) {
		_currentButtonState = ( digitalRead ( _pin ) == HIGH )
			? ButtonStatePressed
			: ButtonStateReleased;

		return _currentButtonState;
	}

	ButtonState readButton () {
		return readButton ( _pin );
	}

	/******************************************************************
	*
	*	GETTERS - return values of private variables
	*
	*******************************************************************/
	char* getName () {
		return _name;
	}

	int getPin () {
		return _pin;
	}

	int getLEDPin () {
		return _ledPin;
	}

	int getMode () {
		return _mode;
	}

	int getStates () {
		return _states;
	}

	TogglingState getTogglingState () {
		return _togglingState;
	}

	ButtonState getCurrentButtonState () {
		return _currentButtonState;
	}

	ButtonState getPreviousButtonState () {
		return _previousButtonState;
	}

	ButtonValueState getButtonValueState () {
		return _buttonValueState;
	}

	int getValue () {
		return _value;
	}

	void setValue ( int value ) {
		_value = value;
	}

	bool getTracing () {
		return _tracing;
	}

	void setTracing ( bool value ) {
		_tracing = value;
	}

	//	Reset button values to their initial state
	void reset () {
		_togglingState = TogglingStateInactive;
		_currentButtonState = ButtonStateUnknown;
		_previousButtonState = ButtonStateUnknown;
		_buttonValueState = ButtonValueUndefined;
	}

	//	Determine the mode of this button then update its
	//	current state and return the updated state value.
	ButtonValueState getButtonState () {

		switch (_mode) {
		case BUTTON_MODE_IMMEDIATE:
			return immediateButtonRead ();
			break;

		case BUTTON_MODE_RAW:
			return rawButtonRead ();
			break;

		case BUTTON_MODE_SIMPLE:
			return simpleButtonRead ();
			break;

		case BUTTON_MODE_TOGGLE:
			return toggleButtonRead ();
			break;

		case BUTTON_MODE_MULTI_STATE:
			return multiStateButtonRead ();
			break;

		default:
			Serial.print ( _name );
			Serial.println ( F ( " has unsupported mode" ) );
			delay ( 1000 );
			exit ( -1 );
		}

		return ButtonValueUndefined;
	}

	/******************************************************************
	*
	*	BUTTON MODE HANDLERS
	*
	*	Process a button through various states and determine its new
	*	value.
	*
	*******************************************************************/

	//	Return as soon as the button state changes
	//	This will take at least two cycles. The first
	//	establishes the baseline button state while
	//	the second returns if that state has changed.
	//	The return value will be -1 if the value is not
	//	defined yet, 0 or 1 for other 
	ButtonValueState immediateButtonRead () {
		//	Read the pin state
		_currentButtonState = readButton ();

		switch (_togglingState) {
		case TogglingStateInactive:
			if (_currentButtonState != _previousButtonState) {

				//	Remember the state we are in now
				_previousButtonState = _currentButtonState;

				//	Skip directly to Pressed state
				_togglingState = TogglingStatePressed;
			}
			break;

		case TogglingStateUnPressed:
			break;

		case TogglingStatePressed:
			//	See if the button state has changed
			if (_previousButtonState != _currentButtonState) {
				_value = ( _currentButtonState == ButtonStateReleased )
					? 1 : 0;

				_buttonValueState = ( _buttonValueState == ButtonValueUndefined )
					? ButtonValueDefined : ButtonValueChanged;

				_togglingState = TogglingStateCompleted;
			}
			break;

		case TogglingStateCompleted:
			reset ();
			break;
		}

		return _buttonValueState;
	}

	//	Return immediately when button state changes
	ButtonValueState rawButtonRead () {
		//	Read the pin state
		_currentButtonState = readButton ();

		switch (_togglingState) {
		case TogglingStateInactive:
			_previousButtonState = _currentButtonState;
			_togglingState = TogglingStateUnPressed;
			break;

		case TogglingStateUnPressed:
			if (_currentButtonState != _previousButtonState) {
				//	Move to the next state
				_togglingState = TogglingStatePressed;
			}
			break;

		case TogglingStatePressed:
			//	Button has been pressed or released
			_value = ( _currentButtonState == ButtonStatePressed ) ? 1 : 0;
			_togglingState = TogglingStateCompleted;
			break;

		case TogglingStateCompleted:
			reset ();
			break;
		}

		return _buttonValueState;
	}

	//	Wait for debounce and then return value
	ButtonValueState simpleButtonRead () {
		_currentButtonState = readButton ();

		switch (_togglingState) {
		case TogglingStateInactive:
			_previousButtonState = _currentButtonState;
			_togglingState = TogglingStateUnPressed;
			break;

		case TogglingStateUnPressed:
			if (_currentButtonState == HIGH) {
				//	Button has been pressed, so start tue timer
				//	so we can see if this is a "real" press or
				//	just noise.
				_lastDebounceTime = millis ();
				_togglingState = TogglingStatePressed;
			}
			break;

		case TogglingStatePressed:
			//	Check the timer. This will fall through until
			//	_debounceDelay millis have happened.
			if (( millis () - _lastDebounceTime ) > _debounceDelay) {
				if (_currentButtonState == ButtonStateReleased) {
					//	Change state of switch associated with
					//	this button:
					_value = ( _value > 0 ) ? 0 : 1;

					//	Indicate that we have a good result
					_buttonValueState = ButtonValueChanged;

					//	Reset to get ready for next time
					_togglingState = TogglingStateCompleted;
				}
			}

			break;

		case TogglingStateCompleted:
			reset ();
			break;
		}

		return _buttonValueState;
	}

	//	When the button pressed, we set _currentButtonState to 0.
	//	When it is released, we flip the_toggle value and
	//	set currentState to 1 to indicate that the state 
	//	has changed.
	ButtonValueState toggleButtonRead () {
		//	Read the button's current state:
		//	If pressed, _currentButtonState will be HIGH
		//	else, _currentButtonState will be LOW
		_currentButtonState = readButton ( _pin );

		switch (_togglingState) {
		case TogglingStateInactive:
			_togglingState = TogglingStateUnPressed;
			_previousButtonState = _currentButtonState;
			break;

		case TogglingStateUnPressed:
			if (_currentButtonState == HIGH) {
				//	Button has been pressed, so start tue timer
				//	so we can see if this is a "real" press or
				//	just noise.
				_lastDebounceTime = millis ();
				_togglingState = TogglingStatePressed;
			}
			break;

		case TogglingStatePressed:
			//	Check the timer. This will fall through until
			//	_debounceDelay millis have happened.
			if (( millis () - _lastDebounceTime ) > _debounceDelay) {
				if (_currentButtonState == LOW) {
					//	Change state of switch associated with
					//	this button:
					_value = ( _value > 0 ) ? 0 : 1;

					//	Indicate that we have a good result
					_currentButtonState = ButtonStateReleased;

					//	Reset to get ready for next time
					_togglingState = TogglingStateCompleted;
				}
			}
			break;

		case TogglingStateCompleted:
			reset ();
			break;
		}

		return _buttonValueState;
	}

	ButtonValueState multiStateButtonRead () {
		//	Read the button's current state:
		//	If pressed, _currentButtonState will be HIGH
		//	else, _currentButtonState will be LOW
		_currentButtonState = readButton ( _pin );

		switch (_togglingState) {

		case TogglingStateInactive:
			_previousButtonState = _currentButtonState;
			_togglingState = TogglingStateUnPressed;
			break;

		case TogglingStateUnPressed:
			//	State has changed
			if (_currentButtonState == _previousButtonState) {
				break;
			}

			_previousButtonState = _currentButtonState;

			if (_currentButtonState == HIGH) {
				//	Button has been pressed, so start tue timer
				//	so we can see if this is a "real" press or
				//	just noise.
				_lastDebounceTime = millis ();

				//	Prepare to move to the next level
				_togglingState = TogglingStatePressed;
				break;
			}

		case TogglingStatePressed:
			//	Check the timer. This will fall through until
			//	_debounceDelay millis have happened.
			if (( millis () - _lastDebounceTime ) > _debounceDelay) {
				if (_currentButtonState == LOW) {
					//	Change state of switch associated with	this button:
					_togglingState = TogglingStateCompleted;
					if (_value == -1) {
						_value = 0;
					} else if (_value++ >= ( _states - 1 )) {
						_value = 0;
					}
				}
			}

			break;

		case TogglingStateCompleted:
			reset ();
			break;

		}

		return _buttonValueState;
	}

	char* getStateName () {
		return getStateName ( _togglingState );
	}

	char* getStateName ( int state ) {
		switch (state) {
		case 0:
		case 1:
		case 2:
		case 3:
			return 	_togglingStateName [ state ];

		default:
			return NULL;
		}
	}

	char *getModeName () {
		return _modeName [ _mode ];
	}

	char *getButtonStateName ( ButtonState state ) {
		return _buttonStateName [ state ];
	}

#if TEST_SWITCHES

	/******************************************************************
	*
	*	Diagnostic tests to determine if each switch can recognize
	*	a press and release sequence.
	*
	*******************************************************************/
	bool testPress () {
		unsigned long counter = millis ();
		while (millis () - counter < TEST_MILLIS) {
			int state = digitalRead ( _pin );
			if (state == HIGH) {
				//	Button has been pressed
				return true;
			}
		}
		return false;
	}

	bool testRelease () {
		unsigned long counter = millis ();
		while (millis () - counter < TEST_MILLIS) {
			int state = digitalRead ( _pin );
			if (state == LOW) {
				//	Button has been pressed
				return true;
			}
		}
		return false;
	}
#endif /* TEST_SWITCHES */
};

#endif  /* _BJDriverButton_h */