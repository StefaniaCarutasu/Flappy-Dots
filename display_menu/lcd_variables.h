#include<LiquidCrystal.h>

// lcd variables
const int V0 = 6;
const int RS = 8;
const int enable = 9;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 7;


LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);

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

const int downArrow = 0;
const int upArrow = 1;
const int rightArrow = 2;

// Scrolling text variables
int L1 = 15;
int L11 = 0;
