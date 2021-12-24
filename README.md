# Matrix Project: Flappy Dots
<br/>

## Requirements

Flappy Bird game adaptation using Arduino Uno. 
The menu of the game is displayed on a Liquid Crystal Display. The main menu contains the Start Game, Settings, Highest Scores and About sections. 
The game is played on a 8x8 matrix.
The navigation in the menu and playing the game are possible using a joystick. 

<br/>

## Implementation Details

A greeting containing the name of the game is displayed when the game is launched. After 5 seconds the menu appears and the current menu item is indicated with a right arrow pointing towards it. With a joystick, the navigation through the menu is possible. When the button is pressed, an action occurs based on the current item selected: 

- **Start game**: a new game starts and the message "Game Started" is displayed.
- **Settings**: there are available settings for LCD contrast and brightness, matrix brightness, resetting the 3 highest scores and turning on and off the sounds in the game
- **Highest Scores**: the first 3 highest scores are displayed along with the name of the player who achieved them
- **About**: the name of the game, author and github repository are displayed on separate lines. 

<br/>

### Game logic
- The "bird" is formed of two dots floating on the first column, respectivly second column, of the matrix. 
- An obstacle similar to those used in Flappy Bird is generated on the last column of the matrix. The obstacle travels towards the "bird" that must avoid it. If the obstacle is succesfully avoided, the score increases and a new obstacle is generated.
- With each level passed, the obstacles increase in speed. Starting with level 5, the obstacles shift up and down with each movement, increasing the difficulty of the game.
- If the player is inactive for a certain amount of time, the "bird" will fall one position. If the bird touches the bottom row, the player loses the game and a message is shown along with the score. 
- The current level is passed if the score passes a certain benchmark. The difficulty is increased by increasing the speed of the obstacles. 
- The game is lost when the bird either touches an obstacle or the bottom row automatically (not when the player moves the bird to the bottom row in order to avoid obstacles.
- During the game, the current score and level are displayed on the LCD.

<br/>

### Available sounds
- Theme sound that plays while scrolling through the menu.
- In game sounds when the player moves the bird up and down.
- If the bird touches the bottom row or an obstacle, a sound indicating the lost is played.

<br/>

### Available animations 
- While scrolling through the menu, a bird animation is shown on the matrix.
- If the game is lost, and X animation is shown on the matrix.
- If the player passes one of the highest scores, a tropy animation is shown on the matrix.

<br/>

### Messages displayed on the 
- At the beginning, the name of the game is displayed for 5 seconds.
- After the game is finished, a message indicating if the player won or lost is shown.
- The player's statistics are shown: the level and score he achieved when the game was finished
- If the player passed one of the highscores, a congratulatory message is shown.

<br/>

### End of the game
- After the game is finished, the player is shown the level and score they have achieved.
- If a highscore was achieved, a congratulatory message is displayed.
- If a highscore was achieved, the player is asked to set their name.

### Setting the name
- If a highscore is achieved, the player will be asked to enter their name
- The name can be a combination of 3 letters
- To enter the name, the player has to press the button on the joystick
- To change the current letter, the joystick is moved up and down
- To hover between letters, the joystick is moved left and right
- If the button was pressed, the letter is locked and only the up and down movements work.
- If the player is hovering between letters, an up and down movement changes the line on the display. 
- The first line on the display contains the name; when the button is pressed on the second line on the display, the name is set and the player is brought back in the main menu

<br/>

### Stored information
- All settings are stored in EEPROM: LCD brightness, LCD contrast, matrix brightness and if the sound is on or off.
- The first 3 highscores and the names of the players' who achieved them.

<br/>

### Project Architecture
- **flappy_dots.ino**: main file of the project where the setup, loop and most of the functionalities are written.
- **buzzer_variables.h**: constains all the variables for the buzzer such as the pin, the theme song, the EEPROM address where the sound option is stored, and functions for playing the different available sounds.
- **game_variables.h**: contains all the variables needed for the game logic such as the position of the bird, the shape, posion and speed of the obstacles, the level and score all the variables needed to enter and store the player's name.
- **highscore.h**: contains all the variables and functions needed to store and retrieve the highscores. The score achieved by the player is compared with the existing ones. If is surpassed any of the existing scores, they are shifted to make room for the new one. The file also contains the functions for writting and retrieving integres from EEPROM. 
- **joystick_variables.h**: contains all the variables needed to operate de joystick such as the pins used, the debounce interval for the button and the min and max thresholds.
- **lcd_variables.h**: constains all the variables used for the LCD such as the pins, the EEPROM addresses used to store the values for brightness and contrast, and the icons for the arrows displayed. 
- **matrix_variables.h**: contains all the variables used for the matrix such as the pins, the matrixed used in the game and the matrices used to display the animations
- **menu_variables.h**: contains all the variables needed to display all the available menus such as lists of elements to be displayed, the current menu, the last displayed menu, the current selected item and the current row.
- **messages.h**: contains all the messages displayed on the lcd, before, during and after the game
- **pitches.h**: constains musical pitches used to make the theme song and the rest of the sounds.
- **system_states.h**: contains all the possible states of the system. in the loop function, the state of the sistem is checked to check the phase of the game and call the needed functions accordingly.

<br/>

## Setup picture phase 1
![](https://github.com/StefaniaCarutasu/Flappy-Dots/blob/main/setup_pictures/setup_picture_phase1.jpeg)

## Setup picture phase 2
![](https://github.com/StefaniaCarutasu/Flappy-Dots/blob/main/setup_pictures/setup_picture_phase2.jpeg)

## Setup picture phase 3

## Demo video
[]()

