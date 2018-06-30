#include <LiquidCrystal.h>
#define DELAY 250
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS A2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

int flag_pasang, flag_surut, show_pasang = 1, show_surut = 1;
int state = 0, wait = 0;
int surut = 0, pasang = 0;
long int durasi_pasang = 0, durasi_surut = 0;
int waktuHabis = 0;
int selenoid_pin = A1;
int i = 0;
float suhu1 = 0.00, suhu2 = 0.00, suhu3 = 0.00, suhu4 = 0.00;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup() {
  Serial.begin(9600);
  pinMode(selenoid_pin, OUTPUT);
  lcd.begin(16, 2);
  sensors.begin();
}

void loop() {

set_waktu();

}

void sensor(){

           sensors.requestTemperatures();
           suhu1 = sensors.getTempCByIndex(0);
           suhu2 = sensors.getTempCByIndex(1);
           suhu3 = sensors.getTempCByIndex(2);
           suhu4 = sensors.getTempCByIndex(3);
           
           lcd.setCursor(0, 0); 
           lcd.print("S1 ");
           lcd.print(String(suhu1, 1));
           lcd.print("  S2 ");
           lcd.print(String(suhu2, 1)); 
           lcd.setCursor(0, 1); 
           lcd.print("S3 ");
           lcd.print(String(suhu3, 1));
           lcd.print("  S4 ");
           lcd.print(String(suhu4, 1)); 
}

void set_waktu(){
      if (wait == 1) { // Menunggu respon user
            switch (state) {
              case 0:
                if ( enter() ) {
                  state = 1;
                  wait = 0;
                }
                break; 
              case 1:
                if ( atas() ) {
                  if (surut < 24) surut= surut + 1;
                }
                if ( bawah() ){
                  if (surut > 0) surut= surut - 1;
                }
                if (enter() ) {
                  state = 2;
                  wait = 0;
                }
                wait = 0;
                break;    
              case 2:
                if ( atas() ) {
                  if (pasang < 240) pasang++;
                }
                if ( bawah() ){
                  if (pasang > 1) pasang--;
                }
                if (enter() ) {
                  state = 3;
                  wait = 0;
                }
                wait = 0;
                break;  
              case 4:
                if ( enter() ) {
                  state = 0;
                  wait = 0;
                }
                break;    
            }
      }
      else { // Proses sedang berjalan
            switch (state) {
              case 0:
                reset();
                wait = 1;
                break; 
              case 1:
                waktu_surut();
                wait = 1;
                break;
              case 2:
                waktu_pasang();
                wait = 1;
                break;     
              case 3:
                proses();
                wait = 0;
                state = 4;
                break;
              case 4:
                selesai();
                wait = 1;
                break;
            }
        delay(DELAY);
      }  
}

void reset() {
  lcdClear(); 
  for ( int i = 0; i < 15; i++){
      lcd.setCursor(0, 0);
      lcd.print("Loading");
      lcd.setCursor(i, 1);
      lcd.print(">");
      delay(200);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(">>  ARDUINO  <<");
  lcd.setCursor(0, 1);
  lcd.print("   AKUAPONIK   ");
  delay(2000);
  lcdClear();
  lcd.setCursor(0, 0);
  lcd.print(">   Setting:   <");
  lcd.setCursor(0, 1);
  lcd.print("Press : (Select)");
  delay(30);}

void waktu_surut() {
  lcdClear();
  lcd.setCursor(0, 0);
  lcd.print("Surut :");
  lcd.setCursor(0, 1);
  lcd.print("     ");
  lcd.print(surut);
  lcd.print(" minute    ");
}
void waktu_pasang() {
  lcdClear();
  lcd.setCursor(0, 0);
  lcd.print("Pasang :");
  lcd.setCursor(0, 1);
  lcd.print("    ");
  lcd.print(pasang);
  lcd.print(" minute");
}
int enter() {
  int val = analogRead(A0);
  if ((val >= 625) && (val < 675)){
     return 1;
  }  
  else{
     return 0;
  } 
}
int kanan() {
  int val = analogRead(A0);
  if (val == 0){
     return 1;
  }  
  else{
     return 0;
  } 
}
int kiri() {
  int val = analogRead(A0);
  if ((val >= 380) && (val < 420)){
     return 1;
  }  
  else{
     return 0;
  } 
}
int atas() {
  int val = analogRead(A0);
  if ((val >= 75) && (val < 125)){
     return 1;
  }  
  else{
     return 0;
  } 
}
int bawah() {
  int val = analogRead(A0);
  if ((val >= 225) && (val < 275)){
     return 1;
  }  
  else{
     return 0;
  } 
}

void lcdClear() {
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
}

void sisaWaktu_pasang() {
  
  long int sisaMs = (long int) pasang*60*1000 - durasi_pasang;
  if (sisaMs <= 0) waktuHabis = 1;
  long int sisaDetik = sisaMs/1000;
  long int sisaMenit = sisaDetik/60;
  long int sisaJam = sisaMenit/60;

  if(show_pasang == 1){
          lcdClear();
          tmElements_t tm;
          if (RTC.read(tm)){
              lcd.setCursor(0, 0);
              lcd.print(tm.Hour);
              lcd.print(':');
              lcd.print(tm.Minute);
              lcd.print(':');
              lcd.print(tm.Second);
              lcd.print(' ');
              lcd.print(tm.Day);
              lcd.print('/');
              lcd.print(tm.Month);
              //lcd.print('/');
              //lcd.print((tmYearToCalendar(tm.Year)) - 2000);
              
          lcd.setCursor(0, 1);
          lcd.print("Pasang: ");
          }
          if (sisaJam > 0) {
            lcd.print(sisaJam);
            lcd.print(":");
          } //else lcd.print("00:");
          if (sisaMenit > 0) {
            lcd.print(sisaMenit - sisaJam*60);
            lcd.print(":");
          } //else lcd.print("00:"); 
          lcd.print(sisaDetik%60);
  }
  else {
          sensor(); 
  }
}

void sisaWaktu_surut() {
  
  long int sisaMs = (long int) surut*60*1000 - durasi_surut;
  if (sisaMs <= 0) waktuHabis = 1;
  long int sisaDetik = sisaMs/1000;
  long int sisaMenit = sisaDetik/60;
  long int sisaJam = sisaMenit/60;

  if(show_surut == 1){
          lcdClear();        
          tmElements_t tm;
          if (RTC.read(tm)){
              lcd.setCursor(0, 0);
              lcd.print(tm.Hour);
              lcd.print(':');
              lcd.print(tm.Minute);
              lcd.print(':');
              lcd.print(tm.Second);
              lcd.print(' ');
              lcd.print(tm.Day);
              lcd.print('/');
              lcd.print(tm.Month);
              //lcd.print('/');
              //lcd.print((tmYearToCalendar(tm.Year)) - 2000);
              
          lcd.setCursor(0, 1);
          lcd.print("Surut  : ");
          }  
          if (sisaJam > 0) {
            lcd.print(sisaJam);
            lcd.print(":");
          }  //else lcd.print("00:");
          if (sisaMenit > 0) {
            lcd.print(sisaMenit - sisaJam*60);
            lcd.print(":");
          }  //else lcd.print("00:");
          lcd.print(sisaDetik%60);  
  }
  else {
          sensor(); 
  }  
}

void proses() {

  while(1){
    
      durasi_pasang = 0;
      durasi_surut = 0;
      flag_pasang = 0;
      flag_surut = 0;

    while(flag_pasang != 1){
        //-----------------------------------------------------------------------------------------------------------------------
        durasi_pasang = durasi_pasang + 1000;
        sisaWaktu_pasang();
        delay(1000);
        if (waktuHabis == 1){ 
           flag_pasang = 1;
           waktuHabis = 0;
        }
        //-----------------------------------------------------------------------------------------------------------------------

        if ( kanan() || kiri() ) {
            if( show_pasang == 0){show_pasang = 1; }
            else { show_pasang = 0; }
        }
        digitalWrite(selenoid_pin, LOW);
    }
    while(flag_surut != 1){
        //-----------------------------------------------------------------------------------------------------------------------
        durasi_surut = durasi_surut + 1000;
        sisaWaktu_surut();
        delay(1000);
        if (waktuHabis == 1){ 
           flag_surut = 1;
           waktuHabis = 0;
        } 
        //-----------------------------------------------------------------------------------------------------------------------

        if ( kanan() || kiri() ) {
            if( show_surut == 0){show_surut = 1; }
            else { show_surut = 0; }
        }
        digitalWrite(selenoid_pin, HIGH);
    }
  }
}

void selesai() {
  lcdClear(); 
  lcd.setCursor(0, 0);
  lcd.print("Sistem off!");
  lcd.setCursor(0, 1);
  lcd.print("Reset > (select)"); 
  //digitalWrite(7, LOW);
}
