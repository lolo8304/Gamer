#include <Gamer.h>

Gamer gamer;

int currentX;
int currentY;
int dir;
int goalX;
int goalY;
int snakeMap[8][8];
int snakeLength;
int score;

void setup() {
  gamer.begin();
  setupStuff();
}

void setupStuff() {
  snakeLength = 2;
  score = 0;
  goalX = random(0,8);
  goalY = random(0,8);
  currentX = 0;
  currentY = 0;
  dir = UP;
  for(int x=0;x<8;x++) {
    for(int y=0;y<8;y++) {
      snakeMap[x][y] = 0;
    }
  }
  gamer.clear();
}

void loop() {
  //gamer.clear, but DON'T UPDATE YET!!!!
  for(int x=0;x<8;x++) {
    for(int y=0;y<8;y++) {
      gamer.display[x][y] = LOW;
    }
  }
  
  if(gamer.isPressed(UP)) dir=UP;
  if(gamer.isPressed(RIGHT)) dir=RIGHT;
  if(gamer.isPressed(DOWN)) dir=DOWN;
  if(gamer.isPressed(LEFT)) dir=LEFT;
  
  if(dir==UP) {
    currentY--;
    if(currentY<0) currentY=7;
  } else if(dir==RIGHT) {
    currentX++;
    if(currentX>7) currentX=0;
  } else if(dir==DOWN) {
    currentY++;
    if(currentY>7) currentY=0;
  } else if(dir==LEFT) {
    currentX--;
    if(currentX<0) currentX=7;
  }
  gamer.display[currentX][currentY] = HIGH;
  snakeRec();
  isCollected();
  delay(150);
  gamer.updateDisplay();
}

void isCollected() {
  if(currentX==goalX && currentY==goalY) {
    goalX = random(0,8);
    goalY = random(0,8);
    snakeLength++;
    score++;
    for(int x=0;x<8;x++) {
      for(int y=0;y<8;y++) {
        snakeMap[x][y]++;
      }
    }
  } else {
    gamer.display[goalX][goalY] = HIGH;
  }
}

void snakeRec() {
  for(int x=0;x<8;x++) {
    for(int y=0;y<8;y++) {
      if(snakeMap[x][y] > 0) {
        snakeMap[x][y]--;
      }
    }
  }
  collided();
  snakeMap[currentX][currentY] = snakeLength;
  for(int x=0;x<8;x++) {
    for(int y=0;y<8;y++) {
      gamer.display[x][y] |= (snakeMap[x][y] > 0);
    }
  }
}

void collided() {
  if(snakeMap[currentX][currentY] > 0) {
    gamer.clear();
    delay(20);
    gamer.printString("GAME OVER  your score is ");
    gamer.showScore(score);
    delay(1500);
    setupStuff();
  }
}

