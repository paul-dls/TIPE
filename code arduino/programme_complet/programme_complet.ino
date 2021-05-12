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
#define KP 2
#define KI 0
#define KD 0.5
#define periode 1000

#define processingOn true

// variables golbales
double mesure; // de 0 a 1023, (la librairie utilisé ne fonctionne qu'avec des doubles)
double mesureOld = 0;
long int tempsOld = millis();
double deriv = 0;

double commande = 0; // de 0 a 1023
double output; // de 0 a 1023

String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

//objets
Servo moteur;
//AutoPID myPID(&mesure, &commande, &output, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);



void setup() {
  Serial.begin(115200);
  inputString.reserve(200);
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
  moteur.write(120); // on place l'ESC a 0 pour enlever la securité, j'utilise un esc de voiture télécommandé, son 0 n'est donc en réalité à 100
  for(int i=120; i<140 ; i++){
    moteur.write(i);
    Serial.println(i);
    delay(70);
  }
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

 
   
    //Serial.print ("intégrale:" +String(myPID.getIntegral())+ ", ");
  
    
  
    //digitalWrite(LED_PIN, myPID.atSetPoint(0.1));
    deriv = 1000*(mesure - mesureOld)/(millis()-tempsOld);
    output = KP * (mesure-commande) + KD * deriv;

    mesureOld = mesure;
    tempsOld = millis();
    
    //Serial.print ("output:" +String(output) + ", ");
  


  //output est en radian par seconde, motorspeed en rpm
  //double motorSpeed = map(output, 0, 2*pi, 0, 60);
  int motorSpeed = output/(2*pi)*60;
  if (motorSpeed > 0) {
    // on met a jour la vitesse:
    myStepper.setSpeed(motorSpeed+5);
    // step 1/100 of a revolution:
    myStepper.step(stepsPerRevolution / 200);
  }
  else if (motorSpeed < 0) {
    // on met a jour la vitesse:
    myStepper.setSpeed(- motorSpeed+5);
    // step 1/100 of a revolution:
    myStepper.step(- stepsPerRevolution / 200);
  }
  if (not processingOn ){
    Serial.print ("temps :" +String(millis()) + ", ");
    Serial.print ("mesure:" +String(mesure) + ", ");
    Serial.print("deriv:" + String(deriv)+",");
    Serial.print ("commande :" +String(commande) + ", ");
    Serial.print ("output:" +String(output) + ", ");
    Serial.println ("motorSpeed:" +String(motorSpeed) );
  }
  else {
    
    Serial.print (String(commande) + ",");
    Serial.print (String(mesure)+",");
    Serial.print (String(deriv)+",");
    Serial.print (String(output) + ",");
    Serial.print (String(motorSpeed)+ "," );
    Serial.println (String(millis()-tempsOld));
  }
  while(tempsOld +50> millis()){
    delay(1);
  }
  //serialEvent();
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
  //if inputString[0:2]="KD="{
  //  KD = int(inputString)
  //}
  stringComplete = false;
  inputString="";
  
}
