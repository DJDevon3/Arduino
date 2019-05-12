/*
 * INFINITE COUNTDOWN ON A 16x2 LCD, WHEN COUNTDOWN REACHES 0 THE TIMER RESTARTS.
 * Good for gym workout timing or other uses.
 * DON'T FORGET TO SET THE RESET TIMES ON LINES 52-54 THE SAME AS THE START TIME!
 * By default it's set to 5 minutes.
 * 
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
*/

// include the library code:
#include <LiquidCrystal.h>
int hours = 0; // start hours
int minutes = 5; //start min
int seconds = 0; //start seconds

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  lcd.begin(16, 2);
}
void stepDown() {
  if (seconds > 0) {
  seconds -= 1;
  } else {
    if (minutes > 0) {
    seconds = 59;
    minutes -= 1;
    } else {
      if (hours > 0) {
      seconds = 59;
      minutes = 59;
      hours -= 1;
      } else {
      lcd.clear();
      lcd.setCursor(5, 1);
      lcd.println("SWITCH ");
      delay(1000);
      hours = 0; // RESET hours
      minutes = 5; //RESET min
      seconds = 0; //RESET seconds
      }
    }
  }
}

void begintimer(){
 lcd.clear();
 lcd.begin(16, 2);
 lcd.print("Count Down Timer ");
 delay(150);
}

void loop() {
  begintimer();
 if (hours > 0 || minutes > 0 || seconds >= 0) {
    lcd.setCursor(4, 2);
    (hours < 10) ? lcd.print("0") : NULL;
    lcd.print(hours);
    lcd.print(":");
    (minutes < 10) ? lcd.print("0") : NULL;
    lcd.print(minutes);
    lcd.print(":");
    (seconds < 10) ? lcd.print("0") : NULL;
    lcd.print(seconds);
    lcd.display();
    stepDown();
    delay(1000);
  } else {
    begintimer();
    lcd.setCursor(4, 2);
    (hours < 10) ? lcd.print("0") : NULL;
    lcd.print(hours);
    lcd.print(":");
    (minutes < 10) ? lcd.print("0") : NULL;
    lcd.print(minutes);
    lcd.print(":");
    (seconds < 10) ? lcd.print("0") : NULL;
    lcd.print(seconds);
    lcd.display();
    stepDown();
    delay(1000);
  }
}
