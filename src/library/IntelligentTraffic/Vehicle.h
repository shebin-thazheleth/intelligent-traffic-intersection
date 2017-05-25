#ifndef VEHICLE_H
#define VEHICLE_H

#include "Arduino.h"

class Vehicle {

public:
	Vehicle(int pin, String name, unsigned long interval);
	int getCount();
	int getPinNumber();
	bool hasVehicle();
	void setState(int state);
	void setCount(int count);
	void increment();
	void check();
	void updateState();
	static const int MAX_VEHICLE_COUNT = 20;


private:
	int _count;
	int _pin;
	int _state;
	int _lastState;
	String _name;
	unsigned long _interval;
	unsigned long _previousInterval;
};

#endif
