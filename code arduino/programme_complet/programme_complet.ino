/*
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * le est stepper sur les pins 8 a 11
 * 
 * ce programme reprend des parties des exemples des librairies autoPID, Stepper
 * 
 * créé le 6 mars 2021
 * par Paul de La Sayette
 * 
 */
//librairies
#include <Servo.h>
#include <Stepper.h>
#include <AutoPID.h>

// pins
#define POT_PIN A0
#define LED_PIN 13
#define pinMot 6

// constantes du correcteur
#define pi 3.1416
#define stepsPerRevolution 200
#define OUTPUT_MIN -double(50)/60*2*pi
#define OUTPUT_MAX double(50)/60*2*pi
#define KP 3
#define KI 0
#define KD 0
#define periode 1000

// variables golbales
double mesure; // de 0 a 1023, (la librairie utilisé ne fonctionne qu'avec des doubles)
double commande; // de 0 a 1023
double output; // de 0 a 1023

//objets
Servo moteur;
//AutoPID myPID(&mesure, &commande, &output, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);



void setup() {
  Serial.begin(9600);
  //while (!Serial) {
  //  ; // wait for serial port to connect. Needed for native USB port only
  //}
  Serial.println("initialisation du port série");
  
  moteur.attach(pinMot,100,200);
  myStepper.setSpeed(10);
  //myPID.setTimeStep(periode);
  pinMode(LED_PIN, OUTPUT);
  delay(500);

  //initialisation du moteur
  moteur.write(120); // on place l'ESC a 0 pour enlever la securité, j'utilise un esc de voiture télécommandé, son 0 n'est donc pas à 100
  for(int i=120; i<140 ; i++){
    moteur.write(i);
    Serial.println(i);
    delay(40);
  }
  moteur.write(140);
  delay(1000);
  Serial.println("moteur lancé");
  //myPID.setBangBang(3);

  commande = double(analogRead(POT_PIN))/1023*2*pi;
  Serial.println("commande :" + String(commande));
}

void loop() {
  
  //on attend un peu
  
  double mesure = double(analogRead(POT_PIN))/1023*2*pi;


  //myPID.run();
  
  Serial.print ("temps :" +String(millis()) + ", ");
  Serial.print ("mesure:" +String(mesure) + ", ");
  //Serial.print ("intégrale:" +String(myPID.getIntegral())+ ", ");

  //digitalWrite(LED_PIN, myPID.atSetPoint(0.1));
  
  output = KP* (mesure-commande);
  Serial.print ("output:" +String(output) + ", ");


  //output est en radian par seconde, motorspeed en rpm
  //double motorSpeed = map(output, 0, 2*pi, 0, 60);
  int motorSpeed = output/(2*pi)*60;
  if (motorSpeed > 0) {
    // on met a jour la vitesse:
    myStepper.setSpeed(motorSpeed);
    // step 1/100 of a revolution:
    myStepper.step(stepsPerRevolution / 100);
  }
  else if (motorSpeed < 0) {
    // on met a jour la vitesse:
    myStepper.setSpeed(- motorSpeed);
    // step 1/100 of a revolution:
    myStepper.step(- stepsPerRevolution / 100);
  }

  Serial.print ("commande :" +String(commande) + ", ");
  Serial.print ("output:" +String(output) + ", ");
  Serial.println ("motorSpeed:" +String(motorSpeed) );
}
