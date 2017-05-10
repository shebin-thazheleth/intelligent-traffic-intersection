int trafficLightCount = 3;

// Contains array of traffic led pins. Pin 0 - green, pin 1 - amber, pin 3 - red.
int trafficLights[] = { 0, 1, 2 };

// Pin for controlling traffic
int trafficController = 13;

// Light delay
int delayTime = 1000; //1sec

void setup() {
  // Put your setup code here, to run once:

  // Make all traffic light pins output
  for (int thisTrafficLight = 0; thisTrafficLight < trafficLightCount; thisTrafficLight++) {
    pinMode(trafficLights[thisTrafficLight], OUTPUT);
  }

  // Input pins
  pinMode(trafficController, INPUT);
}

void loop() {
    // put your main code here, to run repeatedly:
    for (int thisTrafficLight = 0; thisTrafficLight < trafficLightCount; thisTrafficLight++) {
      digitalWrite(trafficLights[thisTrafficLight], HIGH);
      delay(delayTime);
      digitalWrite(trafficLights[thisTrafficLight], LOW);
      delay(delayTime);
    }
}
