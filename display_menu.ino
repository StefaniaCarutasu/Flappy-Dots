#include<LiquidCrystal.h>
#include<LedControl.h>
#include<EEPROM.h>

// joystick variables
const int pinX = A0; // A0 - analog pin connected to X output
const int pinY = A1; // A1 - analog pin connected to Y output

int xValue = 0;
int yValue = 0;

bool joyMoved = false;

const int minThreshold = 200;
const int maxThreshold = 600;

// matrix variables
const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); //DIN, CLK, LOAD, No. DRIVER

byte matrixBrightness = 2;

// lcd variables 
const int V0 = 6;
const int RS = 8;
const int enable = 9;
const int d4 = 5;
const int d5 = 4; 
const int d6 = 3;
const int d7 = 2;

LiquidCrystal lcd(RS,enable,d4,d5,d6,d7);

const int displayCols = 16;
const int displayRows = 2;

// down arrow
byte downArrowByte[8] = {
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B11111,
  B01110,
  B00100
};

// up arrow
byte upArrowByte[8] = {
  B00100,
  B01110,
  B11111,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100
};

// right arrow
byte rightArrowByte[8] = {
  B01000,
  B00100,
  B00010,
  B00001,
  B00001,
  B00010,
  B00100,
  B01000
};

// The message displayed before the menu
const String greetingMessage = "Sssnake";

// Main menu items 
String mainMenuItems[] = {
  "Start Game",
  "High Scores", 
  "Settings",
  "About"  
};

int currentMenuItem = 0;
int currentRow = 0;
int displayedItems[2] = {0, 1};

// Submenus items 

String settings[] = {
  "LCD Constrast",
  "LCD Brightness"
  "Matrix Brightness"
};

int contrasts[3] = {100, 120, 140};

const int downArrow = 0;
const int upArrow = 1;
const int rightArrow = 2;

void setup() {
  Serial.begin(9600);
  
  analogWrite(V0, contrasts[1]);
  
  lcd.createChar(0, downArrowByte);
  lcd.createChar(1, upArrowByte);
  lcd.createChar(2, rightArrowByte);
  
  lcd.begin(16, 2);
  displayGreeting();

  // setup matrix
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen
}

void loop() {
  readJoystick();
  navigateMainMenu();
  displayMainMenu();
}

void displayGreeting() {
  int initialPos;
  initialPos = (displayCols - greetingMessage.length()) / 2;
  lcd.setCursor(initialPos, 0);
  lcd.print(greetingMessage);
  delay(5000);
  lcd.clear();
}

void displayMainMenu() {
  // display the items in the menu that are indicated by the array 
  lcd.setCursor(0, 0); 
  if (currentRow == 0) {
    lcd.write(rightArrow);
    lcd.setCursor(1, 0);
  }
  
  lcd.print(mainMenuItems[displayedItems[0]]);

  lcd.setCursor(0, 1);
  if (currentRow == 1) {
    lcd.write(rightArrow);
    lcd.setCursor(1, 1);
  }
  
  lcd.print(mainMenuItems[displayedItems[1]]);
  
  // if i am on the last item of the menu, i display the arrow pointing upwards
  if (displayedItems[1] == 3) {
    lcd.setCursor(15, 0); 
    lcd.write(upArrow);
  }
  // if i am on the first item of the menu, i display the arrow pointing downwards
  else if (displayedItems[0] == 0) {
    lcd.setCursor(15, 1); 
    lcd.write(downArrow);
  }
  // i display both arrows to show the player that they can go either way
  else {
    lcd.setCursor(15, 0); 
    lcd.write(upArrow);
    lcd.setCursor(15, 1); 
    lcd.write(downArrow);
  }
}

void readJoystick() {
  yValue = analogRead(pinY);  
  xValue = analogRead(pinX);
}

void navigateMainMenu() {
  // go upwards
  if (yValue > maxThreshold && joyMoved == false) {
    lcd.clear();
    joyMoved = true; 

    if (currentRow == 1) {
      currentRow = 0;
      currentMenuItem -= 1;
    }
    else {
      if (currentMenuItem != 0) {
        currentRow = 1 - currentRow;
        displayedItems[0] -= 1;
        displayedItems[1] -= 1;
      }
    }   
  }

  if (yValue < minThreshold && joyMoved == false) {
    lcd.clear();
    joyMoved = true; 

    if (currentRow == 0) {
      currentRow = 1;
      currentMenuItem += 1;
    }
    else {
      if (currentMenuItem != 3) {
        currentRow = 1 - currentRow;
        displayedItems[0] += 1;
        displayedItems[1] += 1;
      }
    }
  }

  if (minThreshold <= yValue && yValue <= maxThreshold) {
    joyMoved = false;
 }
}
