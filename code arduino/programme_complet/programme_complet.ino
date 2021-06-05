/*
 * Ce programme permet de controler l'orientation d'un solide sur un pivot vertical à partir d'un volant d'inertie 
 *  
 * le pas à pas est sur les pins 8 a 11
 * 
 * ce programme reprend des parties des exemples de la librairie Stepper
 * 
 * créé le 6 mars 2021
 * par Paul de La Sayette
 */
//librairies
#include <Servo.h>
#include <Stepper.h>

// pins
#define POT_PIN A0
#define pinMot 6

//constantes
#define pi 3.1416
#define stepsPerRevolution 200
#define processingOn true
#define tempsRafraichissement 100 //en ms

// constantes du correcteur
#define K 1.1
#define Td 45 //en ms
#define a 25

// variables golbales
float mesure; // en radian
float mesureOld = 0;// en radian
long int tempsOld = millis();//en ms
float deriv = 0; //en rad/s
long int derniereCom= millis();//en ms

float commande = 0; // en radian
float output; // en rad/s
int  motorSpeed=0;// en rpm

//objets
Servo moteur;
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);


void setup() {
  Serial.begin(115200); // on définit un débit rapide pour ne pas ralentir l'arduino inutilement
  //while (!Serial) {}
  Serial.println("initialisation du port série");
  
  moteur.attach(pinMot,100,200);
  myStepper.setSpeed(10);
  delay(500);

  //initialisation du moteur
  moteur.write(125); 
  // on place l'ESC a 0 pour enlever la securité, j'utilise un esc de voiture télécommandé
  //son 0 n'est donc en réalité pas à 100 (borne basse du PWM) mais à 120
  for(int i=125; i<140 ; i++){ 
    // 120 correspond donc à 25% de la vitesse maximale, on est donc environ à 2500 tr/min(vérifié au stroboscope)
    moteur.write(i);
    Serial.println(i);
    delay(50);
  }
  delay(500);
  Serial.println("moteur lancé");

  commande = float(analogRead(POT_PIN))/1023*2*pi;
  Serial.println("commande :" + String(commande));
}

void loop() {
  //calcul de motorSpeed par un correcteur PD/avance de phase
  mesure = float(analogRead(POT_PIN))/1023*2*pi;
  deriv = ((mesure - mesureOld)/(millis()-tempsOld)+deriv)/2;// on fait une moyenne glissante sur 2 points

  //stockage des anciennes mesures
  mesureOld = mesure;
  tempsOld = millis(); 
  
  output = K *((mesure-commande) + Td*a*deriv);
  motorSpeed = output/(2*pi)*60;//output est en radian par seconde, motorspeed en rpm
  setMotorSpeed();

  //on impose une durée de boucle minimum ce qui filtre les hautes fréquences
  while(millis()-tempsOld < Td){
    delay(1);
  }

  //on communique avec l'ordinateur tout les 100ms 
  if (millis() - derniereCom >= tempsRafraichissement){
    com();
    derniereCom = millis();
  }
}
void com(){
  //communication avec l'ordinateur, j'utilise une synthaxe spaciale lorsque je fais des graphiques avec processing
  if (not processingOn ){
    Serial.print ("temps :" +String(millis()) + ", ");
    Serial.print ("mesure:" +String(mesure) + ", ");
    Serial.print("deriv:" + String(1000*deriv)+",");
    Serial.print ("commande :" +String(commande) + ", ");
    Serial.print ("output:" +String(output) + ", ");
    Serial.println ("motorSpeed:" +String(motorSpeed) );
  }
  else {
    Serial.print (String(commande) + ",");
    Serial.print (String(mesure)+",");
    Serial.print (String(1000*deriv)+",");
    Serial.print (String(motorSpeed)+ "," );
    Serial.println (String(millis()-tempsOld));
  }

  //l'ordinateur envoie "1" si il souhaite un échelon de 1 rad
  if (Serial.available() > 0) {
    if (Serial.read() == 49){ // 1 coorespond à 49 en ASCII
      commande = commande + 1;
    }
  }
}

void setMotorSpeed(){
  //on met a jour la vitesse du moteur et on lui fait se déplacer d'un pas
  if (motorSpeed > 60){//saturation haute
    myStepper.setSpeed(60);
    myStepper.step(1);
  }
  else if (motorSpeed > 0){//sens positif
    myStepper.setSpeed(motorSpeed+2);
    myStepper.step(1);
  }
  else if (motorSpeed < -60){//saturation basse
    myStepper.setSpeed(60);
    myStepper.step(-1);
  }
  else if (motorSpeed < 0) {//sens négatif
    myStepper.setSpeed(- motorSpeed+2);
    myStepper.step(-1);
  }
}
