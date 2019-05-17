/*********************

Example menu code for the Adafruit NEGATIVE RGB LCD Shield and Adafruit RGB Library
Coded by DJDevon3 https://github.com/DJDevon3/Arduino
Adapted code by PaulSS for a LiquidCrystal.h LCD.
https://www.instructables.com/id/Arduino-Uno-Menu-Template/

This code will probably work with other Adafruit library shields, I only have this one to test.
If you have code suggestions to make this SIMPLE MENU EXAMPLE more efficient feel free to contact me. :)

Adafruit Negative RGB LCD Shield
Uses only the I2C pins - Analog 4 & 5 on classic Arduinos, Digital 20 and 21 on Arduino Mega R3
This board/chip uses I2C 7-bit address 0x20
**********************/

// include the library code:
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>

// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// These #defines make it easy to set the backlight color. These are all the colors available for the negative RGB backlight shield.
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7
#define DISABLE 0x8
const int BaudRate = 115200;

// name the items in the array whatever you want
String menuItems[] = {"ITEM 1", "ITEM 2", "ITEM 3", "ITEM 4", "ITEM 5", "ITEM 6", "ITEM 7", "ITEM 8", "ITEM 9", "ITEM 10", "ITEM 11", "ITEM 12"};

#define NUMITEMS(menuItems) ((unsigned int) (sizeof (menuItems) / sizeof (menuItems [0])))
int maxMenuPages = (NUMITEMS (menuItems)-2);
 //int maxMenuPages = round(((sizeof(menuItems) / sizeof(String)) / 2) + .5);
 
// Navigation button variables
int readKey;

// Menu control variables
int menuPage = 0;
int cursorPosition = 0;

  // Creates custom arrow characters for the menu display (in hex).
  // You can only create a maximum of 8 custom characters choose wisely. 6 are already here for you.
  byte leftArrow[8] = {0x00,0x04,0x0C,0x1F,0x0C,0x04,0x00,0x00};
  // byte rightArrow[8] = {0x00,0x04,0x06,0x1F,0x06,0x04,0x00,0x00}; don't really need 2 right arrows
  byte checkMark[8] = {0x0,0x1,0x3,0x16,0x1c,0x8,0x0}; // unused but available for value set confirmations
  byte upArrow[8] = {0x04,0x0E,0x1F,0x04,0x04,0x04,0x04,0x00};
  byte downArrow[8] = {0x04,0x04,0x04,0x04,0x1F,0x0E,0x04,0x00};
  byte menuCursor[8] = {0x10,0x08,0x04,0x02,0x04,0x08,0x10,0x00};  // right arrow menu navigation


void setup() {
  Serial.begin(BaudRate); // Set above as a const int so you can lcd.print it to your display if you want.
  lcd.begin(16, 2); // set up the LCD's number of columns and rows:
  lcd.createChar(0, menuCursor); // Create the custom arrow characters in void setup for global use
  lcd.createChar(1, upArrow);
  lcd.createChar(2, downArrow);
  lcd.createChar(3, leftArrow);
  lcd.createChar(4, rightArrow);
  lcd.createChar(5, checkMark);
  lcd.setBacklight(RED);
  lcd.print("LED Show Chooser");
  lcd.setCursor(0, 1);
  lcd.print("Main Menu");
  delay(2000);
  lcd.setBacklight(GREEN);
  delay(500);
  lcd.setBacklight(BLUE);
  delay(500);

}
void loop() {
  mainMenuDraw();
  drawCursor();
  operateMainMenu();
}

/* EVERY PAGE HAS 2 MAIN MENU ITEMS. 
 *  Don't get pages confused with items. 
 *  Draws 2 menu items on the screen. 
 *  They change as you scroll through the menu.
*/
void mainMenuDraw() {
  lcd.setBacklight(WHITE);
  Serial.print("Menu Page ->" );
  Serial.println(menuPage);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(menuItems[menuPage]);
  lcd.setCursor(1, 1);
  lcd.print(menuItems[menuPage + 1]);
  if (menuPage == 0) {
    lcd.setCursor(15, 1);
    lcd.write(byte(2));
  } else if (menuPage > 0 and menuPage < maxMenuPages) {
    lcd.setCursor(15, 1);
    lcd.write(byte(2));
    lcd.setCursor(15, 0);
    lcd.write(byte(1));
  } else if (menuPage == maxMenuPages) {
    lcd.setCursor(15, 0);
    lcd.write(byte(1));
  }
}

// NAVIGATION CURSOR > When called, this function will erase the current cursor and redraw it based on the cursorPosition and menuPage variables.
void drawCursor() {
  for (int x = 0; x < 2; x++) {  // Erases current cursor
    lcd.setCursor(0, x);
    lcd.print(" ");
  }

  /* NAVIGATION CURSOR POSITION 
   * The menu is set up to be progressive (menuPage 0 = Item 1 & Item 2, menuPage 1 = Item 2 & Item 3, menuPage 2 = Item 3 & Item 4), so 
   * in order to determine where the cursor should be you need to see if you are at an odd or even menu page and an odd or even cursor position.
  */
  if (menuPage % 2 == 0) {
    if (cursorPosition % 2 == 0) {  // If the menu page is even and the cursor position is even that means the cursor should be on line 1
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
    }
    if (cursorPosition % 2 != 0) {  // If the menu page is even and the cursor position is odd that means the cursor should be on line 2
      lcd.setCursor(0, 1);
      lcd.write(byte(0));
    }
  }
  if (menuPage % 2 != 0) {
    if (cursorPosition % 2 == 0) {  // If the menu page is odd and the cursor position is even that means the cursor should be on line 2
      lcd.setCursor(0, 1);
      lcd.write(byte(0));
    }
    if (cursorPosition % 2 != 0) {  // If the menu page is odd and the cursor position is odd that means the cursor should be on line 1
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
    }
  }
}

void operateMainMenu() {
  int activeButton = 0;
  while (activeButton == 0) {
    int button;
    readKey = lcd.readButtons();
    if (readKey == 0) {
      delay(100);
      readKey = lcd.readButtons();
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 0: // When button returns as 0 there is no action taken
        break;
      case 1:  // This case will execute if the "forward" button is pressed
        button = 0;
        switch (cursorPosition) { // The case that is selected here is dependent on which menu page you are on and where the cursor is.
          case 0:
            subItem1();
            break;
          case 1:
            subItem2();
            break;
          case 2:
            subItem3();
            break;
          case 3:
            subItem4();
            break;
          case 4:
            subItem5();
            break;
          case 5:
            subItem6();
            break;
          case 6:
            subItem7();
            break;
          case 7:
            subItem8();
            break;
          case 8:
            subItem9();
            break;
          case 9:
            subItem10();
            break;
          case 10:
            subItem11();
            break;
           case 11:
            subItem12();
            break;
        }
        activeButton = 1;
        mainMenuDraw();
        drawCursor();
        break;
      case 2:
        button = 0;
        if (menuPage == 0) {
          cursorPosition = cursorPosition - 1;
          cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
        }
        if (menuPage % 2 == 0 and cursorPosition % 2 == 0) {
          menuPage = menuPage - 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        if (menuPage % 2 != 0 and cursorPosition % 2 != 0) {
          menuPage = menuPage - 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        cursorPosition = cursorPosition - 1;
        cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));

        mainMenuDraw();
        drawCursor();
        activeButton = 1;
        break;
      case 3:
        button = 0;
        if (menuPage % 2 == 0 and cursorPosition % 2 != 0) {
          menuPage = menuPage + 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        if (menuPage % 2 != 0 and cursorPosition % 2 == 0) {
          menuPage = menuPage + 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        cursorPosition = cursorPosition + 1;
        cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
        mainMenuDraw();
        drawCursor();
        activeButton = 1;
        break;
       default: break;
    }
  }
}

// Monitors for shield button presses and returns which was pressed.
/* HARDWARE SHIELD PIN VALUES
 * BUTTON READING STATE = 0
 * BUTTON_RIGHT = 2
 * BUTTON_UP = 8
 * BUTTON_DOWN = 4
 * BUTTON_LEFT = 16
 * BUTTON_SELECT = 1
 */
int evaluateButton(int x) {
  uint8_t buttons = lcd.readButtons();
  int result = 0;
  if (buttons & BUTTON_RIGHT) {
    Serial.print("Button Right ");
    Serial.println (BUTTON_RIGHT); // for debugging serial values
    result = 1; // right
  } else if (buttons & BUTTON_UP) {
    Serial.print("Button Up ");
    Serial.println (BUTTON_UP); // for debugging serial values
    result = 2; // up
  } else if (buttons & BUTTON_DOWN) {
    Serial.print("Button Down ");
    Serial.println (BUTTON_DOWN); // for debugging serial values
    result = 3; // down
  } else if (buttons & BUTTON_LEFT) {
    Serial.print("Button Left ");
    Serial.println (BUTTON_LEFT); // for debugging serial values
    result = 4; // left
  } else if (buttons & BUTTON_SELECT) { // currently unused work in progress
    Serial.print("Button Select ");
    Serial.println (BUTTON_SELECT); // for debugging serial values
    result = 5; // left
  }
  return result;
}
void backArrow(){ // Sub menu navigation back arrow
  lcd.setCursor(0,0);
  lcd.write(byte(3));
}
void backbutton (){ // Left button navigates back until mainmenu
   int activeButton = 0;
     while (activeButton == 0) {
     uint8_t button = lcd.readButtons();
    readKey = lcd.readButtons();;
    if (button & BUTTON_LEFT) {
      // delay(50);
      readKey = lcd.readButtons();
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        activeButton = 1;
        lcd.setBacklight(WHITE);
        break;
    }
  } 
}

void subItem1() { // Function executes when you select the 1st item from main menu
  lcd.clear();
  backArrow();
  lcd.setCursor(15, 1);
  lcd.write(byte(2));
  lcd.setCursor(3, 0);
  lcd.print("Sub Menu 1");
  lcd.setBacklight(RED);
  //choosebacklight();
  backbutton ();
}

void subItem2() { // Function executes when you select the 2nd item from main menu
  lcd.clear();
  backArrow();
  lcd.setCursor(3, 0);
  lcd.print("Sub Menu 2");
  lcd.setBacklight(YELLOW);
  backbutton ();
}

void subItem3() { // Function executes when you select the 3rd item from main menu
  lcd.clear();
  backArrow();
  lcd.setCursor(3, 0);
  lcd.print("Sub Menu 3");
  lcd.setBacklight(GREEN);
  backbutton ();
}

void subItem4() { // Function executes when you select the 4th item from main menu
  lcd.clear();
  backArrow();
  lcd.setCursor(3, 0);
  lcd.print("Sub Menu 4");
  lcd.setBacklight(TEAL);
  backbutton ();
}

void subItem5() { // Function executes when you select the 5th item from main menu
  lcd.clear();
  backArrow();
  lcd.setCursor(3, 0);
  lcd.print("Sub Menu 5");
  lcd.setBacklight(BLUE);
  backbutton ();
}

void subItem6() { // Function executes when you select the 6th item from main menu
  lcd.clear();
  backArrow();
  lcd.setCursor(3, 0);
  lcd.print("Sub Menu 6");
  lcd.setBacklight(VIOLET);
  backbutton ();
}

void subItem7() { // Function executes when you select the 7th item from main menu
  lcd.clear();
  backArrow();
  lcd.setCursor(3, 0);
  lcd.print("Sub Menu 7");
  lcd.setBacklight(RED);
  backbutton ();
}

void subItem8() { // Function executes when you select the 8th item from main menu
  lcd.clear();
  backArrow();
  lcd.setCursor(3, 0);
  lcd.print("Sub Menu 8");
  lcd.setBacklight(YELLOW);
  backbutton ();
}

void subItem9() { // Function executes when you select the 9th item from main menu
  lcd.clear();
  backArrow();
  lcd.setCursor(3, 0);
  lcd.print("Sub Menu 9");
  lcd.setBacklight(GREEN);
  backbutton ();
}

void subItem10() { // Function executes when you select the 10th item from main menu
  lcd.clear();
  backArrow();
  lcd.setCursor(3, 0);
  lcd.print("Sub Menu 10");
  lcd.setBacklight(TEAL);
  backbutton ();
}

void subItem11() { // Function executes when you select the 10th item from main menu
  lcd.clear();
  backArrow();
  lcd.setCursor(3, 0);
  lcd.print("Sub Menu 11");
  lcd.setBacklight(GREEN);
  backbutton ();
}
void subItem12() { // Function executes when you select the 10th item from main menu
  lcd.clear();
  backArrow();
  lcd.setCursor(3, 0);
  lcd.print("Sub Menu 12");
  lcd.setBacklight(TEAL);
  backbutton ();
}
