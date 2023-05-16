#include <string.h>
#include <EEPROM.h>
#include <IRremote.hpp>
#include <LiquidCrystal_I2C.h>
#include "in.hpp"
#define pinOfremote 2


#define upkey 0xB946FF00
#define downkey 0xEA15FF00
#define leftkey 0xBC43FF00
#define rightkey 0xBC43FF00
#define okkey 0xBF40FF00






IRrecv remote(pinOfremote);
LiquidCrystal_I2C lcd(0x27, 16, 2);

int hour[] = { 1, 0, 0 };
int date[] = { 2023, 5, 1 };
long int time;
long int time1;

void setup() {
  time = millis();
  time1 = millis();

  lcdinit();
  remote.enableIRIn();

  Serial.begin(9600);
  pinMode(3, 1);


  lcd.setCursor(12, 0);
  for (int x = 0; x < 2; x++) {

    lcd.print(hour[x]);
    if (x < 1)
      lcd.print(":");
  }
}

bool isligth = true;
void loop() {
  if (millis() >= time1 + 1000) {
    time1 = millis();
    hour[2]++;
  }
  if (millis() >= time + 2000) {
    time = millis();
    lcdClean();
  }

  lcd.setCursor(3, 0);
  for (int x = 0; x < 3; x++) {

    lcd.print(date[x]);
    if (x < 2)
      lcd.print("/");
  }


  lcd.setCursor(12, 0);
  for (int x = 0; x < 2; x++) {
    if (x == 1 && hour[1] < 10) lcd.print("0");
    lcd.print(hour[x]);
    if (x < 1)
      lcd.print(":");
  }


  if (date[1] == 12) {
    date[1] = 1;
    date[0]++;
  }


  if (hour[0] == 12) {
    hour[0] = 1;
    if (date[2] < 30)
      date[2]++;
    else if (date[2] == 30) {
      date[1]++;
    } else date[2] = 1;
  }
  if (hour[1] == 60) {

    hour[0]++;
    hour[1] = 0;
  }
  if (hour[2] == 60) {
    hour[1]++;
    hour[2] = 0;
  }



  if (remote.decode()) {

    if (remote.decodedIRData.decodedRawData == 0xE916FF00 /* key nubmer 1*/) {
      Serial.println("1");
      isligth = false;
      PIND |= (1 << 3);
      lcdClean();
      lcd.setCursor(0, 1);
      lcd.print("light manually ");
    }
    if (remote.decodedIRData.decodedRawData == 0xE619FF00 /* key nubmer 2*/) {
      isligth = true;
      lcdClean();
      lcd.setCursor(0, 1);

      lcd.print("light Auto mod");
    }
    // Serial.println(remote.decodedIRData.decodedRawData , HEX);

    if (remote.decodedIRData.decodedRawData == okkey) {
      const String seting[] = {
        "irrigation day",
        "date",
        "time"
      };
      uint8_t irrigation_day = EEPROM.read(0);
      remote.resume();
      int nubmerOfseting = 0;
      lcd.setCursor(0, 1);
      lcd.print(seting[nubmerOfseting].c_str());
      while (1) {

        if (remote.decode()) {

          if (remote.decodedIRData.decodedRawData == upkey) {

            if (nubmerOfseting > 0) {
              nubmerOfseting--;
            }
          } else if (remote.decodedIRData.decodedRawData == downkey) {
            if (nubmerOfseting < 2) {
              nubmerOfseting++;
            }
          } else if (remote.decodedIRData.decodedRawData == okkey) {
            remote.resume();

            while (1) {
              if (remote.decode()) {

                if (nubmerOfseting == 0) {


                  if (remote.decodedIRData.decodedRawData == upkey && irrigation_day < 29) {
                    irrigation_day++;

                  } else if (remote.decodedIRData.decodedRawData == downkey && irrigation_day > 1) {
                    irrigation_day--;

                  } else if (remote.decodedIRData.decodedRawData == okkey) {
                    EEPROM.update(0, irrigation_day);
                    lcdClean();
                    lcd.setCursor(0, 1);
                    lcd.print("done");
                    delay(1000);
                    goto start;
                  }
                  lcdClean();
                  lcd.setCursor(0, 1);
                  lcd.print(irrigation_day);
                }else  if (nubmerOfseting == 1) {
                  
                }
              }
              remote.resume();
            }
          }
          lcdClean();
          lcd.setCursor(0, 1);
          lcd.print(seting[nubmerOfseting].c_str());
        }
        
        remote.resume();
      }
    }
start:
    remote.resume();
  }


  if (analogRead(A5) <= 800 && isligth) {
    digitalWrite(3, 1);
  } else if (isligth) digitalWrite(3, 0);
}
void lcdinit(void) {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.createChar(0, ls);
  lcd.createChar(1, le);

  lcd.blink_off();
  lcd.setCursor(0, 0);
  lcd.write(0);
  lcd.setCursor(1, 0);
  lcd.write(1);
}

void lcdClean() {
  lcd.setCursor(0, 1);
  lcd.print("                 ");
}
