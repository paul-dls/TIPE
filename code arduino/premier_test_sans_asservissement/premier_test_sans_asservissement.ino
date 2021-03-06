
/*
 Stepper Motor Control - one revolution

 This program drives a unipolar or bipolar stepper motor.
 The motor is attached to digital pins 8 - 11 of the Arduino.

 The motor should revolve one revolution in one direction, then
 one revolution in the other direction.


 Created 11 Mar. 2007
 Modified 30 Nov. 2009
 by Tom Igoe

 */
#include <Servo.h>
const int pinMot =6;
Servo moteur;
#include <Stepper.h>

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

void setup() {

  moteur.attach(pinMot,100,200);
  // set the speed at 60 rpm:
  myStepper.setSpeed(20);
  // initialize the serial port:
  Serial.begin(9600);
  //myStepper.step(stepsPerRevolution/2);
  delay(1500);

  moteur.write(120);
  delay(2500);
  moteur.write(130);
  
  
}

void loop() {
  // step one revolution  in one direction:
  Serial.println("clockwise");
  myStepper.step(stepsPerRevolution/6);
  delay(500);

  // step one revolution in the other direction:
  Serial.println("counterclockwise");
  myStepper.step(-stepsPerRevolution/6);
  delay(500);

}
