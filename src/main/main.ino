//allows us to set actions to perform on separate timed intervals
//http://playground.arduino.cc/Code/TimedAction
#include <TimedAction.h>
#include <Pedestrian.h>
#include <Vehicle.h>
#include <TrafficLight.h>
#include <TrafficColor.h>
#include <PedestrianLight.h>
#include <Light.h>
#include <EVehicle.h>

const int trafficLightCount = 3;

// Pin 2 - green, pin 3 - amber, pin 4 - red.
int trafficNSLightPins[] = { 8, 9, 10 };

// Pin 5 - green, pin 6 - amber, pin 7 - red.
int trafficEWLightPins[] = { 11, 12, 13 };

// Pin for controlling traffic
int vehicleNSPin = 4;
int vehicleEWPin = 5;

// Pin for emergency vehicle detection external interruptable pins
const int eVehicleNSPin = 2;
const int eVehicleEWPin = 3;

volatile byte eVehicleNSState = 0;
volatile byte eVehicleEWState = 0;

// Due to lack of pins in ardunio uno temporary using pin 2 and 3 for pedestrian testing
const int pedNSPin = 0;
const int pedEWPin = 1;

// Pin for pedestrian crossing light
const int pedNSLightPin = 6;
const int pedEWLightPin = 7;

unsigned long vehicleTime = 5000; //5s
unsigned long previousVehicleTime = 0;

enum trafficStates {
  NS_GREEN, // North South green
  NS_AMBER,
  NS_PED_ON,
  NS_PED_OFF,
  NS_EVEHICLE,
  EW_EVEHICLE,
  EW_PED_OFF,
  NS_RED,
  EW_PED_ON, // North South pedestrian
  EW_GREEN, // East West green
  EW_AMBER,
  EW_RED
};

enum trafficStates traffic = NS_GREEN;
enum trafficStates previousSignalState = NS_GREEN;

String vehicleNSName = "VehicleNS";
String vehicleEWName = "VehicleEW";
Vehicle vehicleNS = Vehicle(vehicleNSPin, vehicleNSName, 50);
Vehicle vehicleEW = Vehicle(vehicleEWPin, vehicleEWName, 50);

String trafficNSName = "TrafficNS";
String trafficEWName = "TrafficEW";
TrafficLight trafficNS = TrafficLight(trafficNSLightPins, trafficColor::GREEN, trafficNSName, 50);
TrafficLight trafficEW = TrafficLight(trafficEWLightPins, trafficColor::RED, trafficEWName, 50);

String pedNSName = "pedNS";
String pedEWName = "pedEW";

Pedestrian pedNS = Pedestrian(pedNSPin, pedNSName, 100);
Pedestrian pedEW = Pedestrian(pedEWPin, pedEWName, 100);

String pedNSLightName = "pedNSLight";
String pedEWLightName = "pedEWLight";

PedestrianLight pedNSLight = PedestrianLight(pedNSLightPin, pedNSLightName);
PedestrianLight pedEWLight = PedestrianLight(pedEWLightPin, pedEWLightName);

String eVehicleNSName = "eVehicleNS";
String eVehicleEWName = "eVehicleEW";

EVehicle eVehicleNS = EVehicle(eVehicleNSPin, eVehicleNSName, 0); //check every second
EVehicle eVehicleEW = EVehicle(eVehicleEWPin, eVehicleEWName, 0);


/**
   Passes vehicles
*/
void passVehicles() {
  //pass vehicles only after minimum vehicle time
  if (millis() - previousVehicleTime > vehicleTime) {
    //Pass any blocked vehicles in NS traffic
    if (traffic == NS_GREEN || traffic == NS_PED_OFF || traffic == NS_PED_ON || traffic == NS_EVEHICLE) {
      //when NS traffic cycle change traffic to north south
      int count = vehicleNS.getCount();
      if (count > 0 && (millis() - trafficNS.getPreviousStateTime() > vehicleTime)) {
        vehicleNS.setCount(count - 1);
        Serial.print("Vehicle NORTH-SOUTH passing. Total vehicles remainning: ");
        Serial.println(vehicleNS.getCount());
        previousVehicleTime = millis();
      }
    }

    if (traffic == EW_GREEN || traffic == EW_PED_OFF || traffic == EW_PED_ON || traffic == EW_EVEHICLE) {
      int count = vehicleEW.getCount();
      //when NS traffic cycle change traffic to north south
      if (count > 0 && (millis() - trafficEW.getPreviousStateTime() > vehicleTime)) {
        vehicleEW.setCount(count - 1);
        Serial.print("Vehicle EAST-WEST passing. Total vehicles remainning: ");
        Serial.println(vehicleEW.getCount());
        previousVehicleTime = millis();
      }
    }
  }
}

void sampleVehicles() {
  // count for a traffic change
  if (previousSignalState != traffic && (traffic == NS_GREEN || traffic == EW_GREEN || traffic == NS_PED_ON || traffic == EW_PED_ON)) {

    if (traffic == NS_GREEN && vehicleNS.getCount() < 8) {
      unsigned long value = TrafficLight::GREEN_LIGHT_DELAY;

      if (previousSignalState == NS_PED_ON) {
        value = value - pedNSLight.getTotalDelay();
      }

      trafficNS.setGreenSignalTime(value);
      previousSignalState = NS_GREEN;
      Serial.print("NS_GREEN calculated green signal time needed to change: ");
      Serial.println(value / 1000);
    }

    if (traffic == NS_GREEN && vehicleNS.getCount() >= 8) {
      unsigned long value = TrafficLight::GREEN_LIGHT_DELAY;

      // use normal time to change traffic if emergency vehicle
      if (!eVehicleEW.hasEVehicle()) {
        value = TrafficLight::GREEN_LIGHT_DELAY * (vehicleNS.getCount() / 4);
      }

      trafficNS.setGreenSignalTime(value);
      previousSignalState = NS_GREEN;
      Serial.print("NS_GREEN calculated green signal time needed to change: ");
      Serial.println(value / 1000);
    }

    if (traffic == EW_GREEN && vehicleEW.getCount() < 8) {
      unsigned long value = TrafficLight::GREEN_LIGHT_DELAY;
      if (previousSignalState == EW_PED_ON) {
        value = value - pedEWLight.getTotalDelay();
      }
      trafficEW.setGreenSignalTime(value);
      previousSignalState = EW_GREEN;
      Serial.print("EW_GREEN calculated Green signal time needed to change: ");
      Serial.println(value / 1000);
    }

    if (traffic == EW_GREEN && vehicleEW.getCount() >= 8) {
      unsigned long value = TrafficLight::GREEN_LIGHT_DELAY;

      // use normal time to change traffic if emergency vehicle
      if (!eVehicleNS.hasEVehicle()) {
        value = TrafficLight::GREEN_LIGHT_DELAY  * (vehicleEW.getCount() / 4);
      }

      trafficEW.setGreenSignalTime(value);
      previousSignalState = EW_GREEN;
      Serial.print("EW_GREEN calculated Green signal time needed to change: ");
      Serial.println(value / 1000);
    }

    if (traffic == NS_PED_ON) {
      previousSignalState = NS_PED_ON;
    }

    if (traffic == EW_PED_ON) {
      previousSignalState = EW_PED_ON;
    }
  }

  //override initial state calculation if emergency vehicle
  if ((eVehicleNS.hasEVehicle() && trafficNS.getGreenSignalTime() > TrafficLight::GREEN_LIGHT_DELAY) || (eVehicleEW.hasEVehicle() && trafficEW.getGreenSignalTime() > TrafficLight::GREEN_LIGHT_DELAY) ) {
    if (eVehicleEW.hasEVehicle()) {
      trafficNS.setGreenSignalTime(TrafficLight::GREEN_LIGHT_DELAY);
    }

    if (eVehicleNS.hasEVehicle()) {
      trafficEW.setGreenSignalTime(TrafficLight::GREEN_LIGHT_DELAY);
    }

    Serial.println("Overrided calculated green signal due to emergency vehicle");
  }
}

// Remove serial after testing
void indicateEmergencyVehicleNS() {
  if (eVehicleNSState != 1) {
    Serial.print(eVehicleNSState);
    Serial.println(" : emergency vehicle NS INPUT");
    eVehicleNSState = 1;
  }
}

void indicateEmergencyVehicleEW() {
  if (eVehicleEWState != 1) {
    Serial.print(eVehicleEWState);
    Serial.println(" : emergency vehicle EW INPUT");
    eVehicleEWState = 1;
  }
}

//creates a couple timers that will fire repeatedly every x ms
TimedAction vehicleRemovalThread = TimedAction(1000, passVehicles); //a vehicle take minimum 5s to pass sampled at 1 sec
TimedAction signalThread = TimedAction(1000, sampleVehicles); // samples vehicle at 5s both end to get a optimised traffic green time


void setup() {
  attachInterrupt(digitalPinToInterrupt(eVehicleNSPin), indicateEmergencyVehicleNS, FALLING);
  attachInterrupt(digitalPinToInterrupt(eVehicleEWPin), indicateEmergencyVehicleEW, FALLING);

  // Put your setup code here, to run once:
  Serial.begin(9600); // Can't use pin 0 and 1 becuase its using serial
}

// put your main code here, to run repeatedly:
void loop() {

  checkState();

  // Checks for vehicles inputs every 50ms
  vehicleNS.check();
  vehicleEW.check();

  // Checks for vehicles inputs every 50ms
  pedNS.check();
  pedEW.check();

  // Sets the emergency vehicle internal states
  eVehicleNS.check(eVehicleNSState);
  eVehicleEW.check(eVehicleEWState);

  // Passes vehicle if right state
  vehicleRemovalThread.check();

  // Calculate the optimised green time
  signalThread.check();
}

void checkState() {
  switch (traffic) {
    case NS_GREEN:
      checkEmergencyVehicle();
      // north south green and detected vehicle in east west OR pedo other end
      if (vehicleEW.getCount() > 0 || pedEW.isActive() || eVehicleEW.hasEVehicle()) {
        //based on calculated green time
        trafficNS.changeToAmber();
        if (trafficNS.getState() == trafficColor::AMBER) {
          traffic = NS_AMBER;
        } else if (pedNS.isActive()) {
          //if traffic hasn't changed and 10s left before changing allow passenger to pass
          if (millis() - trafficNS.getPreviousStateTime() <= pedNSLight.getTotalDelay()) {
            if (pedNSLight.getState() == lightState::OFF) {
              pedNSLight.updatePreviousStateTime();
              traffic = NS_PED_OFF;
            }
          }
        } else {
        }
      } else {
        // meaning no vehicle opposite side and possibly a NS ped press
        if (pedNS.isActive()) {
          pedNSLight.updatePreviousStateTime();
          traffic = NS_PED_OFF; // extend timing in normal flow
        }
      }
      break;
    case EW_GREEN:
      checkEmergencyVehicle();
      if (vehicleNS.getCount() > 0 || pedNS.isActive() || eVehicleNS.hasEVehicle()) {
        trafficEW.changeToAmber();
        if (trafficEW.getState() == trafficColor::AMBER) {
          traffic = EW_AMBER;
        } else {
          if (pedEW.isActive()) {
            //if traffic hasn't changed and 10s left before changing allow passenger to pass
            if (millis() - trafficEW.getPreviousStateTime() <= pedEWLight.getTotalDelay()) {
              if (pedEWLight.getState() == lightState::OFF) {
                pedEWLight.updatePreviousStateTime();
                traffic = EW_PED_OFF;
              }
            }
          }
        }
      } else {
        // meaning no vehicle opposite side and possibly a EW ped press
        if (pedEW.isActive()) {
          pedEWLight.updatePreviousStateTime();
          traffic = EW_PED_OFF; // extend timing in normal flow
        }
      }
      break;
    case NS_AMBER:
      trafficNS.changeToRed();
      if (trafficNS.getState() == trafficColor::RED) {
        traffic = NS_RED;
      }
      break;
    case NS_RED:
      // only after 3 sec after changing in other side
      if (millis() - trafficNS.getPreviousStateTime() > TrafficLight::RED_LIGHT_DELAY) {
        trafficEW.changeToGreen();
        if (trafficEW.getState() == trafficColor::GREEN) {
          traffic = EW_GREEN;
        }
      }
      break;
    case EW_AMBER:
      trafficEW.changeToRed();
      if (trafficEW.getState() == trafficColor::RED ) {
        traffic = EW_RED;
      }
      break;
    case EW_RED:
      if (millis() - trafficEW.getPreviousStateTime() > TrafficLight::RED_LIGHT_DELAY) {
        trafficNS.changeToGreen();
        if (trafficNS.getState() == trafficColor::GREEN) {
          traffic = NS_GREEN;
        }
      }
      break;
    case NS_PED_OFF:
      if (pedNS.isActive()) {
        if (pedNSLight.getState() == lightState::OFF) {
          pedNSLight.changeToON();
        } else {
          traffic = NS_PED_ON;
        }
      }
      break;
    case NS_PED_ON:
      if (pedNS.isActive()) {
        if (pedNSLight.getState() == lightState::ON) {
          pedNSLight.changeToOFF();
        } else {
          pedNS.deactivate();
          traffic = NS_GREEN;
        }
      }
      break;
    case EW_PED_OFF:
      if (pedEW.isActive()) {
        if (pedEWLight.getState() == lightState::OFF) {
          pedEWLight.changeToON();
        } else {
          traffic = EW_PED_ON;
        }
      }
      break;
    case EW_PED_ON:
      if (pedEW.isActive()) {
        if (pedEWLight.getState() == lightState::ON) {
          pedEWLight.changeToOFF();
        } else {
          pedEW.deactivate();
          traffic = EW_GREEN;
        }
      }
      break;
    case NS_EVEHICLE:
      //stay in the same state for max vehicle delay
      //EVehicle::MAX_EVEHICLE_DELAY
      if (millis() - eVehicleNS.getPreviousStateTime() > 10000) {
        eVehicleNSState = 0;
        traffic = NS_GREEN;
        Serial.println("NORTH SOUTH EMERGENCY VEHICLE PASSED.");
      }
      break;
    case EW_EVEHICLE:
      //stay in the same state for max vehicle delay
      if (millis() - eVehicleEW.getPreviousStateTime() > 10000) {
        eVehicleEWState = 0;
        traffic = EW_GREEN;
        Serial.println("EAST WEST EMERGENCY VEHICLE PASSED.");
      }
      break;
    default:
      break;
  }
}

void checkEmergencyVehicle() {
  if (traffic == NS_GREEN) {
    if (eVehicleNS.hasEVehicle()) {
      Serial.println("NS_EMERGENCY_VEHICLE highest priority. NS traffic green for 1 min");
      eVehicleNS.resetStateTime();
      traffic = NS_EVEHICLE;
    }
  } else if (traffic == EW_GREEN) {
    if (eVehicleEW.hasEVehicle()) {
      Serial.println("EW_EMERGENCY_VEHICLE highest priority. EW traffic green for 1 min");
      eVehicleEW.resetStateTime();
      traffic = EW_EVEHICLE;
    }
  } else {
  }
}
