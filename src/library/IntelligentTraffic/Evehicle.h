#ifndef EVEHICLE_H
#define EVEHICLE_H

#include "Arduino.h"

class EVehicle {

public:
	EVehicle(int pin, String name, unsigned long interval);
	int getPinNumber();
	bool hasEVehicle();
	void resetState();
	void check(int state);
	static const int MAX_EVEHICLE_DELAY = 60000; //60 second
	void setState();
	unsigned long getPreviousStateTime();
	void resetStateTime();


private:
	int _pin;
	volatile byte _state;
	String _name;
	unsigned long _interval;
	unsigned long _previousInterval;
	unsigned long _previousStateTime;
};

#endif
