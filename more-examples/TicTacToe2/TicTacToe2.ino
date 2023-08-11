#include <GamerIR.h>

#include <Gamer.h>

Gamer gamer;

int adelay[] = {4,2};//animation delay (ticks per frame)
int cframe[] = {0,0};//current frame per animation
int nframes[] = {2,4};//total number of frames per animation
boolean anims[2][4][4] = {{{1,0,0,1},{0,1,1,0}},//animations for x and o
                      {{1,0,0,0},{0,1,0,0},{0,0,0,1},{0,0,1,0}}};
int size = 3;//size of the board (3x3)
int gridLen = size*size;


int turn; //player's turn 0 = x, 1 = o
int player;  // current player, 0=human, 1=computer
int grid[] = {0,0,0,//store
              0,0,0,
              0,0,0};//0 is clear, 1 is x, 2 is o
int lines[8][3] = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};
int cx,cy,cdelay = 3;
boolean cstate;
int tick = 0;

void setup(){
  gamer.begin();
  restart();
}

void loop(){
  handleInput();
  tick = (tick+1)%32000;
  if(tick % cdelay == 0) cstate = !cstate;
  if(tick % adelay[0] == 0) cframe[0] = (cframe[0]+1)%nframes[0];//update animation frames independently
  if(tick % adelay[1] == 0) cframe[1] = (cframe[1]+1)%nframes[1];
  drawBoard();
  drawPieces();
  if(cstate){
    for(int sy = 0 ; sy < 2; sy++)
      for(int sx = 0; sx < 2; sx++)
        gamer.display[cx*3+sx][cy*3+sy] = true;
  }
  gamer.updateDisplay();
  delay(40);
}
void drawBoard(){
  // draw empty grid
  for(int y = 0 ; y < 8; y++)
    for(int x = 0 ; x < 8; x++)
      if(x == 2 || x == 5 || y == 2 || y == 5) gamer.display[x][y] = true;
      else gamer.display[x][y] = false;
}
void drawPieces(){
  // draw pieces
  for(int y = 0 ; y < size; y++){
    for(int x = 0 ; x < size; x++){
      int s = grid[y*size+x]-1;//symbol at current index
      if(s >= 0){//if there is symbol to draw
        for(int sy = 0 ; sy < 2; sy++)
          for(int sx = 0; sx < 2; sx++)
            gamer.display[x*3+sx][y*3+sy] = anims[s][cframe[s]][sy*2+sx];
      }
    }
  }
}
int checkLoser(int s){
  for( int i=0; i<8; i++ ){
    if( grid[lines[i][0]] == s && grid[lines[i][1]] == s && grid[lines[i][2]] == s )
      return 3-s;
  }
  for( int i=0; i<gridLen; i++ ){
    if( grid[i]==0 ) return 0;
  }
  return -1;
}

void handleInput(){
  if( player!=0 ){
    int i = calcMove();
    doMove(i);
  }
  else{
    if(gamer.isPressed(UP)   && cy > 0)      cy--;
    if(gamer.isPressed(DOWN) && cy < size-1) cy++;
    if(gamer.isPressed(LEFT) && cx > 0)      cx--;
    if(gamer.isPressed(RIGHT)&& cx < size-1) cx++;
    int cindex = cy*size+cx;
    if(gamer.isPressed(START)){
      if(gamer.isHeld(START) && gamer.isHeld(DOWN) )
        player=1;
      else if(gamer.isHeld(START) && gamer.isHeld(UP))
        restart();
      else if( grid[cindex] == 0)
        doMove(cindex);
    }
  }
}

void doMove(int ix){
  grid[ix] = turn+1;
  turn = 1-turn;
  player = 1-player;
  checkGameOver(grid[ix]);
}

int scores[2][4]={ {1,5,100,0},{1,20,120,0}};

int calcMove(){
  int sc[gridLen];
  boolean empty=true;
  for( int i=0; i<gridLen; i++){
    sc[i]=0;
    empty &= grid[i]==0;
  }
  if( empty ){
    return random(2)*2 + random(2)*6;
  }
  for( int i=0; i<8; i++ ){
    int cnt[3] = {0};
    for( int ix = 0; ix<3; ix++){
      cnt[grid[lines[i][ix]]]++;
    }
    if( cnt[1]==0 || cnt[2]==0 ){
      int s = scores[0][cnt[2-turn]] + scores[1][cnt[1+turn]];
      for( int ix = 0; ix<3; ix++){
        sc[lines[i][ix]]+=s;
      }
    }
  }
  int mxix = -1;
  for( int i=0; i<gridLen; i++){
    if( grid[i]==0 && ( mxix<0 || sc[i]>sc[mxix] || (sc[i]==sc[mxix] && random(3)==0) ) )
      mxix=i;
  }
  return mxix;  
}

boolean checkGameOver(int s){
  int loser = checkLoser(s);
  if( loser==0) return false;
  for(int i = 0 ; i < gridLen ; i++)
    if(grid[i] == loser)
      grid[i] = 0;
  for( int i=0; i<10; i++){
    gamer.toggleLED();
    drawBoard();
    gamer.updateDisplay();
    delay(100);
    drawBoard();
    drawPieces();
    gamer.updateDisplay();
    delay(100);
  }
  delay(1000);
  restart();
  return true;
}

void restart(){
  for(int i = 0 ; i < gridLen; i++) grid[i] = 0;
  turn = 0;
  player = 0;
}
