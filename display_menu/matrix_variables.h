#include<LedControl.h>

// matrix variables
const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); //DIN, CLK, LOAD, No. DRIVER

byte matrixBrightness = 2;
