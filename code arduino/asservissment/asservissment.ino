
/*
   AutoPID BasicTempControl Example Sketch

   This program reads a dallas temperature probe as input, potentiometer as setpoint, drives an analog output.
   It lights an LED when the temperature has reached the setpoint.
*/
#include <AutoPID.h>

//pins
#define SET_PIN A1
#define OUTPUT_PIN A1
#define POT_PIN A0
#define LED_PIN 13

#define DELAY 10 

//pid settings and gains
#define OUTPUT_MIN 0
#define OUTPUT_MAX 255
#define KP .12
#define KI .0003
#define KD 0

double angleMes, setPoint, outputVal;


//input/output variables passed by reference, so they are updated automatically
AutoPID myPID(&angleMes, &setPoint, &outputVal, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);

unsigned long lastAngleUpdate; //tracks clock time of last temp update

//call repeatedly in loop, only updates after a certain time interval
//returns true if update happened
bool updateAngle() {
  if ((millis() - lastAngleUpdate) > DELAY) {
    angleMes=analogRead(POT_PIN);
    setPoint=analogRead(SET_PIN);
    lastAngleUpdate = millis();
    return true;
  }
  return false;
}//void updateTemperature


void setup() {
  pinMode(OUTPUT_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  //set PID update interval to 50ms
  myPID.setTimeStep(50);

}//void setup


void loop() {
  updateAngle();
  setPoint = analogRead(SET_PIN);
  myPID.run(); //call every loop, updates automatically at certain time interval
  analogWrite(OUTPUT_PIN, outputVal);
  digitalWrite(LED_PIN, myPID.atSetPoint(1)); //light up LED when we're at setpoint +-1 degree

}//void loop
