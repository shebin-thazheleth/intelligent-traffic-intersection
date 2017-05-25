#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include "Arduino.h"
#include "TrafficColor.h"

#define NO_PREDELAY 0

class TrafficLight {

public:
	TrafficLight(int trafficLightPins[], trafficColor initialState, String name, unsigned long interval);
	void setLights(uint8_t  greenValue, uint8_t amberValue, uint8_t redValue);
	void updateLights(trafficColor color);
	static const unsigned long AMBER_LIGHT_DELAY = 2000; //2sec
	static const unsigned long GREEN_LIGHT_DELAY = 20000; //20 sec
	static const unsigned long RED_LIGHT_DELAY = 3000; //3 sec
	void setTrafficGreen();
	void setTrafficAmber();
	void setTrafficRed();
	void changeToGreen();
	void changeToAmber();
	void changeToRed();
	bool checkDelay(unsigned long delay);
	void changeState();
	trafficColor getState();
	void setGreenSignalTime(unsigned long time);
	unsigned long  getGreenSignalTime();
	unsigned long getPreviousStateTime();

private:
	int _trafficLightPins[3];
	enum trafficColor _state;
	String _name;
	unsigned long _previousStateTime;
	void updateStateTime();
	unsigned long _greenSignalTime;
};

#endif
