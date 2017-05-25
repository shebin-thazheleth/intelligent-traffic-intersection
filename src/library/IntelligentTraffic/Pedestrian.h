#ifndef PEDESTRIAN_H
#define PEDESTRIAN_H

#include "Arduino.h"

#define NO_PREDELAY 0

class Pedestrian {

public:
	Pedestrian(int pin, String name, unsigned long interval);

	void reset();
	void deactivate();
	void activate();
	void check();
	bool isActive();
	void updateState();
	void setInterval(unsigned long interval);
	static const unsigned long PEDESTRIAN_TIME = 10000; //10sec


private:
	bool _active;
	int _pin;
	unsigned long _previous;
	unsigned long _interval;
	String _name;
	int _state;
	int _lastState;
	unsigned long _deactiveTime;
};

#endif