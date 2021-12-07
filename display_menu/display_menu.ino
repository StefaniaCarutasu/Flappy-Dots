#include<EEPROM.h>

#include "joystick_variables.h";
#include "lcd_variables.h";
#include "matrix_variables.h";
#include "menu_variables.h";
#include "game_variables.h";

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

  pinMode(pinSW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinSW), checkSw, FALLING);

  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);
}

void loop() {
  readJoystick();

  if (!gameStarted) {
    switchMenues();
  }
}

void checkSw() {
  // trying to eliminate the possible noise by waiting a few miliseconds before doing something
  if (millis() - lastDebounceTime > debounceInterval) {
    if (!gameStarted) {
      currentMenuToDisplay = mainMenuItems[displayedItems[currentRow]];
      changedMenu = !changedMenu;
      resetMenuVariables();
      lcd.clear();
    }

  }

  lastDebounceTime = millis();
}

void displayGreeting() {
  int initialPos;
  initialPos = (displayCols - greetingMessage.length()) / 2;
  lcd.setCursor(initialPos, 0);
  lcd.print(greetingMessage);
  delay(5000);
  lcd.clear();
}

void resetMenuVariables() {
  currentMenuItem = 0;
  currentRow = 0;
  displayedItems[0] = 0;
  displayedItems[1] = 1;
}

String scrollLCDLeft(String toBeDisplayed) {
  String result;
  String StrProcess = "              " + toBeDisplayed + "               ";
  result = StrProcess.substring(L1, L11);
  L1++;
  L11++;
  if (L1 > StrProcess.length()) {
    L1 = 15;
    L11 = 0;
  }

  return result;
}

void switchMenues() {
  navigateMainMenu();
  if (currentMenuToDisplay == "High Scores") {
    // dispay the high scores menu
    if (changedMenu) {
      changedMenu = !changedMenu;
    }
    displayHighScores();
  }
  else if (currentMenuToDisplay == "Settings") {
    // dispay the settings menu
    if (changedMenu) {
      changedMenu = !changedMenu;
    }
    displaySettings();
  }
  else if (currentMenuToDisplay == "About") {
    // dispay the settings menu
    if (changedMenu) {
      changedMenu = !changedMenu;
    }
    displayAbout();
  }
  else if (currentMenuToDisplay == "Start Game") {
    // dispay the settings menu
    if (changedMenu) {
      changedMenu = !changedMenu;
    } 
    game();
  }
  else {
    // display the main menu
    if (changedMenu) {
      changedMenu = !changedMenu;
    }
    displayMainMenu();
  }
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
  delay(350);
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

void displayHighScores() {

}

void displaySettings() {
  // display the items in the menu that are indicated by the array
  lcd.setCursor(0, 0);
  if (currentRow == 0) {
    lcd.write(rightArrow);
    lcd.setCursor(1, 0);
  }

  lcd.print(settings[displayedItems[0]]);

  lcd.setCursor(0, 1);
  if (currentRow == 1) {
    lcd.write(rightArrow);
    lcd.setCursor(1, 1);
  }

  lcd.print(settings[displayedItems[1]]);

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

void displayAbout() {
  lcd.setCursor(0, 0);
  if (currentRow == 0) {
    lcd.write(rightArrow);
    lcd.setCursor(1, 0);
  }

  if (about[displayedItems[0]].length() > 15) {
    lcd.print(scrollLCDLeft(about[displayedItems[0]]));
    delay(350);
  }
  else {
    lcd.print(about[displayedItems[0]]);
  }


  lcd.setCursor(0, 1);
  if (currentRow == 1) {
    lcd.write(rightArrow);
    lcd.setCursor(1, 1);
  }

  if (about[displayedItems[1]].length() > 15) {
    lcd.print(scrollLCDLeft(about[displayedItems[1]]));
    delay(350);
  }
  else {
    lcd.print(about[displayedItems[1]]);
  }

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

void game() {
  gameStarted = !gameStarted;
  lcd.clear();
  lcd.print("Game Started");
  delay(2000);
  lcd.clear();
}
