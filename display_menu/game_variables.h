// General game variables
bool gameStarted = true;
bool gameFinished = false;

int previousLevel = 1;
int level = 1;
int previousScore = 0;
int score = 0;

// at the begining the bird is formed by 1 dot
int birdSize = 1;

// the bird can be max 3 dots
/*int currentBirdPosition[3][2] = {{4, 0}, {-1, -1}, {-1, -1}}; 
int lastBirdPosition[3][2];*/

// the bird is always on the first column, only the row changes 
int xPos = 3;
int yPos = 0;

int xLastPos;
int yLastPos; 

// number of cols between the walls (will decrease at a higher level) 
int wallSpacing = 4;

// if during this interval the player hasn't moved the bird, it will decrease on position
const int decreaseInterval = 2000;

int obstacle[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int obstacleColumn = 7;
int lastMovedObstacle = 0;
int moveObstacleInterval = 1000;
int generated = 0;
