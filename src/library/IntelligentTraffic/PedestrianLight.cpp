#include "PedestrianLight.h"
#include "Light.h"
#include "Arduino.h"

PedestrianLight::PedestrianLight(int pin, String name) {
	// Make all traffic light pins output
	pinMode(pin, OUTPUT);

	_state = lightState::OFF;
	_name = name;
	_pin = pin;
}

lightState PedestrianLight::getState() {
	return _state;
}

unsigned long PedestrianLight::getPreviousStateTime() {
	return _previousStateTime;
}

unsigned long PedestrianLight::getTotalDelay() {
	return ON_TIME_DELAY + TURNON_DELAY;
}

void PedestrianLight::turnOn() {
	digitalWrite(_pin, HIGH);
	_state = lightState::ON;
	Serial.print(_name);
	Serial.println(": signal is ON");
	updatePreviousStateTime();
}

void PedestrianLight::turnOff() {
	digitalWrite(_pin, LOW);
	_state = lightState::OFF;
	Serial.print(_name);
	Serial.println(": signal is OFF");
	updatePreviousStateTime();
}

void PedestrianLight::updatePreviousStateTime() {
	_previousStateTime = millis();
}

bool PedestrianLight::checkDelay(unsigned long minimumStateTime) {
	// check signal meets the minimum time requirements 
	return millis() - _previousStateTime >= minimumStateTime;
}

void PedestrianLight::changeToON() {
	if (checkDelay(TURNON_DELAY)) {
		turnOn();
	}
}

void PedestrianLight::changeToOFF() {
	if (checkDelay(ON_TIME_DELAY)) {
		turnOff();
	}
}