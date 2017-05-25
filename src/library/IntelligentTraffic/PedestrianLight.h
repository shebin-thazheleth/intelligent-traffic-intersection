#ifndef PEDESTRIANLIGHT_H
#define PEDESTRIANLIGHT_H

#include "Arduino.h"
#include "Light.h"

class PedestrianLight {

public:
	PedestrianLight(int pin, String name);
	static const unsigned long ON_TIME_DELAY = 10000; //10 se
	static const unsigned long TURNON_DELAY = 2000; //2 se
	void turnOff();
	void turnOn();
	bool checkDelay(unsigned long delay);
	enum lightState getState();
	unsigned long getPreviousStateTime();
	void changeToON();
	void changeToOFF();
	void updatePreviousStateTime();
	unsigned long getTotalDelay();

private:
	int _pin;
	enum lightState _state;
	String _name;
	unsigned long _previousStateTime;
	unsigned long _onTime;
};

#endif

