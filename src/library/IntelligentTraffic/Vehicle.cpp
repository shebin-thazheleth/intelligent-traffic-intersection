#include "Vehicle.h"
#include "Arduino.h"

Vehicle::Vehicle(int pin, String name, unsigned long interval) {

	pinMode(pin, INPUT);

	_count = 0;
	_pin = pin;
	_state = 0;
	_name = name;
	_interval = interval;
	_previousInterval = 0;
	_lastState = 0;
}

int Vehicle::getCount() {
	return _count;
}

void Vehicle::setCount(int count) {
	_count = count;
}

int Vehicle::getPinNumber() {
	return _pin;
}

bool Vehicle::hasVehicle() {
	return _count > 0;
}

// Keep track of the number of vehicles in EW
void Vehicle::increment() {
	// sensor can detect only max amount of vehicles
	if (MAX_VEHICLE_COUNT > _count) {
		if (_state != _lastState) {
			if (_state == HIGH) {
				_count = _count + 1;
				Serial.print(_name);
				Serial.print(" vehicle total count: ");
				Serial.println(_count);
			}
		}
		_lastState = _state;
	}
	else {
		Serial.print(_name);
		Serial.println(" sensor can't detect anymore vehicle. Max limit reached");
	}
}

void Vehicle::check() {
	updateState();

	if (millis() - _previousInterval >= _interval) {
		_previousInterval = millis();
		increment();
	}
}

void Vehicle::updateState() {
	_state = digitalRead(_pin);
}

void Vehicle::setState(int state) {
	_state = state;
}
