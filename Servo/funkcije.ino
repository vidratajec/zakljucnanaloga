
#define DEBUG 0

#define TEST 1

float preracunKota(String moj_ukaz)
{
  int A, B, C, D;
  A = moj_ukaz[2] - 48; 
  B = moj_ukaz[3] - 48; 
  C = moj_ukaz[4] - 48;
  D = moj_ukaz[5] - 48;
  float kot_ =  A * 100.0 + B*10.0 + C*1.0 + D*0.1;
  float kot = constrain(kot_, MIN_KOT, MAX_KOT);
  return kot;
}

int preracunHitrosti(String moj_ukaz)
{
  int A, B, C;
  A = moj_ukaz[1] - 48; 
  B = moj_ukaz[2] - 48; 
  C = moj_ukaz[3] - 48;
  int hitrost_ =  A * 100.0 + B*10.0 + C*1.0;
  int hitrost = constrain(hitrost_, MIN_HITROST, MAX_HITROST);
  return hitrost;
}

float beriKot()
{
  float kot___ = as5600.readAngle() * AS5600_RAW_TO_DEGREES;
  return kot___;
}

int vOkolici(float a, float b)
{
  /* Funkcija preveri ce je vrednost a v okolici vrednosti b, toleranca je podana z definicijo TOLERANCA v procentih/100 */
  if(b*(1-TOLERANCA) < a && a < b*(1+TOLERANCA)){
    return 1;
  }
  else{
    return 0;
  }
}

int globalni_predznak = 1;

int dolocanjeSmeri()
{
  int a = as5600.rawAngle(); //Ali readAngle();
  if(DEBUG) Serial.print("Pred pomikom: ");
  if(DEBUG) Serial.println(a);
  motor.setRPM(1);
  motor.setMicrostep(1);
  motor.move(3);
  int b = as5600.rawAngle();
  if(DEBUG) Serial.print("Po pomiku: ");
  if(DEBUG) Serial.println(b);
  if(DEBUG) Serial.print("Razlika: ");
  int c = b -a;
  if(DEBUG) Serial.println(c);
  if(c < 0){
    globalni_predznak = -1;
  }
  if(c == 0){
    Serial.println("Napaka-Premika");
    return 0;
  }
  if(c != 0){
    return 1;
  }
}

void zamik(float cas)
{
  int time = cas * 1000;
  if(KOMPENZACIJA_CAS_BRANJA && time > (ST_BRANJ * cas_senzorja)){
    time -= ST_BRANJ * cas_senzorja;
  }
  long konec = micros() + time;
  int j=0;
  while(micros() < konec){
    j++;
  }
}

float casBranjaSenzorja()
{
  float skupni_cas = 0.0;
  float a = micros();
  float kot = beriKot();
  float b = micros();
  return b-a;
}

void inkrementalniPremik(float zeljeni_kot, int predznak)
{
  //Pri inkrementalnemu premiku se motor premakne za tak kot, v zeljeno smer

  int stevilo_korakov = 0;
  int opravljeni_koraki = 0;
  int mikrokoraki = 0;
  float min_koeficient = 1000.0; // Neka zalo velika stevilka
  int napaka = 0;
  float df = 0.0;
  float df_ = 0.0;
  for(int i=0; i <= 4; i++){
    
    float kot_koraka = 360.0000/MOTOR_STEPS;
    float velikost_koraka = kot_koraka / pow(2.0, (float) i);
    //Serial.print(velikost_koraka, 4);
    int koraki = zeljeni_kot / velikost_koraka;
    //Napaka, ki jo bomo dobili z koraki je.
    float napaka = zeljeni_kot - koraki * velikost_koraka;
    float koef_napake = napaka * koraki;

    // napako * korake pomnozimo, najboljsa izbira velikosti in stevila mikrokorakov je tista, ki ima najmajsi produkt. 
    // napake, ki so manjse od kosnante senorja ali napakje koraka (5%) ne upsotevamo saj tak premik ni smiseln

    if(koef_napake < min_koeficient){
      stevilo_korakov = koraki;
      mikrokoraki = (int) pow(2.0, (float) i);
      min_koeficient = koef_napake;
    }

    if(DEBUG){
      Serial.print(i);
      Serial.print(" ");
      Serial.print(" ");
      Serial.print(koraki);
      Serial.print(" napaka: ");
      Serial.print(napaka, 4);
      Serial.print(" ");
      Serial.print(" koef napake: ");
      Serial.println(koef_napake);
      Serial.println("Najboljsa izbira je");
      Serial.println(min_koeficient);
      Serial.println(stevilo_korakov);
      Serial.println(mikrokoraki);
    }
  }

    motor.setMicrostep(mikrokoraki);
    float rps = motor.getRPM() / 60.0; //Koloko obratov v sekundi
    //V enem obratu naredimo MOTOR_STEPS * mikrokoraki
    float korakivrotaciji = MOTOR_STEPS * motor.getMicrostep();
    float korakivsenkundi = rps * korakivrotaciji;
    float cas = 1000/korakivsenkundi;
    if(DEBUG) Serial.println(cas*1000);


    float pk = 0.0;
    float pk1 = 0.0;
    float pk2 = 0.0;
    float zk = 0.0;
    float zk1 = 0.0;
    float zk2 = 0.0;
    float zacetni_kot = beriKot();
    float skupni_premik = 0;
    
    for(int j=1; j <= stevilo_korakov; j++){

      pk = abs(beriKot());
      motor.move((globalni_predznak*predznak)*1);
      //delay(cas);
      zamik(cas);
      zk = abs(beriKot());
      df = abs(zk-pk);
      if(df > 0){
        opravljeni_koraki++;
        if(DEBUG) Serial.println(j);
      }
      else {
        if(!TEST) Serial.print("Napaka koraka");
        if(!TEST) Serial.println(j);
        if(DEBUG){
          Serial.print("kot pred: ");
          Serial.print(pk,5);
          Serial.print(" kot po: ");
          Serial.print(zk,5);
          Serial.print(" razlika: ");
          Serial.println(abs(pk-zk),5);
        }
      }
      if(DEBUG){
        Serial.print("kot pred: ");
        Serial.print(pk);
        Serial.print(" kot po: ");
        Serial.print(zk);
        Serial.print(" razlika: ");
        Serial.println(abs(pk-zk));


      }
    }

    if(opravljeni_koraki == stevilo_korakov || vOkolici(beriKot(), zacetni_kot + zeljeni_kot )){
      if(!TEST) Serial.println(beriKot());
    }
    else{
      if(!TEST) Serial.println("Napaka-Premika");
    }

}




void absolutniPremik(float zeljeni_kot, int predznak)
{
  float pot = 0.0;
  float trenutni_kot = beriKot();
  float zacetni_kot = trenutni_kot;
  int smer = 0;

  float razlika1 = abs(trenutni_kot - zeljeni_kot); 
  float razlika2 = abs(360.0 - razlika1);
  
  /*Dolocitev smeri + -> smer urinega kazalca
                    - -> proti urinem kazalcu
                    0 -> najkrajsa pot       */
  
  /* Smer + -> CW
  Ce je zacetni kot nad ali 180 -> manjsa pot, ce je zacetni kot pod 180 -> daljsa pot */
  if(predznak == 1){
    if(zacetni_kot >= 180 ){
     pot = min(razlika1, razlika2);
   }
   else{
     pot = max(razlika1, razlika2);
   }
  }
  else if(predznak == -1){
    if(zacetni_kot >= 180){
      pot = max(razlika1, razlika2);
    }
    else{
      pot = min(razlika1, razlika2);

    }
  }
  else if(predznak == 0){
    //najkrajsa pot!
    pot = min(razlika1, razlika2);
    if(zacetni_kot >= 180){
      predznak = 1;
    }
    else{
      predznak = -1;
    }
  }
  //Za dolocanje korakov uporabimo isti algoritem kot pri intrementalnem premiku!

  int stevilo_korakov = 0;
  int mikrokoraki = 0;
  float min_koeficient = 1000.0;
  float kot_koraka = 360.0000/MOTOR_STEPS;

  for(int i=0; i <= 4; i++){
    float velikost_koraka = kot_koraka / pow(2.0, (float) i);
    int koraki = pot / velikost_koraka;
    float napaka = abs(pot - koraki * velikost_koraka);
    float koef_napake = napaka * koraki;

    if(koef_napake < min_koeficient){
      stevilo_korakov = koraki;
      mikrokoraki = (int) pow(2.0, (float) i);
      min_koeficient = koef_napake;
    }
  }

  int smer_rotacje = globalni_predznak * predznak;
  motor.setMicrostep(mikrokoraki);
  float rps = motor.getRPM() / 60.0; //Koloko obratov v sekundi
  float korakivrotaciji = MOTOR_STEPS * motor.getMicrostep();
  float korakivsenkundi = rps * korakivrotaciji;
  float cas = 1000/korakivsenkundi;


  float pk = 0.0;
  float zk = 0.0;
  int opravljeni_koraki = 0;

  if(DEBUG) Serial.print("stevilo korakov: ");
  if(DEBUG) Serial.println(stevilo_korakov);

  for(int k=0; k < stevilo_korakov; k++){
    pk = beriKot();
    motor.move(1 * smer_rotacje);
    //delay(cas);
    zamik(cas);
    zk = beriKot();
    if(abs(pk-zk) > 0){ // ne rabimo?
      opravljeni_koraki++;
    }
  }

  float koncni_kot = beriKot();
  if(vOkolici(zeljeni_kot, koncni_kot) ){
    Serial.println(koncni_kot);
  }
  else{
    Serial.println("Napaka-Premika");
  }
}

int inkrementalniPremik2(float zeljeni_kot, int predznak)
{
  //Pri inkrementalnemu premiku se motor premakne za tak kot, v zeljeno smer

  int stevilo_korakov = 0;
  int opravljeni_koraki = 0;
  int mikrokoraki = 0;
  float min_koeficient = 1000.0; // Neka zalo velika stevilka
  int napaka = 0;
  float df = 0.0;
  float df_ = 0.0;
  for(int i=0; i <= 4; i++){
    
    float kot_koraka = 360.0000/MOTOR_STEPS;
    float velikost_koraka = kot_koraka / pow(2.0, (float) i);
    //Serial.print(velikost_koraka, 4);
    int koraki = zeljeni_kot / velikost_koraka;
    //Napaka, ki jo bomo dobili z koraki je.
    float napaka = zeljeni_kot - koraki * velikost_koraka;
    float koef_napake = napaka * koraki;

    if(koef_napake < min_koeficient){
      stevilo_korakov = koraki;
      mikrokoraki = (int) pow(2.0, (float) i);
      min_koeficient = koef_napake;
    }
  }
    motor.setMicrostep(mikrokoraki);
    float rps = motor.getRPM() / 60.0; //Koloko obratov v sekundi
    //V enem obratu naredimo MOTOR_STEPS * mikrokoraki
    float korakivrotaciji = MOTOR_STEPS * motor.getMicrostep();
    float korakivsenkundi = rps * korakivrotaciji;
    float cas = 1000/korakivsenkundi;
    if(DEBUG) Serial.println(cas*1000);

  
    float pk = 0.0;
    float zk = -0.1;
    float zacetni_kot = beriKot();
    float skupni_premik = 0;
    
    for(int j=1; j <= stevilo_korakov; j++){
      

      pk = beriKot();
      motor.move((globalni_predznak*predznak)*1);
      zamik(cas);
      
      if(pk != zk){
        opravljeni_koraki++;
        if(DEBUG) Serial.println(j);
      }
      else {
        if(!TEST) Serial.print("Napaka koraka");
        if(!TEST) Serial.println(j);
        break;
        if(DEBUG){
          Serial.print("kot pred: ");
          Serial.print(pk,5);
          Serial.print(" kot po: ");
          Serial.print(zk,5);
          Serial.print(" razlika: ");
          Serial.println(abs(pk-zk),5);
        }
      }
      

      if(DEBUG){
        Serial.print("kot pred: ");
        Serial.print(pk);
        Serial.print(" kot po: ");
        Serial.print(zk);
        Serial.print(" razlika: ");
        Serial.println(abs(pk-zk));


      }
      zk = pk;
    }

    float koncni_kot = beriKot();
  if(vOkolici(zeljeni_kot, koncni_kot) || opravljeni_koraki == stevilo_korakov){
    if(!TEST) Serial.println(koncni_kot);
    return 1;
  }
  else{
    return 0;
  }

}
