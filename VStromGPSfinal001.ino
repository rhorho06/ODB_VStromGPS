//derniers essais le 05/05/2018
//reste le probleme des caracteres speciaux.
//affiner l'affichage avec lcd.setCursor





#include <LiquidCrystal_I2C.h>
#include <SparkFunBME280.h>
#include <Wire.h>
#include <TinyGPS.h>
#include <SoftwareSerial.h>

#define LedT 2
#define RXpin 4
#define TXpin 3
BME280 capteur;


LiquidCrystal_I2C lcd(0x3F,20,4);

char *jours[] = {"Dim","Lun","Mar","Mer","Jeu","Ven","Sam"};

void aff_jour (int mod)
{
  char out[3];
  sprintf(out,"%s", jours[mod]);
  lcd.setCursor(0, 0);
  lcd.print("                    ");
  lcd.setCursor(0, 0);
  lcd.print(out);
}

int get_mod (int annee, byte mois, byte jour)
{
  int z;
  char k;

  if (mois < 3 ){
    z = annee - 1;
    k = 0;
  }
  else {
    z = annee;
    k = 2;
  }
  int Mod = (((23*mois)/9)+jour+4+annee+(z/4)-(z/100)+(z/400)-k)%7;
  return Mod;
}

int get_hev(byte jour, byte mois, int mod)
{
  int hev;
  if (mois < 3 || mois > 10){
    hev = 1;
  }
  if (mois > 3 && mois < 10){
    hev = 2;
  }
  if (mois == 3 ){
    if ((jour - mod) > 24){
      hev = 2;
    }
    else {hev =1;}
  }
  if (mois == 10){
    if ((jour - mod) > 24){
      hev = 1;
    }
    else {hev = 2;}
  }
  return hev;
}


TinyGPS gps;
SoftwareSerial MT3339(RXpin,TXpin);
void getgps(TinyGPS &gps);








void setup() {
  
  lcd.init();
  Serial.begin(9600);
  MT3339.begin(9600);
  digitalWrite(11, HIGH);
  while (!Serial) {
    // Attente de l'ouverture du port série pour Arduino LEONARDO
  }
  //configuration du capteur
  capteur.settings.commInterface = I2C_MODE; 
  capteur.settings.I2CAddress = 0x76;
  capteur.settings.runMode = 3; 
  capteur.settings.tStandby = 0;
  capteur.settings.filter = 0;
  capteur.settings.tempOverSample = 1 ;
  capteur.settings.pressOverSample = 1;
  capteur.settings.humidOverSample = 1;
 
  lcd.setCursor(0, 1);
  lcd.print("Starting BME280... ");
  delay(10);  // attente de la mise en route du capteur. 2 ms minimum
  // chargement de la configuration du capteur
  capteur.begin();

  lcd.setCursor(0, 0);
  lcd.print("Suzuki DL650 V-Strom");
  lcd.setCursor(0,1);
  lcd.print("Attn Satellites ....");
  
}

void loop() {
  lcd.backlight();
  while(MT3339.available())
    {
    int c=MT3339.read();
    if(gps.encode(c))
    {
      getgps(gps);
    }
  }
 
}

void getgps(TinyGPS &gps)
{
  float latitude, longitude;
  gps.f_get_position(&latitude, &longitude);
  //lcd.clear();
  lcd.setCursor(0, 3);
  lcd.print("         ");
  lcd.setCursor(0, 3);
  lcd.print(latitude,5);
  if (latitude < 0){
    lcd.print("S");
    }
    else {
      lcd.print("N");
    }
  

  lcd.setCursor(10, 3);
  lcd.print("          ");
  lcd.setCursor(10, 3);
  lcd.print(longitude,5);
  if (longitude < 0){
    lcd.print("O");
    }
    else {
      lcd.print("E");
    }
  
  int ete_hiver;
  int year;
  byte month, day, hour, minute, second, hundredths;
  gps.crack_datetime(&year,&month,&day,&hour,&minute,&second,&hundredths);

  int mod = get_mod(year, month, day);
  aff_jour (mod);
  int hev = get_hev(day, month, mod);
  ete_hiver= hour + hev;

  char out[6];
  sprintf(out, "%02d", day);
  lcd.setCursor(4, 0);
  lcd.print(out);
  lcd.print("/"); 
  sprintf(out, "%02d", month);
  lcd.print(out);
  lcd.print("/");
  lcd.print(year);
  lcd.print(" ");
  sprintf(out, "%02d", ete_hiver);
  lcd.print(out);
  lcd.print(":");
  sprintf(out, "%02d", minute,DEC);
  lcd.print(out);

  float Volts = (15 / 1024.0) * analogRead(0);
  
  lcd.setCursor(0, 2);
  lcd.print("                    ");
  lcd.setCursor(0, 2);
  lcd.print(Volts, 1);
  lcd.print("V ");
  lcd.print(gps.f_altitude(),0); 
  lcd.print("m ");
  
  int caplu = gps.f_course();
  int boussole;
  if (caplu <= 22) {
    boussole = 0;
    }
  if (23 <= caplu && caplu< 68) {
    boussole = 1;
    }
  if (68 <= caplu && caplu< 113) {
    boussole = 2;
    }
  if (113 <= caplu && caplu< 158) {
    boussole = 3;
    }
  if (158 <= caplu && caplu< 203) {
    boussole = 4;
    }
  if (203 <= caplu && caplu< 248) {
    boussole = 5; 
    }
  if (248 <= caplu && caplu< 293) {
    boussole = 6;
    }
  if (293 <= caplu && caplu< 338) {
    boussole = 7;
    }
   if (caplu >= 338) {
    boussole = 1;
    }
  char *cap[] = {"-N","NE","-E","SE","S-","SO","O-","NO"};
  lcd.print(cap[boussole]);
  lcd.print(" ");
  lcd.print(gps.f_speed_kmph(),0);
  lcd.print("Kh ");

  
  float tempread = capteur.readTempC();
  float tempcalc = tempread * 2;
  int tempint = tempcalc;
  tempcalc = tempint / 2.0;
  if (tempint <= 6) {
    digitalWrite(LedT, HIGH);
  }
  else {
    digitalWrite(LedT, LOW);
  }
  
  lcd.setCursor(0, 1);
  lcd.print("                    ");
  lcd.setCursor(0, 1);
  lcd.print(tempcalc,1);
  lcd.print((char)223);
  //lcd.print("C");
  //lcd.write(byte(0));
  lcd.setCursor(7, 1);
  lcd.print(((capteur.readFloatPressure())/100)+28, 0);
  //lcd.print(" ");
  lcd.print("mB");
  //lcd.print(" ");
  lcd.setCursor(14, 1);
  lcd.print(capteur.readFloatHumidity(), 0);
  lcd.print("%");
  lcd.print(" ");
  lcd.print((char)178);
  //lcd.write(1);
  lcd.print(gps.satellites());
  
  delay(1000);
  
  
}


