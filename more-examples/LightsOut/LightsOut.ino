#include <Gamer.h>

Gamer gamer;

int currentX=3;
int currentY=3;
int moveCount;
int optimalCount;
byte pattern[8];
int level=1;
const int maxLevel=25;

void setup() {
  gamer.begin();
  setupPattern();
}

void setupPattern() {
  selectLevel();
  for( int i=0; i<8; i++){
    pattern[i] = 0;
  }
  for( int i=0; i<level+4; i++) {
    int x,y;
    do {
      x = random(0,8);
      y = random(0,8);
    }while( bitRead( pattern[y], x) );
    bitSet( pattern[y],x );
  }
  applyPattern();
}

void applyPattern() {
  for(int x=0;x<8;x++) {
    for(int y=0;y<8;y++) {
      gamer.display[x][y] = false;
    }
  }
  optimalCount=0;
  for(int x=0;x<8;x++) {
    for(int y=0;y<8;y++) {
      if( bitRead( pattern[y], x) ){
        optimalCount++;
        domove(x,y);
      }
    }
  }
  moveCount=0;
  gamer.updateDisplay();
}

void domove(int x, int y){
  gamer.display[x][y]^=true;
  if( x>0 ) gamer.display[x-1][y]^=true;
  if( y>0 ) gamer.display[x][y-1]^=true;
  if( x<7 ) gamer.display[x+1][y]^=true;
  if( y<7 ) gamer.display[x][y+1]^=true;
}

boolean isClear(){
  for(int x=0;x<8;x++) {
    for(int y=0;y<8;y++) {
      if(gamer.display[x][y]) return false;
    }
  }
  return true;
}

void loop() {
  for( int i=0; i<4; i++ ){
    if(i==0) gamer.display[currentX][currentY]^=true;
    gamer.updateDisplay();    
    delay(100);
    if(i==0) gamer.display[currentX][currentY]^=true;
    
    if(gamer.isPressed(START)) {
      moveCount++;
      domove(currentX,currentY);
    }
    if(gamer.isPressed(UP))    currentY=(currentY-1)&7;
    if(gamer.isPressed(DOWN))  currentY=(currentY+1)&7;
    if(gamer.isPressed(LEFT))  currentX=(currentX-1)&7;
    if(gamer.isPressed(RIGHT)) currentX=(currentX+1)&7;
    
    if(isClear()) gameSolved();
    
    int b=0;
    if(gamer.isHeld(UP))    b++;
    if(gamer.isHeld(DOWN))  b++;
    if(gamer.isHeld(LEFT))  b++;
    if(gamer.isHeld(RIGHT)) b++;
    if(gamer.isHeld(START)) b++;
    if( b>=3 ){
      setupPattern();
    }
  }
}

byte spiral[64]={7,15,23,31,39,47,55, 63,62,61,60,59,58,57, 56,48,40,32,24,16,8, 0,1, 2, 3, 4, 5, 6,
                  14,22,30,38,46,       54,53,52,51,50,       49,41,33,25,17,     9,10,11,12,13,
                     21,29,37,             45,44,43,             42,34,26,          18,19,20,
                        28,                   36,                   35,                27};

void gameSolved() {
  gamer.clear();
  int score = moveCount-optimalCount;
  if( score==0 ){
    // spiral animation
    for( int i=0; i<64; i++ ){
      gamer.display[spiral[i]&7][spiral[i]>>3]=true;
      gamer.updateDisplay();
      delay(10);
    }
    for( int i=0; i<64; i++ ){
      gamer.display[spiral[i]&7][spiral[i]>>3]=false;
      gamer.updateDisplay();
      delay(10);
    }
    level=constrain(level+1,1,maxLevel);
    setupPattern();
  }else if(score<=16){
    for(int i=0; i<score; i++ ){
      gamer.display[i&7][i>>3]=true;
    }
    gamer.updateDisplay();
    delay(1200);
    setupPattern();
  }else{
    for(int i=0; i<8; i++ ){
      gamer.display[i][i]=true;
      gamer.display[i][7-i]=true;
    }
    gamer.updateDisplay();
    delay(800);
    applyPattern();
  }
}


void selectLevel(){
  byte scr[8] = {B10101010,B10101010,B11010011};
  byte numbers[10][4] = {
    {B010,B101,B101,B010},
    {B010,B010,B010,B010},
    {B110,B001,B010,B111},
    {B111,B010,B001,B110},
    {B100,B101,B111,B001},
    {B111,B110,B001,B110},
    {B011,B110,B101,B010},
    {B111,B001,B010,B010},
    {B010,B111,B101,B010},
    {B010,B101,B011,B110}};
  do{
    for(int i=0; i<4; i++){
      scr[i+4] = numbers[level%10][i];
      if( level>9 )
        scr[i+4] |= numbers[level/10][i]<<4;
    }
    gamer.printImage(scr);
    delay(100);
    if(gamer.isPressed(UP))    level=constrain(level+1,1,maxLevel);
    if(gamer.isPressed(DOWN))  level=constrain(level-1,1,maxLevel);
  }while( !gamer.isPressed(START) );
}
