// The message displayed before the menu
const String greetingMessage = "Sssnake";

// Main menu items
String mainMenuItems[] = {
  "Start Game",
  "High Scores",
  "Settings",
  "About"
};

String availableMenues[] = {
  "Start Game",
  "Main",
  "High Scores",
  "Settings",
  "About",
  "Back"
};

String currentMenuToDisplay = "Main";

int currentMenuItem = 0;
int currentRow = 0;
int displayedItems[2] = {0, 1};

bool changedMenu = false;

// Submenues items
String settings[] = {
  "LCD Constrast",
  "LCD Brightness",
  "Matrix Brightness",
  "Back"
};

int contrasts[3] = {100, 120, 140};

// About menu
String about[] = {
  "Sssnake",
  "Carutasu Stefania",
  "https://github.com/StefaniaCarutasu/Introduction-to-Robotics---Matrix-Project",
  "Back"
};
