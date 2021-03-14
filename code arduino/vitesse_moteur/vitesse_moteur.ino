#include <Servo.h>
const int pinMot =6;
Servo moteur;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //while(!Serial){;} 
  moteur.attach(pinMot,100,200);
}

void loop() {
  // put your main code here, to run repeatedly:
  int i=0;
  for(int i=0; i<100 ; i++){
    moteur.write(i);
    Serial.println(i);
    delay(80);
  }
  delay(5000);
  for(int i=100; i>0 ; i--){
    moteur.write(i);
    Serial.println(i);
    delay(80);
  }
  delay(50);
}
