/*
 * PROJECT ARCHITECTURE
*flappy_dots.ino: main file of the project where the setup, loop and most of the functionalities are written.
*buzzer_variables.h: constains all the variables for the buzzer such as the pin, the theme song, the EEPROM address where the sound option is stored, and functions for playing the different available sounds.
*game_variables.h: contains all the variables needed for the game logic such as the position of the bird, the shape, posion and speed of the obstacles, the level and score all the variables needed to enter and store the player's name.
*highscore.h: contains all the variables and functions needed to store and retrieve the highscores. The score achieved by the player is compared with the existing ones. If is surpassed any of the existing scores, they are shifted to make room for the new one. The file also contains the functions for writting and retrieving integres from EEPROM.
*joystick_variables.h: contains all the variables needed to operate de joystick such as the pins used, the debounce interval for the button and the min and max thresholds.
*lcd_variables.h: constains all the variables used for the LCD such as the pins, the EEPROM addresses used to store the values for brightness and contrast, and the icons for the arrows displayed.
*matrix_variables.h: contains all the variables used for the matrix such as the pins, the matrixed used in the game and the matrices used to display the animations
*menu_variables.h: contains all the variables needed to display all the available menus such as lists of elements to be displayed, the current menu, the last displayed menu, the current selected item and the current row.
*messages.h: contains all the messages displayed on the lcd, before, during and after the game
*pitches.h: constains musical pitches used to make the theme song and the rest of the sounds.
*system_states.h: contains all the possible states of the system. in the loop function, the state of the sistem is checked to check the phase of the game and call the needed functions accordingly.
*/

/*
 * MOST IMPORTANTS FUNCTIONS
 * -> moveBird(): receives info from the joystick and moves the bird accordingly
 * -> moveObstacle(): moves the obstacle from the last column towards the first
 * -> autoDecreaseBird(): if the player hasn't moved the bird during an interval, the bird is decreased automatically
 */
#include "system_states.h";
#include "joystick_variables.h";
#include "lcd_variables.h";
#include "matrix_variables.h";
#include "menu_variables.h";
#include "game_variables.h";

#include "highscore.h";
#include "buzzer_variables.h"

void setup() {
  //Serial.begin(9600);
  pinMode(V0, OUTPUT);

  //EEPROM.update(lcd_contrast_address, 50);
  analogWrite(V0, EEPROM.read(lcd_contrast_address));

  pinMode(A, OUTPUT);
  analogWrite(A,  EEPROM.read(lcd_brightness_address));

  lcd.createChar(0, downArrowByte);
  lcd.createChar(1, upArrowByte);
  lcd.createChar(2, rightArrowByte);

  lcd.begin(displayCols, displayRows);

  // setup matrix
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, EEPROM.read(matrix_brightness_address)); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen

  // joystick setup
  pinMode(pinSW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinSW), handleInterrupt, FALLING);

  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);

  playMusic = readIntFromEEPROM(musicChoiceAddress);

  if (playMusic) {
    settings[4] = "Sound Off";
  }
  else {
    settings[4] = "Sound On";
    noTone(buzzerPin);
  }

  getHighScores();

}

void loop() {
  readJoystick();

  if (SYSTEM_STATE == OPENING_SCREEN) {
    displayGreeting();
  }
  else if (SYSTEM_STATE == MENU_SCREEN) {
    if (playMusic) {
      if (millis() - lastTone > toneInterval) {
        if (note == songLength) {
          note = 0;
        }
        playMelody();
        lastTone = millis();
      }
    }
    else {
      noTone(buzzerPin);
    }

    switchMenues();
  }
  else if (SYSTEM_STATE == START_GAME_SCREEN) {
    displayStartGameMessage();
    initialliseGame();
  }
  else if (SYSTEM_STATE == IN_GAME_SCREEN) {
    gameLogic();
  }
  else if (SYSTEM_STATE == GAME_WON_SCREEN) {
    finishedGameScreen(gameWonMessage);
  }
  else if (SYSTEM_STATE == GAME_LOST_SCREEN) {
    if (playMusic) {
      playDeathPitch();
    }
    displayXAnimation();
    finishedGameScreen(gameLostMessage);
  }
  else if (SYSTEM_STATE == NAME_UPDATE_SCREEN) {
    navigateName();
  }
}

void readJoystick() {
  yValue = analogRead(pinY);
  xValue = analogRead(pinX);
}

void handleInterrupt() {
  unsigned long long current = millis();
  if (current - lastDebounceTime > debounceInterval) {
    lastDebounceTime = current;
    checkSw();
  }
}

/*
 * if the SYSTEM_STATE is MENU_SCREEN, the click of the button can mean changing the current menu or changing the settings
 * if the SYSTEM_STATE is NAME_UPDATE_SCREEN, the click of the button can mean locking the letter (if the player is currently on the first row) or setting the name (if the player is on the second row)
 */

void checkSw() {
  if (SYSTEM_STATE == MENU_SCREEN ) {
    if (lastDisplayedMenu == "LCD Constrast") {
      setLCDContrast(currentItem);
    }
    else if (lastDisplayedMenu == "LCD Bright.") {
      setLCDBrightness(currentItem);
    }
    else if (lastDisplayedMenu == "Mat. Bright.") {
      setMatrixBrightness(currentItem);
    }
    if (currentItem == "Reset Scores") {
      resetScores();
    }
    else if (currentItem == "Sound Off") {
      playMusic = 0;
      settings[4] = "Sound On";
      writeIntIntoEEPROM(musicChoiceAddress, playMusic);
    }
    else if (currentItem == "Sound On") {
      playMusic = 1;
      settings[4] = "Sound Off";
      writeIntIntoEEPROM(musicChoiceAddress, playMusic);
    }
    if (currentItem == "Reset Scores") {
      lastDisplayedMenu = "Setttings";
      currentMenuToDisplay == "Main";
    }
    else {
      lastDisplayedMenu = currentMenuToDisplay;
      currentMenuToDisplay = currentItem;
    }
    lcd.clear();
    changedMenu = !changedMenu;
    resetMenuVariables();
  }
  else if (SYSTEM_STATE == NAME_UPDATE_SCREEN) {
    if (currentRow == 0) {
      lockedLetter = !lockedLetter;
      currentLetter = playerName[currentLetterPosition];
    }
    else {
      setName();
    }
  }
}

/*
   displays the name of the game
*/

void displayGreeting() {
  displayInitialAnimation();
  int initialPos;
  initialPos = (displayCols - sizeof(greetingMessage)) / 2;
  lcd.setCursor(initialPos, 0);
  lcd.print(greetingMessage);
  delay(5000);
  lcd.clear();

  SYSTEM_STATE = MENU_SCREEN;
}

/*
   resets the variables used to handle the menu
*/

void resetMenuVariables() {
  currentMenuItem = 0;
  currentRow = 0;
  displayedItems[0] = 0;
  displayedItems[1] = 1;
}

String scrollLCDLeft(String toBeDisplayed) {
  String result;
  //String StrProcess = toBeDisplayed;
  String StrProcess = toBeDisplayed + "               ";
  result = StrProcess.substring(L1, L11);
  L1++;
  L11++;
  if (L1 > StrProcess.length()) {
    L1 = 14;
    L11 = 0;
  }

  return result;
}


/*
   displays a message indicating the game has started
*/
void displayStartGameMessage() {
  lcd.clear();
  int initialPos;
  initialPos = (displayCols - sizeof(gameStartedMessage)) / 2;
  lcd.setCursor(initialPos, 0);
  lcd.print(gameStartedMessage);
  delay(5000);
  lcd.clear();
}

/*
   switching between menues
   checks the currentMenuToDisplay variable and shows the menu accordingly
   it cleares the display so the next menu can be displayed
*/
void switchMenues() {
  navigateMainMenu();
  if (currentMenuToDisplay == "High Scores") {
    // dispay the high scores menu
    if (changedMenu) {
      lastDisplayedMenu = currentMenuToDisplay;
      changedMenu = !changedMenu;
      lcd.clear();
    }
    displayMenu(scores);
  }
  else if (currentMenuToDisplay == "Settings") {
    // dispay the settings menu
    if (changedMenu) {
      lastDisplayedMenu = currentMenuToDisplay;
      changedMenu = !changedMenu;
      lcd.clear();
    }
    displayMenu(settings);
  }
  else if (currentMenuToDisplay == "About") {
    // dispay the about menu
    if (changedMenu) {
      lastDisplayedMenu = currentMenuToDisplay;
      changedMenu = !changedMenu;
      lcd.clear();
    }
    displayMenu(about);
  }
  else if (currentMenuToDisplay == "Start Game") {
    // start the game
    if (changedMenu) {
      lastDisplayedMenu = currentMenuToDisplay;
      lcd.clear();
      changedMenu = !changedMenu;
    }
    SYSTEM_STATE = START_GAME_SCREEN;
  }
  else if (currentMenuToDisplay == "LCD Constrast") {
    // dispay the settings menu
    if (changedMenu) {
      lastDisplayedMenu = currentMenuToDisplay;
      changedMenu = !changedMenu;
      lcd.clear();
    }
    displayMenu(contrasts);
  }
  else if (currentMenuToDisplay == "LCD Bright.") {
    // dispay the settings menu
    if (changedMenu) {
      lastDisplayedMenu = currentMenuToDisplay;
      changedMenu = !changedMenu;
      lcd.clear();
    }

    displayMenu(contrasts);
  }
  else if (currentMenuToDisplay == "Mat. Bright.") {
    // dispay the settings menu
    if (changedMenu) {
      lastDisplayedMenu = currentMenuToDisplay;
      changedMenu = !changedMenu;
      lcd.clear();
    }
    displayMenu(contrasts);
  }
  else if (currentMenuToDisplay == "Back") {
    // display the main menu
    if (changedMenu) {
      lastDisplayedMenu = currentMenuToDisplay;
      changedMenu = !changedMenu;
      lcd.clear();
    }
    currentMenuToDisplay = "Main";
    displayMenu(mainMenuItems);
  }
  else {
    // display the main menu
    if (changedMenu) {
      lastDisplayedMenu = currentMenuToDisplay;
      changedMenu = !changedMenu;
      lcd.clear();
    }

    currentMenuToDisplay = "Main";
    displayMenu(mainMenuItems);
  }
}

/*
   in menu navigation
   it hovers between the elements of the menu accordingly to the joystick's movement
   it overwrites the variables with what needs to be displayed
   it memorises the the current row
*/
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
      int noOfItems;
      if (currentMenuToDisplay == "Settings") {
        noOfItems = 6;
      }
      else {
        noOfItems = 4;
      }
      if (currentMenuItem != noOfItems - 1) {
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

/*
    displayes the menu passed as variable
    on the current row, an arrow is displayed to indicate the current position
    if the menu item has more than 14 letters, the item is scrolled on the lcd
    on the right side there are arrows pointing towards the direction in which the player can scroll
*/
void displayMenu(String menu[]) {
  String top = menu[displayedItems[0]];
  String bottom = menu[displayedItems[1]];
  // display the items in the menu that are indicated by the array
  lcd.setCursor(0, 0);
  if (currentRow == 0) {
    lcd.write(rightArrow);
    lcd.setCursor(1, 0);
    currentItem = top;
  }
  if (top.length() > 14) {
    lcd.print(scrollLCDLeft(top));
    delay(350);
  }
  else {
    lcd.print(top);
  }

  lcd.setCursor(0, 1);
  if (currentRow == 1) {
    lcd.write(rightArrow);
    lcd.setCursor(1, 1);
    currentItem = bottom;
  }

  if (bottom.length() > 14) {
    lcd.print(scrollLCDLeft(bottom));
    delay(350);
  }
  else {
    lcd.print(bottom);
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

/*
   the parametre of the function is received accordingly to the player's choice in the menu
   the value is update in EEPROM
*/
void setLCDContrast(String contrast) {
  int contrastValue;

  if (contrast == "Low") {
    contrastValue = constrastValues[0];

  }
  else if (contrast == "Medium") {
    contrastValue = constrastValues[1];

  }
  else if (contrast == "High") {
    contrastValue = constrastValues[2];
  }

  analogWrite(V0, contrastValue);
  EEPROM.update(lcd_contrast_address, contrastValue);

  lcd.clear();
  currentMenuToDisplay = "Settings";
}

/*
   the parametre of the function is received accordingly to the player's choice in the menu
   the value is update in EEPROM
*/
void setLCDBrightness(String brightness) {
  int brightnessValue;

  if (brightness == "Low") {
    brightnessValue = brightnessLCDValues[0];
  }
  else if (brightness == "Medium") {
    brightnessValue = brightnessLCDValues[1];
  }
  else if (brightness == "High") {
    brightnessValue = brightnessLCDValues[2];
  }

  analogWrite(A, brightnessValue);
  EEPROM.update(lcd_brightness_address, brightnessValue);

  lcd.clear();
  currentMenuToDisplay = "Settings";

}

/*
   the parametre of the function is received accordingly to the player's choice in the menu
   the value is update in EEPROM
*/
void setMatrixBrightness(String brightness) {
  int brightnessValue;

  if (brightness == "Low") {
    brightnessValue = brightnessMatrixValues[0];
  }
  else if (brightness == "Medium") {
    brightnessValue = brightnessMatrixValues[1];
  }
  else if (brightness == "High") {
    brightnessValue = brightnessMatrixValues[2];
  }

  lc.setIntensity(0, brightnessValue);
  EEPROM.update(matrix_brightness_address, brightnessValue);

  lcd.clear();
  currentMenuToDisplay = "Settings";
}

/*
   the function is responsible for implementing the game logic
   the current level and scored are displayed
   if an obstacle hasn't been generated yet or the bird has passed it, a new one is generated
   the obstacle is moved automatically
   the player has the option to move the bird from the joystick
   if the player hasn't moved the bird, it decreases automatically
   if there are any changes, the led matrix is updated
*/
void gameLogic() {
  displayCurrentLevel();
  if (generated == 0) {
    generateObstacle();
    generated = 1;
  }
  moveObstacle();
  moveBird();
  autoDecreaseBird();
  if (matrixChanged == true) {
    // matrix display logic
    updateMatrix();
    matrixChanged = false;
  }
}

/*
   the matrix is cleared
   the state of the system becomes IN_GAME_STATE
   the bird is on it's initial position
   the display is cleared
*/
void initialliseGame() {
  lc.clearDisplay(0);
  SYSTEM_STATE = IN_GAME_SCREEN;
  matrix[currentBirdPosition[0][0]][currentBirdPosition[0][1]] = 1;
  matrix[currentBirdPosition[1][0]][currentBirdPosition[1][1]] = 1;
  lcd.clear();
}


/*
   the function updates the led matrix accordingly to the changes on the game matrix
*/
void updateMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, matrix[row][col]);
    }
  }
}

/*
   the function updates the led matrix with the bird animation displayed while the player scrolls through the menu
*/

void displayInitialAnimation() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, birdAnimationMatrix[row][col]);
    }
  }
}

/*
   the function updates the led matrix with the trophy animation displayed when the player has surpassed a highscore
*/
void displayTrophyAnimation() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, trophyAnimationMatrix[row][col]);
    }
  }
}

/*
   the function updates the led matrix with the X animation displayed when the player has lost the game
*/
void displayXAnimation() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, xAnimationMatrix[row][col]);
    }
  }
}


/*
   before the bird has changed position, the current position is stored
*/
void copyBirdPosition() {
  for (int i = 0; i < maxBirdSize; i++) {
    for (int j = 0; j < maxBirdSize; j++) {
      lastBirdPosition[i][j] = currentBirdPosition[i][j];
    }
  }
}


/*
   the function updates the matrix accordingly to the bird's movement
*/
void updateBirdMatrix() {

  matrix[lastBirdPosition[0][0]][lastBirdPosition[0][1]] = 0;
  matrix[lastBirdPosition[1][0]][lastBirdPosition[1][1]] = 0;

  matrix[currentBirdPosition[0][0]][currentBirdPosition[0][1]] = 1;
  matrix[currentBirdPosition[1][0]][currentBirdPosition[1][1]] = 1;
}


/*
   the moment the bird last moved is updated (it's needed to automatically decrease the bird if the player hasn't moved)
   if the player has moved the joystick upwards, the bird moves one position upwards (if it has space on the matrix)
   if the sound is on, a pitch is played with the movement

   the same logic is behind the downwards movement
*/
void moveBird() {
  readJoystick();

  copyBirdPosition();

  if (yValue > maxThreshold && joyMoved == false) {
    joyMoved = true;
    lastMoved = millis();
    if (currentBirdPosition[1][0] > 0) {
      birdMoved = 1;
      currentBirdPosition[1][0]--;
      currentBirdPosition[0][0]--;

      if (playMusic) {
        playMoveUpPitch();
      }
    }
  }

  if (yValue < minThreshold && joyMoved == false) {
    joyMoved = true;
    lastMoved = millis();
    if (currentBirdPosition[0][0] < matrixSize - 1) {
      birdMoved = 1;
      currentBirdPosition[1][0]++;
      currentBirdPosition[0][0]++;

      if (playMusic) {
        playMoveDownPitch();
      }
    }
  }

  if (minThreshold <= yValue && yValue <= maxThreshold) {
    joyMoved = false;
  }
  if (birdMoved == 1) {
    birdMoved = 0;
    matrixChanged = true;
    updateBirdMatrix();
  }

}


/*
   the moment the bird last moved is updated
   the position of the bird is incremented
   a pitch is played with the movement
   if the bird touches the last row of the matrix, the game is considered lost and the state of the system chages

*/
void autoDecreaseBird() {
  if (millis() - lastMoved > decreaseInterval) {
    lastMoved = millis();
    copyBirdPosition();
    currentBirdPosition[1][0]++;
    currentBirdPosition[0][0]++;

    if (playMusic) {
      playMoveDownPitch();
    }

    if (currentBirdPosition[0][0] == 7) {
      SYSTEM_STATE = GAME_LOST_SCREEN;
    }

    matrixChanged = true;
    updateBirdMatrix();
  }
}


/*
 * a new obstacle is generated so the matrix has changed
 * the column the obstacle is generated on is the last one of the matrix
 * a row to start the obstacle on is randomly generated
 * each obstacle consists of a column on the matrix with 3 empty spaces in in 
 */
void generateObstacle() {
  matrixChanged = true;
  obstacleColumn = 7;
  int spaceStart = random(0, 5);
  for (int i = 0; i < spaceStart; i++) {
    obstacle[i] = 1;
  }
  for (int i = spaceStart + 3; i < matrixSize; i++) {
    obstacle[i] = 1;
  }
  for (int i = 0; i < matrixSize; i++) {
    matrix[i][obstacleColumn] = obstacle[i];
  }
}


// shifts de obstacle up (shifts the space the bird can fly through)
void shiftUp() {
  int o = obstacle[0];
  if (obstacle[0] == 0) {
    o = 1;
    obstacle[3] = 1;
  }
  for (int i = 0; i < matrixSize - 2; i++) {
    obstacle[i] = obstacle[i + 1];
  }
  obstacle[matrixSize - 1] = o;
}

// shifts de obstacle down (shifts the space the bird can fly through)
void shiftDown() {
  int o = obstacle[matrixSize - 1];
  if (obstacle[matrixSize - 1] == 0) {
    o = 1;
    obstacle[4] = 1;
  }
  for (int i = matrixSize - 1; i > 1; i--) {
    obstacle[i] = obstacle[i - 1];
  }
  obstacle[0] = o;
}


/*
 * the obstacles move at a fixed interval (fixed for the level)
 * if the obstacle is on the second column and the bird touches it, the game is lost
 * if the obstacle is on the first column and the bird touches it, the game is lost
 * if the bird hasn't had contact with the obstacle, the latter one shifts one column to the left
 * if it was already on the first column, a new obstacle with similar properties and behaviour is generated
 * if the bird succesfully avoided the obstacle, the score is increased
 * if it's the case, the level is increased
 * the speed of the obstacle increased with the level
 * if the level is greater than 4, the obstacle shifts up or down, depending on the parity of the column it's on
 * if the score is greater than 50, the game is considered won
 */
void moveObstacle() {
  if (millis() - lastMovedObstacle > moveObstacleInterval) {
    // it's time to move the obstacle one column towards the bird
    matrixChanged = true;

    if (obstacleColumn == 1) {
      if (obstacle[currentBirdPosition[1][0]] == 1) {
        SYSTEM_STATE = GAME_LOST_SCREEN;
      }
      for (int i = 0; i < matrixSize; i++) {
        if (i != currentBirdPosition[1][0]) {
          matrix[i][obstacleColumn] = 0;
        }
      }
      obstacleColumn--;
      for (int i = 0; i < matrixSize; i++) {
        if (obstacleColumn != 0 && obstacleColumn != 1) {
          matrix[i][obstacleColumn] = obstacle[i];
        }
        else {
          if (i != currentBirdPosition[0][0] && i != currentBirdPosition[1][0]) {
            matrix[i][obstacleColumn] = obstacle[i];
          }
        }

      }
      if (level > 4) {
        if (level % 2) {
          if (obstacleColumn % 2) {
            shiftUp();
          }
          else {
            shiftDown();
          }
        }
        else {
          if (obstacleColumn % 2) {
            shiftDown();
          }
          else {
            shiftUp();

          }
        }

      }
    }
    else if (obstacleColumn == 0) {
      if (obstacle[currentBirdPosition[0][0]] == 1) {
        SYSTEM_STATE = GAME_LOST_SCREEN;
      }
      for (int i = 0; i < matrixSize; i++) {
        if (i != currentBirdPosition[0][0]) {
          matrix[i][obstacleColumn] = 0;
        }
      }
      obstacleColumn--;

    }
    else {
      for (int i = 0; i < matrixSize; i++) {
        matrix[i][obstacleColumn] = 0;
      }
      obstacleColumn--;
      if (obstacleColumn > -1) {
        for (int i = 0; i < matrixSize; i++) {
          if (obstacleColumn != 0 && obstacleColumn != 1) {
            matrix[i][obstacleColumn] = obstacle[i];
          }
          else {
            if (i != currentBirdPosition[0][0] && i != currentBirdPosition[1][0]) {
              matrix[i][obstacleColumn] = obstacle[i];
            }
          }

        }
        if (level > 4) {
          if (level % 2) {
            if (obstacleColumn % 2) {
              shiftUp();
            }
            else {
              shiftDown();
            }
          }
          else {
            if (obstacleColumn % 2) {
              shiftDown();
            }
            else {
              shiftUp();

            }
          }

        }

      }
      else {
        // the bird avoided the obstacle
        for (int i = 0; i < matrixSize; i++) {
          obstacle[i] = 0;
          matrix[i][obstacleColumn] = 0;
        }
        score += 1;
        if (score == 50) {
          SYSTEM_STATE = GAME_WON_SCREEN;
        }
        else if (score == changeLevelScore + 5) {
          changeLevelScore = score;
          level += 1;
          if (level <= 4) {
            moveObstacleInterval -= 100;
          }

        }
        generateObstacle();
      }
    }

    lastMovedObstacle = millis();
  }
}

/*
 * displays the current level and score as the game progresses
 */

void displayCurrentLevel() {
  if (score != previousScore) {
    previousScore = score;
    lcd.clear();
  }
  if (level != previousLevel) {
    previousLevel = level;
    lcd.clear();
  }
  String displayCurrentLevel = displayLevel + " " + String(level);
  lcd.setCursor(0, 0);
  lcd.print(displayCurrentLevel);

  String displayCurrentScore = displayScore + " " + String(score);
  lcd.setCursor(0, 1);
  lcd.print(displayCurrentScore);

}


/*
 * the parametre of the function is the message that's shown at the end of the game (a good message if the player has won and a bad one if the player has lost)
 */
void finishedGameScreen(String message) {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(message);

  enterName = true;
  currentRow = 0;
  delay(5000);

  lcd.clear();

  displayEndGameStatistics();
}


/*
 * it displays a congratulatory message on the display
 */
void displayCongrats() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(congratulationMessage);

  lcd.setCursor(0, 1);
  lcd.print(newHighscoreMessage);

  delay(3000);

  lcd.clear();
}

/*
 * shows the player their statistics (level and score he had when the game was over)
 * if compares the score with the highest scores
 * if the score is greater than one of the existing scores, the trophy animation and congratulatory message are displayed and the player is asked to enter their name
 * if the score is not a highscore, the game is reset
 */
void displayEndGameStatistics() {

  lcd.setCursor(0, 0);
  lcd.print(statisticsMessage);

  lcd.setCursor(0, 1);

  String statistics = displayLevel + level + "; " + displayScore + score;
  if (statistics.length() > 16) {
    lcd.print(scrollLCDLeft(statistics));
    delay(350);
  }
  else {
    lcd.print(statistics);
  }

  delay(5000);
  lcd.clear();

  int compareResult = compareScores(score);
  if ( compareResult > -1) {
    displayTrophyAnimation();
    displayCongrats();
    SYSTEM_STATE = NAME_UPDATE_SCREEN;
    enterName = true;
    currentRow = 0;
    joyMoved = false;
  }
  else {
    resetGame();
  }
}


/* 
 *  the function prints on the lcd the interface used to enter the name
 *  on the first, row the player has the option to modify the letters in the name
 *  on the second, row the player can set the name
 */
void enterPlayerName() {

  if (changedName) {
    lcd.clear();
    changedName = false;
  }

  if (currentRow == 1) {
    lcd.setCursor(0, 1);
    lcd.write(rightArrow);
    lcd.setCursor(1, 1);
    lcd.print(setCurrentName);
  }
  else {
    lcd.setCursor(0, 1);
    lcd.print(setCurrentName);
  }

  if (currentRow == 0) {
    lcd.setCursor(0, 0);
    lcd.write(rightArrow);

    lcd.setCursor(1, 0);
    lcd.print("Name: ");
    lcd.setCursor(7, 0);
    for (int i = 0; i < 3; i++) {
      lcd.print(alphabet[playerName[i]]);
    }
  }
  else {
    lcd.setCursor(0, 0);
    lcd.print("Name: ");
    lcd.setCursor(6, 0);
    for (int i = 0; i < 3; i++) {
      lcd.print(alphabet[playerName[i]]);
    }
  }
}


/*
 * name navigation using the joystick
 * if the letter is locked, the player can change de value of the letter, using an up and down movement of the joystick
 * if the letter is not locked, the player can move between the 3 letters, with a left and right movement
 * if the letter is not locked, the player can change rows with and up and down movement
 * if the button on the joystick is clicked when the player is on the second row, the name is set in the form displayed on the first row
 */
void navigateName() {
  if (lockedLetter) {
    // if the joystick was moved up-down
    if (yValue < minThreshold && joyMoved == false) {
      joyMoved = !joyMoved;
      currentLetter--;
      if (currentLetter < 0) {
        currentLetter = alphabetLength - 1;
      }

      playerName[currentLetterPosition] = currentLetter;

      changedName = true;
    }
    if (yValue > maxThreshold && joyMoved == false) {
      joyMoved = !joyMoved;
      currentLetter++;
      if (currentLetter == alphabetLength) {
        currentLetter = 0;
      }
      playerName[currentLetterPosition] = currentLetter;

      changedName = true;
    }

  }
  else {
    // up-down motion means changing the row
    if (yValue < minThreshold && joyMoved == false) {
      if (joyMoved == false) {
        joyMoved = !joyMoved;
      }
      if (currentRow == 0) {
        currentRow = 1;
      }
      changedName = true;
    }
    if (yValue > maxThreshold && joyMoved == false) {
      if (joyMoved == false) {
        joyMoved = !joyMoved;
      }
      if (currentRow == 1) {
        currentRow = 0;
      }
      changedName = true;
    }

    // left-right motion means change the letter updated
    if (xValue < minThreshold && joyMoved == false) {
      if (joyMoved == false) {
        joyMoved = !joyMoved;
      }
      currentLetterPosition--;

      if (currentLetterPosition < 0) {
        currentLetterPosition = 2;
      }
    }

    if (xValue > maxThreshold && joyMoved == false) {
      if (joyMoved == false) {
        joyMoved = !joyMoved;
      }

      currentLetterPosition++;

      if (currentLetterPosition == 3) {
        currentLetterPosition = 0;
      }
    }
  }

  enterPlayerName();

  if (minThreshold <= yValue && yValue <= maxThreshold) {
    joyMoved = false;
  }
}

/*
 * after the name was set, the new rank of the player is determined
 * the current scores are shifted and EEPROM is updated accordingly
 */
void setName() {
  for (int i = 0; i < 3; i++) {
    newName += alphabet[playerName[i]];
  }
  int newRank = compareScores(score);
  shiftScores(newName, score, newRank);
  updateEEPROM();
  resetGame();
}


/*
 * the function resets all the variables as they were before starting the game
 */
void resetGame() {

  SYSTEM_STATE = MENU_SCREEN;
  score = 0;
  previousScore = 0;
  changeLevelScore = 0;

  currentMenuToDisplay = "Main";
  lastDisplayedMenu = "Main";
  currentItem = "";

  getHighScores();

  resetMenuVariables();
  resetMatrix();
  resetObstacle();
  resetBird();
  displayInitialAnimation();
}

void resetBird() {
  currentBirdPosition[0][0] = 3;
  currentBirdPosition[0][1] = 0;

  currentBirdPosition[1][0] = 2;
  currentBirdPosition[1][1] = 1;
}

void resetObstacle() {
  obstacleColumn = 7;
  moveObstacleInterval = 1000;
  generated = 0;
  for (int i = 0; i < matrixSize; i++) {
    obstacle[i] = 0;
  }
}

void resetPlayerName() {
  for (int i = 0; i < 3; i++) {
    playerName[i] = 0;
  }

  changedName = false;
  enterName = true;

  currentLetterPosition = 0;
  currentLetter = 0;

  lockedLetter = false;
  newName = "";
}

void resetMatrix() {
  for (int i = 0; i < matrixSize; i++) {
    for (int j = 0; j < matrixSize; j++) {
      matrix[i][j] = 0;
    }
  }
}
