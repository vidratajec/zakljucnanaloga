

void testTocnostiRotacije()
{
  motor.setRPM(50);
  //absolutniPremik(180,0); //Da slucajno ne prestropimo meje 360/0
  /* Test premakne v eno in drugo smer za enak kot, merimo razliko zacetnega in koncnega kota */
  for(int s = 10; s <= 100; s += 10){
    motor.setRPM(s);
    float idfi = 0.0;
    for(float i = 4.5; i <= 90.0; i += 4.5){
      //Serial.print("Kot premika: ");
      //Serial.print(i);
      float z_kot = beriKot(); 
      inkrementalniPremik2(i, 1);
      inkrementalniPremik2(i, -1);
      float k_kot = beriKot();
      float dfi = k_kot - z_kot;
      idfi += dfi;
    }
    Serial.print(s);
    Serial.print("  ");
    Serial.print(idfi/20);
    Serial.println();
}
}

void testHitrostiMotorja()
{
  /*Dejanska hitrost motorja - brez meritev kota */

  for(int i = 5; i <=300; i+=5){
    int zacetnicas = micros();
    motor.setRPM(i);
    motor.move(200);
    int koncnicas = micros();
    int casrotacije = (koncnicas - zacetnicas); //ms
    Serial.print(i);
    Serial.print(" ");
    Serial.print(60000000.0/casrotacije);
    Serial.println();
  }
}


void testHitrostiInkrementalniPomik1()
{
  for(int i=5; i <= 300; i+=5){
    int zacetnicas = micros();
    motor.setRPM(i);
    inkrementalniPremik(360, 1);
    int koncnicas = micros();
    int casrotacije = (koncnicas - zacetnicas); //ms
    Serial.print(i);
    Serial.print(" ");
    Serial.print(60000000.0/casrotacije);
    Serial.println();
  }
}

void testHitrostiInkrementalniPomik2()
{
  for(int i=5; i <= 300; i+=5){
    int zacetnicas = millis();
    motor.setRPM(i);
    inkrementalniPremik2(360, 1);
    int koncnicas = millis();
    int casrotacije = koncnicas - zacetnicas;
    Serial.print(i);
    Serial.print(" ");
    Serial.print(60000.0/casrotacije);
    Serial.println();
  }
}


void testSenzorja()
{
  int a = 0;
  int b = 0;
  int c = 0;
  float kot=0.0;
  for(int m = 0; m < 50; m++){
    a = micros();
    kot=beriKot();
    b = micros();
    c = b -a;
    Serial.println(c);
  }

}