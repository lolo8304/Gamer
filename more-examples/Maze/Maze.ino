#include <Gamer.h>

Gamer gamer;

const int STOPPED = -1;

const int sz = 71;  // must be odd
const int storageSize = (sz*sz+7)/8;
byte bitStorage[storageSize];

boolean getMaze(int y, int x){
  int n=y*sz+x;
  return bitRead(bitStorage[n>>3],n&7);
}
void setMaze(int y, int x, boolean v){
  int n=y*sz+x;
  bitWrite(bitStorage[n>>3],n&7,v);
}


int currentX;
int currentY;
int dir;

void setup() {
  gamer.begin();
  randomSeed(gamer.ldrValue());
  setupMaze();
}

void setupMaze() {
  gamer.allOn();
  // clear maze
  for(int i=0; i<storageSize; i++){
    bitStorage[i]=0;
  }
  // fill borders
  for(int i=0; i<sz; i++){
    setMaze(0,i,true);
    setMaze(1,i,true);
    setMaze(sz-1,i,true);
    setMaze(sz-2,i,true);
    setMaze(i,0,true);
    setMaze(i,1,true);
    setMaze(i,sz-1,true);
    setMaze(i,sz-2,true);
  }
  
  int numwalls = (sz-5)/2;
  numwalls*=numwalls;
  // generate maze  
  const int listMax = sz+sz;
  byte list[listMax];
  int progressStep = numwalls/64;
  int progressCounter = 0;
  int progress = 0;
  for( int p=0; p<numwalls; p++){
    // build list of potential mazewalls
    int listLen=0;
    for(int y=1; y+2<sz; y+=2){
      for(int x=1; x+2<sz; x+=2){
        if( getMaze(y,x)!=getMaze(y,x+2) ){
          list[listLen++]=y;
          list[listLen++]=x+1;
          if(listLen>=listMax) goto listfull;
        }
        if( getMaze(y,x)!=getMaze(y+2,x) ){
          list[listLen++]=y+1;
          list[listLen++]=x;
          if(listLen>=listMax) goto listfull;
        }
      }
    }
    listfull:
    // Choose a random potential mazewall
    if( listLen==0 ) break;
    int i=random(0,listLen/2)*2;
    int y=list[i];
    int x=list[i+1];
    // turn it into a wall
    setMaze(y,x,true);
    if     (getMaze(y-1,x)) setMaze(y+1,x,true);
    else if(getMaze(y+1,x)) setMaze(y-1,x,true);
    else if(getMaze(y,x-1)) setMaze(y,x+1,true);
    else if(getMaze(y,x+1)) setMaze(y,x-1,true);
    progressCounter++;
    if( progressCounter>=progressStep ){
      progressCounter=0;
      gamer.display[progress&7][progress/8]=false;
      gamer.updateDisplay();
      progress++;
    }
  }
  currentX=4;
  currentY=4;
  dir = STOPPED;
  gamer.clear();
}

void loop() {
  
  for( int i=0; i<4; i++ ){
    displayMaze();
    gamer.display[3][3]=i!=0;
    gamer.display[4][3]=i!=1;
    gamer.display[4][4]=i!=2;
    gamer.display[3][4]=i!=3;
    gamer.updateDisplay();
    delay(200);
    if( currentX==sz*2-6 && currentY==sz*2-6 ){
      foundExit();
      setupMaze();
      break;
    }
    
    if( ((currentX|currentY)&1)==0 ){
      if(gamer.isPressed(UP)){
        if( dir==DOWN ) dir=STOPPED;
        else dir=UP;
      }
      if(gamer.isPressed(DOWN)){
        if( dir==UP ) dir=STOPPED;
        else dir=DOWN;
      }
      if(gamer.isPressed(LEFT)){
        if( dir==RIGHT ) dir=STOPPED;
        else dir=LEFT;
      }
      if(gamer.isPressed(RIGHT)){
        if( dir==LEFT ) dir=STOPPED;
        else dir=RIGHT;
      }
    }
    
    if     (dir==UP    && !getMaze((currentY-1)/2,currentX/2) ) currentY--;
    else if(dir==DOWN  && !getMaze((currentY+2)/2,currentX/2) ) currentY++;
    else if(dir==LEFT  && !getMaze(currentY/2,(currentX-1)/2) ) currentX--;
    else if(dir==RIGHT && !getMaze(currentY/2,(currentX+2)/2) ) currentX++;
    else dir=STOPPED;
    
    int b=0;
    if(gamer.isHeld(UP))    b++;
    if(gamer.isHeld(DOWN))  b++;
    if(gamer.isHeld(LEFT))  b++;
    if(gamer.isHeld(RIGHT)) b++;
    if(gamer.isHeld(START)) b++;
    if( b>=3 ){
      setupMaze();
    }
  }
}

void displayMaze(){
  for( int y=0; y<8; y++){
    int ry = (currentY-3+y)>>1;
    for( int x=0; x<8; x++){
      int rx = (currentX-3+x)>>1;
      gamer.display[x][y] = getMaze(ry,rx);
    }
  }
}

byte spiral[64]={7,15,23,31,39,47,55, 63,62,61,60,59,58,57, 56,48,40,32,24,16,8, 0,1, 2, 3, 4, 5, 6,
                  14,22,30,38,46,       54,53,52,51,50,       49,41,33,25,17,     9,10,11,12,13,
                     21,29,37,             45,44,43,             42,34,26,          18,19,20,
                        28,                   36,                   35,                27};

void foundExit() {
  gamer.clear();
  // spiral animation
  for( int i=0; i<64; i++ ){
    gamer.display[spiral[i]&7][spiral[i]>>3]=true;
    gamer.updateDisplay();
    delay(10);
  }
  delay(50);
  for( int i=0; i<64; i++ ){
    gamer.display[spiral[i]&7][spiral[i]>>3]=false;
    gamer.updateDisplay();
    delay(10);
  }
}


