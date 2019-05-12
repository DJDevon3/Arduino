/*
 * Demonstrates the use a 16x2 LCD display prior to adding the SparkFun Spectrum Shield.
 * Good for reading bias voltage for setting your E.Q. or VU meter floor limiter.
 * An LCD isn't necessary at all. I've only added it for those who run LCD's with the shield.
 * 
 */

#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

float input_voltage = 0.0;
float temp=0.0;

void setup()
{
   Serial.begin(9600); 
   lcd.begin(16, 2);
   lcd.print("Voltage");
}
void loop(){
   int analog_value = analogRead(A0);
   float input_voltage = analog_value * (5.0 / 1023.0);
   if (input_voltage < 0.1) {
     input_voltage=0.0;
   } 
   Serial.print("v= ");
   Serial.println(input_voltage); // print to serial monitor and plotter for monitoring bias, you can write a script to avg bias and reduce it.
   lcd.setCursor(0, 1);
   lcd.print(input_voltage);  // print voltage to LCD
   delay(300); // No sense in refreshing the value so fast that you can't see the changes. Use the serial plotter for that instead, plotter is better for debugging.
}
