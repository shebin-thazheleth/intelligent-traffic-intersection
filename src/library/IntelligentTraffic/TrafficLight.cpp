#include "TrafficLight.h"
#include "Arduino.h"


TrafficLight::TrafficLight(int trafficLightPins[], trafficColor initialState, String name, unsigned long interval) {
	// Make all traffic light pins output
	for (int thisTrafficLight = 0; thisTrafficLight < 3; thisTrafficLight++) {
		pinMode(trafficLightPins[thisTrafficLight], OUTPUT);
		_trafficLightPins[thisTrafficLight] = trafficLightPins[thisTrafficLight];
	}

	updateLights(initialState);
	_name = name;
	_greenSignalTime = GREEN_LIGHT_DELAY;
}

trafficColor TrafficLight::getState() {
	return _state;
}

void TrafficLight::setGreenSignalTime(unsigned long time) {
	_greenSignalTime = time;
}

unsigned long TrafficLight::getGreenSignalTime() {
	return _greenSignalTime;
}

unsigned long TrafficLight::getPreviousStateTime() {
	return _previousStateTime;
}

void TrafficLight::updateLights(trafficColor color) {
	switch (color) {
	case trafficColor::GREEN:
		setTrafficGreen();
		break;
	case trafficColor::AMBER:
		setTrafficAmber();
		break;
	case trafficColor::RED:
		setTrafficRed();
		break;
	default:
		break;
	}
}

void TrafficLight::changeState() {
	switch (_state) {
	case trafficColor::GREEN:
		changeToAmber();
		break;
	case trafficColor::AMBER:
		changeToRed();
		break;
	case trafficColor::RED:
		changeToGreen();
		break;
	default:
		break;
	}
}

// sets the pin
void TrafficLight::setLights(uint8_t  greenValue, uint8_t amberValue, uint8_t redValue) {
	digitalWrite(_trafficLightPins[0], greenValue);
	digitalWrite(_trafficLightPins[1], amberValue);
	digitalWrite(_trafficLightPins[2], redValue);
}

void TrafficLight::setTrafficGreen() {
	setLights(HIGH, LOW, LOW);
	_state = trafficColor::GREEN;
	Serial.print(_name);
	Serial.println(": changed to Green");
	updateStateTime();
}

void TrafficLight::setTrafficAmber() {
	setLights(LOW, HIGH, LOW);
	_state = trafficColor::AMBER;
	updateStateTime();
}

void TrafficLight::setTrafficRed() {
	setLights(LOW, LOW, HIGH);
	_state = trafficColor::RED;
	updateStateTime();
}

void TrafficLight::updateStateTime() {
	_previousStateTime = millis();
}

bool TrafficLight::checkDelay(unsigned long minimumStateTime) {
	// check signal meets the minimum time requirements 
	return millis() - _previousStateTime >= minimumStateTime;
}

void TrafficLight::changeToAmber() {
	if (checkDelay(_greenSignalTime)) {
		setTrafficAmber();
	}
	else {
		//limit printing
		long remainingTime = (_greenSignalTime - (millis() - _previousStateTime)) / long(1000);
		if (remainingTime % 3 == 0) {      //if waiting for green light
			Serial.print("Time left for ");
			Serial.print(_name);
			Serial.print(" to chage ");
			Serial.print(remainingTime);
			Serial.println("s");
		}
	}
}

void TrafficLight::changeToRed() {
	if (checkDelay(AMBER_LIGHT_DELAY)) {
		setTrafficRed();
	}
}

void TrafficLight::changeToGreen() {
	if (checkDelay(RED_LIGHT_DELAY)) {
		setTrafficGreen();
	}
}