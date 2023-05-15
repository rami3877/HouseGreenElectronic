#include <IRremote.hpp>


#include <LiquidCrystal_I2C.h>
#include "in.hpp"
#define pinOfremote 2

IRrecv remote(pinOfremote);
LiquidCrystal_I2C lcd(0x27, 16, 2);


long int tiem;
void setup() {
  tiem = millis();
  lcdinit();
  remote.enableIRIn();

  Serial.begin(9600);
  pinMode(3, 1);
}
// 720
bool isligth = true;
void loop() {
  if (millis() >= tiem + 2000) {
    tiem = millis();
    lcdClean(); 
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
    // Serial.println(remote.decodedIRData.decodedRawData, HEX);

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
