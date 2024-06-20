#include <AS5600.h>
#include <Arduino.h>
#include "A4988.h"
#include "Wire.h"


/*-----------------------------------------------------------------------------------------
-----------------------DEFINICIJE----------------------------------------------------------
-----------------------------------------------------------------------------------------*/
//Motor
#define MOTOR_STEPS 200
#define RPM 50 // osnovna hitrost preden jo nastavimo
#define DIR  29
#define STEP  28
#define ENABLE  14
#define MS1  15
#define MS2  26
#define MS3  27
#define MIN_HITROST 1
#define MAX_HITROST 400
#define MIN_KOT 0
#define MAX_KOT 360
#define NAPAKA_MOTORJA 0.09

#define TOLERANCA 0.05

/*-----------------------------------------------------------------------------------------
-----------------------SPREMENLJIVKE-------------------------------------------------------
-----------------------------------------------------------------------------------------*/
String ukaz = "000000";
char tip_premika;
char smer_premika_u;
int hitrost_vrtenja = 0;
int predznak = 0;
int smer_vrtenja = 0;
float offset = 0.0;
float cas_senzorja = 0.0;

//float trenutni_kot -> berem s senzorja;
float zeljeni_kot = 0.0;
float trenutni_kot = 0.0;
float razlika_kot = 0.0;

#define KOMPENZACIJA_CAS_BRANJA 0
#define ST_BRANJ 1

/*-----------------------------------------------------------------------------------------
-----------------------KODA----------------------------------------------------------------
-----------------------------------------------------------------------------------------*/
A4988 motor(MOTOR_STEPS, DIR, STEP, ENABLE, MS1, MS2, MS3);

AS5600 as5600; 

void setup() {
//Serijska povezava
  Serial.begin(115200);
  while(!Serial);

//I2C
Wire.begin();
//Wire.setClock(1000000);

//Koracni motor
  motor.begin(RPM);
  motor.setEnableActiveState(LOW);
  motor.enable();

//kodrinik
  as5600.begin();
  as5600.setDirection(AS5600_CLOCK_WISE);
  if( as5600.isConnected() && as5600.detectMagnet() && !as5600.magnetTooStrong() && !as5600.magnetTooWeak() && dolocanjeSmeri()){
     Serial.println("Servomotor-OK");
  }
  else{
    Serial.println("Napaka-Senzor");
    while(1);
  }
  cas_senzorja = casBranjaSenzorja();
  Serial.print("povpreceni cas baranja senzorja je: ");
  Serial.println(cas_senzorja);
  
  //motor.setRPM(20);
  /*absolutniPremik(180, 0);
  Serial.println(beriKot());
  for(int n = 0; n < 5; n++){
    Serial.println(n);
    motor.setRPM(20);
    //absolutniPremik(180, 1);
    Serial.println(beriKot());
    delay(1000);
    testTocnostiRotacije();
    delay(1000);
  }*/

  //testHitrostiMotorja();
  //testHitrostiInkrementalniPomik1(); // Premik1 -> 2xbranje
  //testHitrostiInkrementalniPomik2(); //Pomik2 -> 1x branje
  //testSenzorja();

  motor.setRPM(10);
  inkrementalniPremik(30, 1);
  delay(500);
  motor.setRPM(20);
  inkrementalniPremik(30, 1);
  delay(500);
  

  motor.setRPM(30);
  inkrementalniPremik(30,  1);
  delay(500);
  motor.setRPM(40);
  inkrementalniPremik(90, -1);

}

void loop() {}
/*void loop() {
  while(Serial.available() != 0){
    ukaz = Serial.readStringUntil('\n');
    int dolzina = ukaz.length();
    


    if(dolzina == 6 && ukaz[0] == 'a' || ukaz[0] == 'i'){
      //Serial.println("Ukaz za premik");
      tip_premika = ukaz[0];
      smer_premika_u = ukaz[1];
      float kot = preracunKota(ukaz);
      //Serial.println(kot);
      if(smer_premika_u == '-'){
        predznak = -1;

      }
      else if (smer_premika_u == '+'){
        predznak = 1;

      }
      else if(smer_premika_u == '0'){
        predznak = 0;
      }

      if(tip_premika == 'a'){
        absolutniPremik(kot, predznak);
      }
      else if(tip_premika == 'i'){
        inkrementalniPremik2(kot, predznak);
      }

    }
    else if(dolzina == 5 && ukaz[0] == 'n' ){ //TODO
      //Serial.print("nastavitev nove nicle pri kotu ");
      float kotn = preracunKota(ukaz);
      as5600.setOffset(kotn);
      Serial.println("Nastavljeno-zamik");
    }
    else if(dolzina == 4 && ukaz[0] == 'r'){
      hitrost_vrtenja = preracunHitrosti(ukaz);
      motor.setRPM(hitrost_vrtenja); 
      Serial.println(hitrost_vrtenja);
    }
    else if( ukaz == "tk"){
      Serial.println(beriKot());
      
    }
    else{
      Serial.println("Napacen ukaz");
    }
  }
}*/
