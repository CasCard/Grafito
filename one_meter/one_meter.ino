#include <AccelStepper.h>

const int pulsePin = 15;
const int dirPin = 2;
const float stepsPerMM = 1000; // adjust this value based on your specific motor and setup
const int maxSpeed = 5000;
const int acceleration = 1500;
const int startSpeed = 5000;

AccelStepper stepper(1, pulsePin, dirPin);

void setup() {
  stepper.setMaxSpeed(maxSpeed);
  stepper.setAcceleration(acceleration);
  stepper.setSpeed(startSpeed);
}

void loop() {
  // move 1000mm forward
  stepper.move(100*stepsPerMM);
  stepper.run();

  // wait until the motor has stopped
  while (stepper.distanceToGo() != 0) {
    stepper.run();
  }

  // move 1000mm backward
  stepper.move(-100*stepsPerMM);
  stepper.run();

  // wait until the motor has stopped
  while (stepper.distanceToGo() != 0) {
    stepper.run();
  }
}
