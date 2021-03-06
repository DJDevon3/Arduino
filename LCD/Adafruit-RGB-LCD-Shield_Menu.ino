/*********************************************************************************************
ADAFRUIT NEGATIVE RGB LCD SHIELD MENU
Example menu code for the Adafruit NEGATIVE RGB LCD Shield using the Adafruit RGB LCD Library
Adafruit Negative RGB Shield Product Page: https://www.adafruit.com/product/714

Coded by DJDevon3 https://github.com/DJDevon3/Arduino
Adapted code by PaulSS for a LiquidCrystal.h LCD.
https://www.instructables.com/id/Arduino-Uno-Menu-Template/

If you have code examples to make this menu better submit a pull request on my github. :)
***********************************************************************************************/
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

#define RED 0x1 // Every backlight color available for the negative RGB backlight shield.
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7
#define DISABLE 0x8 // Turns display completely off, will "wake" display upon button press. Unused in this sketch but nice to have!

const int BaudRate = 115200;

/* MAIN MENU ITEM ARRAY: 
 *  Name items whatever you want. Array is auto counted. No variable for adjusting amount of items is required. :) 
 *  To have more than 12 add switch/cases and void functions. To have less than 12 no modifications required.
*/
int MainArray [0][12];  // 1 Main Menu Column with 12 Rows.  These numbers must match your string array items.
String menuItems[] = {"ITEM 1", "ITEM 2", "ITEM 3", "ITEM 4", "ITEM 5", "ITEM 6", "ITEM 7", "ITEM 8", "ITEM 9", "ITEM 10", "ITEM 11", "ITEM 12"};
String colors[] = {"RED", "YELLOW", "GREEN", "TEAL", "BLUE", "VIOLET", "WHITE"};

#define NUMITEMS(menuItems) ((unsigned int) (sizeof (menuItems) / sizeof (menuItems [0]))) // Item array auto counter
#define BGCOLORS(colors) ((unsigned int) (sizeof (colors) / sizeof (colors [0]))) // Item array auto counter
int parameters = (NUMITEMS(menuItems));  // Number of menuItems
int maxMenuPages = (NUMITEMS (menuItems)-2); // Number of pages
 
int readKey; // Holds value of keypress during menu navigation
int menuPage = 0;
int button = 0;
int cursorPosition = 0;

/* CUSTOM LCD CHARACTERS 
 *  Creates custom arrow characters for the menu display (in hex). 
 *  You can only create a maximum of 8 custom characters with this library. Choose wisely. 6 are already here for you.
*/
byte rightArrow[8] = {0x10,0x08,0x04,0x02,0x04,0x08,0x10,0x00};
byte upArrow[8] = {0x04,0x0E,0x1F,0x04,0x04,0x04,0x04,0x00};
byte downArrow[8] = {0x04,0x04,0x04,0x04,0x1F,0x0E,0x04,0x00};
byte leftArrow[8] = {0x01,0x02,0x04,0x08,0x04,0x02,0x01,0x00};
byte cancelMark[8] = {0x0,0x1b,0xe,0x4,0xe,0x1b,0x0}; // unused, available for value cancel confirmations
byte checkMark[8] = {0x0,0x1,0x3,0x16,0x1c,0x8,0x0}; // unused, available for value set confirmations

void NavRight(){ // Creates custom character display as functions for easy use within other functions.
  lcd.write(byte(0));
}
void NavUp(){ // menu navigation up arrow
  lcd.write(byte(1));
}
void NavDown(){ // menu navigation down arrow
  lcd.write(byte(2));
}
void NavLeft(){ // menu navigation left arrow
  lcd.write(byte(3));
}
void NavCancel(){ // menu navigation cancel
  lcd.write(byte(4));
}
void NavConfirm(){ // menu navigation checkmark
  lcd.write(byte(5));
}

void setup() {
  Serial.begin(BaudRate); // Configure this in initialized variables. Const int so you can lcd.print it to your display if you want.
  lcd.begin(16, 2); // set up the LCD's number of columns and rows:
  lcd.createChar(0, rightArrow); // Tranlates custom arrow characters from hex to lcd. Now initialized for global use after lcd.begin
  lcd.createChar(1, upArrow);
  lcd.createChar(2, downArrow);
  lcd.createChar(3, leftArrow);
  lcd.createChar(4, cancelMark);
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

/* EVERY PAGE HAS 2 MAIN MENU ITEMS. 
 *  Don't get pages confused with items. 
 *  Draws 2 menu items on each page of the main menu. 
 *  You scroll up/down through both items and pages.
 *  Printed to serial monitor (not LCD) for debugging.
*/
void mainMenuDraw() {
  lcd.setBacklight(WHITE);
  //Serial.print("Menu Page -> " );
  //Serial.println(menuPage);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(menuItems[menuPage]);
  lcd.setCursor(1, 1);
  lcd.print(menuItems[menuPage + 1]);
  if (menuPage == 0) {
    lcd.setCursor(15, 1);
    NavDown();
  } else if (menuPage > 0 and menuPage < maxMenuPages) {
    lcd.setCursor(15, 1);
    NavDown();
    lcd.setCursor(15, 0);
    NavUp();
  } else if (menuPage == maxMenuPages) {
    lcd.setCursor(15, 0);
    NavUp();
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
      NavRight();
    }
    if (cursorPosition % 2 != 0) {  // If the menu page is even and the cursor position is odd that means the cursor should be on line 2
      lcd.setCursor(0, 1);
      NavRight();
    }
  }
  if (menuPage % 2 != 0) {
    if (cursorPosition % 2 == 0) {  // If the menu page is odd and the cursor position is even that means the cursor should be on line 2
      lcd.setCursor(0, 1);
      NavRight();
    }
    if (cursorPosition % 2 != 0) {  // If the menu page is odd and the cursor position is odd that means the cursor should be on line 1
      lcd.setCursor(0, 0);
      NavRight();
    }
  }
}
/*
 * MAIN MENU SWITCH CASES
 */
void operateMainMenu() {
  int activeButton = 0;
  while (activeButton == 0) {
    readKey = lcd.readButtons();
    if (readKey == 0) {
      delay(100);
      readKey = lcd.readButtons();
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 0: // When button returns as 0 there is no action taken
        break;
      case 1:  // This case will execute if the "select" button is pressed  
      case 2:  // This case will execute if the "forward" button is pressed
        button = 0;
        switch (cursorPosition) { // The case selected here = menu page + cursor position.
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
      case 8: // This case will execute if the "up" button is pressed
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
      case 4: // This case will execute if the "down" button is pressed
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

/* BUTTON PRESS EVALUATOR
 * BUTTON READING STATE = 0
 * BUTTON_RIGHT = 2
 * BUTTON_UP = 8
 * BUTTON_DOWN = 4
 * BUTTON_LEFT = 16
 * BUTTON_SELECT = 1
 * 
 * LCD STATES
 * BUTTON READING STATE = 0
 * BUTTON_RIGHT = 1
 * BUTTON_UP = 2
 * BUTTON_DOWN = 3
 * BUTTON_LEFT = 4
 * BUTTON_SELECT = 5
 */
int evaluateButton(int x) {
  uint8_t buttons = lcd.readButtons();
  int result = 0;
  if (buttons & BUTTON_RIGHT) {
    result = 2; // right
    Serial.print("Button Right ");
    Serial.println (BUTTON_RIGHT); // for debugging serial values
  } else if (buttons & BUTTON_UP) {
    result = 8; // up
    Serial.print("Button Up ");
    Serial.println (BUTTON_UP); // for debugging serial values
  } else if (buttons & BUTTON_DOWN) {
    result = 4; // down
    Serial.print("Button Down ");
    Serial.println (BUTTON_DOWN); // for debugging serial values
  } else if (buttons & BUTTON_LEFT) {
    result = 16; // left
    Serial.print("Button Left ");
    Serial.println (BUTTON_LEFT); // for debugging serial values
  } else if (buttons & BUTTON_SELECT) { // currently unused work in progress
    result = 1; // select
    Serial.print("Button Select ");
    Serial.println (BUTTON_SELECT); // for debugging serial values
  }
  delay (100);
  return result;
}

void backbutton (){ // Left button navigates back until mainmenu
   int activeButton = 0;
     while (activeButton == 0) {
     uint8_t button = lcd.readButtons();
    readKey = lcd.readButtons();
    if (button & BUTTON_LEFT) {
      // delay(50);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 16:  // This case will execute if the "back" button is pressed
        button = 0;
        activeButton = 1;
        lcd.setBacklight(WHITE);
        break;
    }
  } 
}

/*
 * SUB MENU PAGES
 * For now we'll use functions. Someday use arrays and loops to make it dynamic.
 */
 void subCursor() { // clears screen adds back cursor to each subItem screen
  lcd.clear();
  lcd.setCursor(0,0);
  NavLeft();
  lcd.setCursor(2, 0);
}

void subItem1() { // Function executes when you select the 1st item from main menu
  subCursor();
  lcd.print("Sub Menu 1");
  lcd.setBacklight(RED);
  lcd.setCursor(0, 1);
  lcd.print("Cylon 1 ");
  // Do something such as include Cylon(); FastLED function
  NavConfirm();
  backbutton ();
}

void subItem2() { // Function executes when you select the 2nd item from main menu
  subCursor();
  lcd.print("Sub Menu 2");
  lcd.setBacklight(YELLOW);
    lcd.setCursor(0, 1);
  lcd.print("Rainbow Glitter ");
  NavConfirm();
  backbutton ();
}

void subItem3() {
  subCursor();
  lcd.print("Sub Menu 3");
  lcd.setBacklight(GREEN);
    lcd.setCursor(0, 1);
  lcd.print("Audio Cylon 1 ");
  NavConfirm();
  backbutton ();
}

void subItem4() {
  subCursor();
  lcd.print("Sub Menu 4");
  lcd.setBacklight(TEAL);
      lcd.setCursor(0, 1);
  lcd.print("Audio RB 1 ");
  NavConfirm();
  backbutton ();
}

void subItem5() {
  subCursor();
  lcd.setCursor(3, 0);
  lcd.print("Sub Menu 5");
  lcd.setBacklight(BLUE);
  backbutton ();
}

void subItem6() {
  subCursor();
  lcd.print("Sub Menu 6");
  lcd.setBacklight(VIOLET);
  backbutton ();
}

void subItem7() {
  subCursor();
  lcd.print("Sub Menu 7");
  lcd.setBacklight(RED);
  backbutton ();
}

void subItem8() {
  subCursor();
  lcd.print("Sub Menu 8");
  lcd.setBacklight(YELLOW);
  backbutton ();
}

void subItem9() {
  subCursor();
  lcd.print("Sub Menu 9");
  lcd.setBacklight(GREEN);
  backbutton ();
}

void subItem10() {
  subCursor();
  lcd.print("Sub Menu 10");
  lcd.setBacklight(TEAL);
  backbutton ();
}

void subItem11() {
  subCursor();
  lcd.print("Sub Menu 11");
  lcd.setBacklight(GREEN);
  backbutton ();
}
void subItem12() {
  subCursor();
  lcd.print("Sub Menu 12");
  lcd.setBacklight(TEAL);
  backbutton ();
}

void loop() {
  mainMenuDraw();
  drawCursor();
  operateMainMenu();
}
