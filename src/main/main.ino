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
unsigned long delayTime = 1000; //1sec

// Light delays
unsigned long amberLightDelay = 2000; //2sec
unsigned long minimumGreenLight = 20000; //20 sec
unsigned long minimumNSGreenLight = 20000; //20 sec
unsigned long minimumEWGreenLight = 20000; //20 sec
unsigned long vehicleTime = 5000; //5s
unsigned long redLightDelay = 3000; //3 sec

int vehicleNSCounter = 0;   // counter for the number of vehicles in NS
int vehicleNSState = 0;        // current state of the vehicleNS button
int lastVehicleNSState = 0;    // previous state of the vehicleNS button
int maxVehicleNSCount = 20; //maximum sensor can read

int vehicleEWCounter = 0;   // counter for the number of vehicleNS in EW
int vehicleEWState = 0;        // current state of the vehicleNS button
int lastVehicleEWState = 0;    // previous state of the vehicleNS button
int maxVehicleEWCount = 20; //maximum sensor can read

// time since processing started
unsigned long timeSinceStart;
unsigned long previousTimeSinceStart;
// time to track the greenSignal
unsigned long timeWhenGreenSignal = 0;

unsigned long previousVehicleTime = 0;

enum trafficStates {
  NS_GREEN, // North South green
  NS_AMBER, // North South amber
  NS_RED, // North South red
  EW_GREEN, // East West green
  EW_AMBER, // East West amber
  EW_RED // East West red
};

enum trafficStates traffic = NS_GREEN;
enum trafficStates previousSignalState = NS_GREEN;

//FUNCTIONS

// Keep track of the number of vehicles in NS
void countNSVehicles() {
  if (maxVehicleNSCount > vehicleNSCounter) {
    if (vehicleNSState != lastVehicleNSState) {
      if (vehicleNSState == HIGH) {
        vehicleNSCounter = vehicleNSCounter + 1;
        Serial.print("North-South vehicle total count: ");
        Serial.println(vehicleNSCounter);
      }
    }
    lastVehicleNSState = vehicleNSState;
  } else {
    Serial.println("Stop pressing the push button NS sensor can't detect anymore vehicle");
  }
}

// Keep track of the number of vehicles in EW
void countEWVehicles() {
  if (maxVehicleEWCount > vehicleEWCounter) {
    if (vehicleEWState != lastVehicleEWState) {
      if (vehicleEWState == HIGH) {
        vehicleEWCounter = vehicleEWCounter + 1;
        Serial.print("East-West vehicle total count: ");
        Serial.println(vehicleEWCounter);
      }
    }
    lastVehicleEWState = vehicleEWState;
  } else {
    Serial.println("Stop pressing the push button EW sensor can't detect anymore vehicle");
  }
}

/**
   Passes vehicles
*/
void passVehicles() {
  //pass vehicles only after minimum vehicle time
  if (timeSinceStart - previousVehicleTime > vehicleTime) {
    //Pass any blocked vehicles in NS traffic
    if (traffic == NS_GREEN) {
      //when NS traffic cycle change traffic to north south
      if (vehicleNSCounter > 0 && (timeSinceStart - timeWhenGreenSignal > vehicleTime)) {
        vehicleNSCounter = vehicleNSCounter - 1;
        Serial.print("Vehicle NORTH-SOUTH passing. Total vehicles remainning: ");
        Serial.println(vehicleNSCounter);
        previousVehicleTime = timeSinceStart;
      }
    }

    if (traffic == EW_GREEN) {
      //when NS traffic cycle change traffic to north south
      if (vehicleEWCounter > 0 && (timeSinceStart - timeWhenGreenSignal > vehicleTime)) {
        vehicleEWCounter = vehicleEWCounter - 1;
        Serial.print("Vehicle EAST-WEST passing. Total vehicles remainning: ");
        Serial.println(vehicleEWCounter);
        previousVehicleTime = timeSinceStart;
      }
    }
  }
}

void sampleVehicles() {
  if (previousSignalState != traffic) {
    if (traffic == NS_GREEN && vehicleNSCounter < 8) {
      minimumNSGreenLight = minimumGreenLight;
      previousSignalState = NS_GREEN;
      Serial.print("NS_GREEN normal green signal time: ");
      Serial.println(minimumNSGreenLight/1000);
    }

    if (traffic == NS_GREEN && vehicleNSCounter >= 8) {
      minimumNSGreenLight = minimumGreenLight * (vehicleNSCounter / 4);
      previousSignalState = NS_GREEN;
      Serial.print("NS_GREEN calculated green signal time: ");
      Serial.println(minimumNSGreenLight/1000);
    }

    if (traffic == EW_GREEN && vehicleEWCounter < 8) {
      minimumEWGreenLight = minimumGreenLight;
      previousSignalState = EW_GREEN;
      Serial.print("EW_GREEN normal green signal time: ");
      Serial.println(minimumEWGreenLight/1000);
    }

    if (traffic == EW_GREEN && vehicleEWCounter >= 8) {
      minimumEWGreenLight = minimumGreenLight * (vehicleEWCounter / 4);
      previousSignalState = EW_GREEN;
      Serial.print("EW_GREEN calculated Green signal time: ");
      Serial.println(minimumEWGreenLight/1000);
    }
  }
}

//creates a couple timers that will fire repeatedly every x ms
TimedAction vehiclesNSThread = TimedAction(50, countNSVehicles); // checks for NS vehicle every 50 sec and count the total vehicle
TimedAction vehiclesEWThread = TimedAction(50, countEWVehicles); // checks for ES vehicle every 50 sec and count the total vehicle
TimedAction vehicleRemovalThread = TimedAction(1000, passVehicles); //a vehicle take minimum 5 to pass
TimedAction signalThread = TimedAction(5000, sampleVehicles); //samples vehicle at both end



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
  timeSinceStart = millis();

  switch (traffic) {
    case NS_GREEN:
      if (vehicleEWCounter > 0) {
        changeNSGreenToAmber();
      }
      break;
    case EW_GREEN:
      if (vehicleNSCounter > 0) {
        changeEWGreenToAmber();
      }
      break;
    case NS_AMBER:
      changeNSAmberToRed();
      break;
    case NS_RED:
      changeNSRedToEWGreen();
      break;
    case EW_AMBER:
      changeEWAmberToRed();
      break;
    case EW_RED:
      changeEWRedToNSGreen();
      break;
    default:
      break;
  }

  //check on our threads. based on how long the system has been
  //running, do they need to fire and do work? if so, do it!
  vehiclesNSThread.check();
  vehiclesEWThread.check();
  vehicleRemovalThread.check();
  signalThread.check();

  vehicleEWState = digitalRead(vehicleEW);
  vehicleNSState = digitalRead(vehicleNS);
}

void setInitialTrafficCondition() {
  //initial stage
  //NS green on, red and amber off
  if (traffic == NS_GREEN) {
    setEWTrafficLights(LOW, LOW, HIGH);
    setNSTrafficLights(HIGH, LOW, LOW);
  }
}

void changeNSGreenToAmber() {
  if (timeSinceStart - previousTimeSinceStart > minimumNSGreenLight) {
    Serial.println("Vehicle EAST-WEST direction. Changing signal EAST-WEST to green");

    //change NS to amber
    setNSTrafficLights(LOW, HIGH, LOW);

    traffic = NS_AMBER;
    previousTimeSinceStart = timeSinceStart;
  }
  else {
    //limit printing
    long remainingTime =  (minimumNSGreenLight - (timeSinceStart - previousTimeSinceStart)) / long(1000);
    if (remainingTime % 3 == 0) {      //if waiting for green light
      //if waiting for green light
      Serial.print("Time left for changing EAST WEST to green in : ");
      Serial.print(remainingTime);
      Serial.println("s");
    }
  }
}

void changeEWGreenToAmber() {
  if (timeSinceStart - previousTimeSinceStart > minimumEWGreenLight) {
    Serial.println("Vehicle NORTH-SOUTH direction. Changing signal NORTH-SOUTH to green");

    //change EW to amber
    setEWTrafficLights(LOW, HIGH, LOW);
    traffic = EW_AMBER;
    previousTimeSinceStart = timeSinceStart;
  } else {
    //limit printing
    long remainingTime =  (minimumEWGreenLight - (timeSinceStart - previousTimeSinceStart)) / long(1000);
    if (remainingTime % 3 == 0) {      //if waiting for green light
      Serial.print("Time left for changing NORTH SOUTH to green in : ");
      Serial.print(remainingTime);
      Serial.println("s");
    }
  }
}

void changeNSAmberToRed() {
  if (timeSinceStart - previousTimeSinceStart > amberLightDelay) {
    //Set NS to red
    setNSTrafficLights(LOW, LOW, HIGH);
    traffic = NS_RED;
    previousTimeSinceStart = timeSinceStart;
  }
}

void changeNSRedToEWGreen() {
  if (timeSinceStart - previousTimeSinceStart > redLightDelay) {
    //Set EW to green
    setEWTrafficLights(HIGH, LOW, LOW);
    timeWhenGreenSignal = timeSinceStart;
    Serial.println("EAST-WEST direction signal green");
    traffic = EW_GREEN;
    previousTimeSinceStart = timeSinceStart;
  }
}

void changeEWAmberToRed() {
  if (timeSinceStart - previousTimeSinceStart > amberLightDelay) {
    //Set EW to red
    setEWTrafficLights(LOW, LOW, HIGH);
    traffic = EW_RED;
    previousTimeSinceStart = timeSinceStart;
  }
}

void changeEWRedToNSGreen() {
  if (timeSinceStart - previousTimeSinceStart > redLightDelay) {
    //Set NS to green
    setNSTrafficLights(HIGH, LOW, LOW);
    timeWhenGreenSignal = timeSinceStart;
    Serial.println("NORTH-SOUTH direction signal green");
    traffic = NS_GREEN;
    previousTimeSinceStart = timeSinceStart;
  }
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

