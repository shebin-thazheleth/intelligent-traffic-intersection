#include "EVehicle.h"
#include "Arduino.h"

EVehicle::EVehicle(int pin, String name, unsigned long interval) {

	pinMode(pin, INPUT);
	_pin = pin;
	_state = 0;
	_name = name;
	_interval = interval;
	_previousInterval = 0;
	_previousStateTime = 0;
}

void EVehicle::setState() {
	if (_state != 1) {
		Serial.print(_name);
		Serial.println(": EMERGENCY VEHICLE DETECTED!!!");
		_state = 1;
		_previousStateTime = millis();
	}
}

int EVehicle::getPinNumber() {
	return _pin;
}

bool EVehicle::hasEVehicle() {
	return _state == 1;
}

// check the status of emergency vehicle every interval
void EVehicle::check(int state) {
	if (millis() - _previousInterval >= _interval) {
		_previousInterval = millis();
		if (state == 1) {
			setState();
		}
		else {
			resetState();
		}
	}
}

void EVehicle::resetState() {
	_state = 0;
}

void EVehicle::resetStateTime() {
	_previousStateTime = millis();
}

unsigned long EVehicle::getPreviousStateTime() {
	return _previousStateTime;
}


