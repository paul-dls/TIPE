/*
 * Header File 
 */ 
#ifndef __CUSTOM_PID_H__ 
#define __CUSTOM_PID_H__ 
 
#include "Arduino.h" 
 
class PID {
public: 
  PID(float tempP, float tempI, float tempD); 
  bool Initialize(); 
  float Update(float tempError, int tempDelay); 
 
private: 
  float kP, kI, kD; 
  float cP, cI, cD; 
  float currTime, prevTime; 
  float prevError;
  float deltaTime, deltaError;
};
 
#endif