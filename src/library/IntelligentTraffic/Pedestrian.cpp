#include "Pedestrian.h"

Pedestrian::Pedestrian(int pin, String name, unsigned long interval) {
	//pinMode(pin, INPUT); ToDo turn on after changing to bigger microntroller
	_previous = 0;
	_interval = interval;
	_pin = pin;
	_name = name;
	_state = 0;
	_lastState = 0;
	_active = false;
	_deactiveTime = 0;
}

bool Pedestrian::isActive() {
	//todo change after changing to bigger micro
	return false;
}

void Pedestrian::activate() {
	// once active 
	if (!_active) {
		if (_state != _lastState) {
			if (_state == HIGH) {
				_active = true;
				Serial.print(_name);
				Serial.println(" : activated");
			}
		}
		_lastState = _state;
	}
	//else {
		//Serial.print(_name);
		//Serial.println(" already activated waiting..");
	//}
}

void Pedestrian::reset() {
	_previous = millis();
}

void Pedestrian::deactivate() {
	_active = false;
	_deactiveTime = millis();
}

void Pedestrian::check() {
	updateState();

	if (millis() - _previous >= _interval) {
		_previous = millis();
		activate();
	}
}

//void Pedestrian::pass() {
	//if (millis() - _previous >= PEDESTRIAN_TIME) {
		//_previous = millis();
		//deactive();
	//}
//}

void Pedestrian::updateState() {
	_state = digitalRead(_pin);
}

void Pedestrian::setInterval(unsigned long intervl) {
	_interval = intervl;
}