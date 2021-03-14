/*
 * Code File 
 */ 
#include "Arduino.h"
#include "Custom_PID.h"
 
PID::PID(float tempP, float tempI, float tempD) {
    kP = tempP; 
    kI = tempI; 
    kD = tempD; 
   } 
 
bool PID::Initialize() {
    currTime = millis(); 
    prevTime = currTime; 
 
    prevError = 0; 
     
    cP = 0; 
    cI = 0; 
    cD = 0; 
 
    return true; 
}
 
float PID::Update(float tempError, int tempDelay) {
  delay(tempDelay); 
 
  currTime = millis(); 
  deltaTime = currTime - prevTime; 
 
  deltaError = tempError - prevError; 
 
  cP = tempError; 
 
  cI += tempError*deltaTime; 
 
  if (deltaTime > 0) {
    cD = (deltaError/deltaTime); 
   } else {
    cD = 0; 
   } 
 
  prevTime = currTime; 
  prevError = tempError; 
 
  return ((kP*cP) + (kI*cI) + (kD*cD)); 
}