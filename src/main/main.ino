//allows us to set actions to perform on separate timed intervals
//http://playground.arduino.cc/Code/TimedAction
#include <TimedAction.h>

const int trafficLightCount = 3;

// Pin 2 - green, pin 3 - amber, pin 4 - red.
int trafficNSLights[] = { 2, 3, 4 };

// Pin 5 - green, pin 6 - amber, pin 7 - red.
int trafficEWLights[] = { 5, 6, 7 };

// Pin for controlling traffic
const int vehicleEW = 12;
const int vehicleNS = 13;

// Light delay
int delayTime = 1000; //1sec

// Light delays
int amberLightDelay = 2000; //2sec
int greenLightDelay = 5000; //20sec for now 5 sec
int redLightDelay = 3000; //3 sec

//boolean to control state
boolean isNSTrafficGreen = false;

int vehicleNSCounter = 0;   // counter for the number of button presses
int vehicleNSState = 0;        // current state of the button
int lastVehicleNSState = 0;    // previous state of the button

int vehicleEWCounter = 0;   // counter for the number of button presses
int vehicleEWState = 0;        // current state of the button
int lastVehicleEWState = 0;    // previous state of the button

//FUNCTIONS

// Keep track of the number of vehicles in NS
void countNSVehicles() {
  if (vehicleNSState != lastVehicleNSState) {
    if (vehicleNSState == HIGH) {
      vehicleNSCounter = vehicleNSCounter + 1;
      Serial.print("North-South vehicle total count: ");
      Serial.println(vehicleNSCounter);
    }
  }
  lastVehicleNSState = vehicleNSState;
}

// Keep track of the number of vehicles in EW
void countEWVehicles() {
  if (vehicleEWState != lastVehicleEWState) {
    if (vehicleEWState == HIGH) {
      vehicleEWCounter = vehicleEWCounter + 1;
      Serial.print("East-West vehicle total count: ");
      Serial.println(vehicleEWCounter);
    }
  }
  lastVehicleEWState = vehicleEWState;
}

//our second task, fires every few seconds and rotates signals
void changeSignal() {
  //check current state. check if theres any vehicle in other side if so rotate signal
  if (isNSTrafficGreen && vehicleEWCounter > 0) {
    changeEWTrafficToGreen();
  } else if (!isNSTrafficGreen && vehicleNSCounter > 0) {
    changeNSTrafficToGreen();
  } else {
    //do nothing
  }
}

//create a couple timers that will fire repeatedly every x ms
TimedAction vehiclesNSThread = TimedAction(300, countNSVehicles);
TimedAction vehiclesEWThread = TimedAction(300, countEWVehicles);
TimedAction signalThread = TimedAction(5000, changeSignal); //minimum cycle green is 20s we use 5 for now
TimedAction vehicleRemovalThread = TimedAction(1000, passVehicles); //minimum 1 second for a vehicle to pass



// where's our third task? well, it's the loop itself :) the task
// which repeats most often should be used as the loop. other
// tasks are able to "interrupt" the fastest repeating task.


void setup() {
  // Put your setup code here, to run once:

  // Make all traffic light pins output
  for (int thisTrafficLight = 0; thisTrafficLight < trafficLightCount; thisTrafficLight++) {
    pinMode(trafficNSLights[thisTrafficLight], OUTPUT);
    pinMode(trafficEWLights[thisTrafficLight], OUTPUT);
  }

  // Input pins
  pinMode(vehicleEW, INPUT);

  Serial.begin(9600);
  setInitialTrafficCondition();
}

void loop() {
  // put your main code here, to run repeatedly:

  //check on our threads. based on how long the system has been
  //running, do they need to fire and do work? if so, do it!
  vehiclesNSThread.check();
  vehiclesEWThread.check();
  signalThread.check();
  vehicleRemovalThread.check();

  vehicleEWState = digitalRead(vehicleEW);
  vehicleNSState = digitalRead(vehicleNS);
}

void setInitialTrafficCondition() {
  //initial stage
  //NS green on, red and amber off
  if (!isNSTrafficGreen) {
    digitalWrite(trafficNSLights[0], HIGH);
    digitalWrite(trafficEWLights[2], HIGH);
    delay(greenLightDelay);
    isNSTrafficGreen = true;
  }
}

/**
   Passes vehicles
*/
void passVehicles() {
  //Pass any blocked vehicles in NS traffic
  if (isNSTrafficGreen) {
    //when NS traffic cycle change traffic to north south
    if (vehicleNSCounter > 0) {
      Serial.print("Total NORTH-SOUTH vehicles. ");
      Serial.println(vehicleNSCounter);
      vehicleNSCounter = vehicleNSCounter - 1;
      Serial.print("Vehicle NORTH-SOUTH passing. Total Number remainning");
      Serial.println(vehicleNSCounter);
    }
  } else {
    //when NS traffic cycle change traffic to north south
    if (vehicleEWCounter > 0) {
      Serial.print("Total EAST-WEST vehicles. ");
      Serial.println(vehicleEWCounter);
      vehicleEWCounter = vehicleEWCounter - 1;
      Serial.print("Vehicle EAST-WEST passing. Total Number remainning");
      Serial.println(vehicleEWCounter);
    }
  }
}

void changeEWTrafficToGreen() {
  Serial.println("Vechile EAST-WEST direction. Changing signal EAST-WEST to green");
  //Change NS to red
  setNSTrafficLights(LOW, HIGH, LOW);
  delay(amberLightDelay);
  setNSTrafficLights(LOW, LOW, HIGH);
  //Change EW to green
  delay(redLightDelay);
  setEWTrafficLights(HIGH, LOW, LOW);
  isNSTrafficGreen = false;
}

void changeNSTrafficToGreen() {
  Serial.println("Vechile NORTH-SOUTH direction. Changing signal NORTH-SOUTH to green");
  //Change EW to red
  setEWTrafficLights(LOW, HIGH, LOW);
  delay(amberLightDelay);
  setEWTrafficLights(LOW, LOW, HIGH);
  //Change NS to green
  delay(redLightDelay);
  setNSTrafficLights(HIGH, LOW, LOW);
  isNSTrafficGreen = true;
}

void setNSTrafficLights(uint8_t  greenValue, uint8_t amberValue, uint8_t redValue) {
  digitalWrite(trafficNSLights[0], greenValue);
  digitalWrite(trafficNSLights[1], amberValue);
  digitalWrite(trafficNSLights[2], redValue);
}

void setEWTrafficLights(uint8_t  greenValue, uint8_t amberValue, uint8_t redValue) {
  digitalWrite(trafficEWLights[0], greenValue);
  digitalWrite(trafficEWLights[1], amberValue);
  digitalWrite(trafficEWLights[2], redValue);
}

