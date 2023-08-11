#include <Gamer.h>

Gamer gamer;

int size = 4;//size of the board (4x4)
int gridLen = size*size;

word board;
word selection;
int sx, sy;
int player;
int cx,cy;
int tick = 0;

word moves[] = {
         0x0001,0x0002,0x0004,0x0008,
         0x0010,0x0020,0x0040,0x0080,
         0x0100,0x0200,0x0400,0x0800,
         0x1000,0x2000,0x4000,0x8000,

         0x0003,0x0006,0x000C,
         0x0030,0x0060,0x00C0,
         0x0300,0x0600,0x0C00,
         0x3000,0x6000,0xC000,
         
         0x0011,0x0022,0x0044,0x0088,
         0x0110,0x0220,0x0440,0x0880,
         0x1100,0x2200,0x4400,0x8800,

         0x0007,0x000E,
         0x0070,0x00E0,
         0x0700,0x0E00,
         0x7000,0xE000,

         0x0111,0x0222,0x0444,0x0888,
         0x1110,0x2220,0x4440,0x8880,
         
         0x000F,
         0x00F0,
         0x0F00,
         0xF000,
         
         0x1111,0x2222,0x4444,0x8888,
};

byte spiral[64]={7,15,23,31,39,47,55, 63,62,61,60,59,58,57, 56,48,40,32,24,16,8, 0,1, 2, 3, 4, 5, 6,
                  14,22,30,38,46,       54,53,52,51,50,       49,41,33,25,17,     9,10,11,12,13,
                     21,29,37,             45,44,43,             42,34,26,          18,19,20,
                        28,                   36,                   35,                27};

void setup(){
  gamer.begin();
  showWipe();
  restart();
}

void loop(){
  handleInput();
  tick = (tick+1)%6;
  int v=board;
  if( player==0 && (tick==0 || tick==3) ) v^=bit(cx+4*cy);
  if( tick<3 ) v ^= selection;
  drawBoard(v);
  delay(40);
}
void drawBoard(word v){
  for(int y = 0 ; y < 4; y++){
    for(int x = 0 ; x < 4; x++){
      drawPiece(x, y, getPiece(v,x,y));
    }
  }
  gamer.updateDisplay();
}
boolean getPiece(int x, int y){
  return getPiece(board,x,y);
}
boolean getPiece(word b, int x, int y){
  return bitRead(b,x+y*4);
}
word togglePiece(word b, int x, int y){
  return b^bit(x+y*4);
}
void drawPiece(int x, int y, boolean v){
  gamer.display[x+x  ][y+y  ] = v;
  gamer.display[x+x  ][y+y+1] = v;
  gamer.display[x+x+1][y+y  ] = v;
  gamer.display[x+x+1][y+y+1] = v;
}

void handleInput(){
  if( player!=0 ){
    gamer.setLED(true);  // switch on
    unsigned long time = millis();
    word i = calcMove();
    time = millis() - time;
    if( time<500 ) delay(500-time);
    gamer.setLED(false); // switch off
    doMove(i);
  }
  else{
    if(gamer.isPressed(UP)   ) moveCursor(cx,cy-1);
    if(gamer.isPressed(DOWN) ) moveCursor(cx,cy+1);
    if(gamer.isPressed(LEFT) ) moveCursor(cx-1,cy);
    if(gamer.isPressed(RIGHT)) moveCursor(cx+1,cy);
    int cindex = cy*size+cx;
    if(gamer.isPressed(START)){
      if(gamer.isHeld(START) && gamer.isHeld(DOWN) )
        player=1;
      else if(gamer.isHeld(START) && gamer.isHeld(UP))
        restart();
      else if( getPiece(cx,cy) ){
        if( selection==0 ){
          selection=togglePiece(selection,cx,cy);
          sx=cx;
          sy=cy;
        }else{
          doMove(selection);
        }
      }
    }
  }
}
void moveCursor(int cx2,int cy2){
  if( cx2<0 || cy2<0 || cx2>=size || cy2>=size) return;
  if( selection!=0 ){
    if( cx2!=sx && cy2!=sy ) return;
    if( !getPiece(cx2,cy2) )return;
    // update selection
    if( getPiece(selection,cx2,cy2) ){
      selection = togglePiece(selection,cx,cy);
    }else{
      selection = togglePiece(selection,cx2,cy2);
    }
  }
  cx=cx2;
  cy=cy2;
}

void doMove(word m){
  board ^= m;
  selection = 0;
  player = 1-player;
  if( board==0){
    gamer.clear();
    delay(300);
    if( player==1 ){
      showSpiral();
    }else{
      showWipe();
    }
    delay(300);
    restart();
  }
}

void showSpiral(){
  // spiral animation
  gamer.clear();
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
}

// Wipe
void showWipe(){
  for( int i=1; i<5; i++){
    for( int y=0; y<8; y++ ){
      for( int x=0; x<8; x++ ){
        gamer.display[x][y]=i&1;
      }
      gamer.updateDisplay();
      delay(30);
    }
  }
}

void restart(){
  board = 0xFFFF;
  selection = 0;
  cx = 0;
  cy = 0;
  player = 0;
}

int calcMove(){
  int bestMove = 0;
  int bestScore = -2;
  for( int i=0; i<64; i++ ){
    word m = moves[i];
    if( (board&m)==m ){
      int s = calcScore(board^m);
      if( s>bestScore || (s==bestScore && random(15)==0 )){
        bestScore = s;
        bestMove = m;
      }
    }
  }
  return bestMove;
}

int calcScore(word brd){
  // returns -1, 0, 1 for bad/unknown/winning position
  word visited = 0;
  int score = 0;
  while( visited!=brd ){
    // find any piece remaining
    for( int y=0; y<4; y++){
      for( int x=0; x<4; x++){
        word remaining = brd^visited;
        if( getPiece(remaining,x,y) ){
          // find connected component of this piece
          word comp = getComponent(remaining, x, y, 0);
          visited |= comp;
          comp = normalise(comp);
          int s = getScore(comp);
          if( s<0 ) return 0;
          score ^=s;
        }
      }
    }
  }
  if( score==0 ) return 1;
  return -1;
}

const int numPatterns = 240;
int scoreTable[numPatterns]={
   1, 0x1,
   2, 0x3,
   3, 0x7, 3, 0x13,
   4, 0xf, 4, 0x17, 2, 0x27, 4, 0x2e, 0, 0x33, 1, 0x36,
   5, 0x1f, 5, 0x2f, 5, 0x37, 5, 0x3e, 1, 0x57, 1, 0x117, 4, 0x136, 5, 0x171, 5, 0x172, 1, 0x174, 3, 0x272,
   6, 0x3f, 4, 0x5f, 6, 0x6f, 2, 0x77, 6, 0x7d, 0, 0x7e, 6, 0x9f, 6, 0x11f, 0, 0x137, 6, 0x13e, 6, 0x157, 6, 0x173, 0, 0x175, 6, 0x176, 6, 0x17c, 4, 0x1f1, 6, 0x1f2,
            4, 0x1f4, 6, 0x1f8, 6, 0x22f, 0, 0x23e, 0, 0x273, 3, 0x275, 0, 0x27c, 6, 0x2ae, 6, 0x2e3, 0, 0x2ea, 6, 0x2ec, 4, 0x2f2, 4, 0x2f4, 6, 0x32e, 3, 0x36c, 
   7, 0x7f, 7, 0xbf, 7, 0x13f, 7, 0x15f, 3, 0x177, 7, 0x17d, 7, 0x17e, 7, 0x19f, 7, 0x1be, 2, 0x1d7, 0, 0x1f3, 5, 0x1f5, 7, 0x1f6, 7, 0x1f9, 7, 0x1fa, 7, 0x1fc,
            7, 0x23f, 1, 0x26f, 4, 0x277, 7, 0x27d, 1, 0x27e, 7, 0x2af, 7, 0x2be, 1, 0x2e7, 7, 0x2eb, 3, 0x2ee, 7, 0x2f3, 7, 0x2f5, 7, 0x2f6, 7, 0x2f9, 5, 0x2fa,
            7, 0x2fc, 7, 0x31f, 7, 0x32f, 3, 0x33e, 3, 0x357, 7, 0x367, 7, 0x36e, 2, 0x376, 7, 0x37c, 3, 0x3ae, 7, 0x3e3, 7, 0x3e6, 7, 0x3ea, 2, 0x557, 1, 0x575,
            7, 0x62f, 7, 0x63e, 7, 0x72e, 7, 0x111f, 2, 0x113e, 2, 0x117c, 7, 0x11f1, 7, 0x11f2, 7, 0x11f4, 7, 0x11f8, 2, 0x136c, 7, 0x13e2, 7, 0x13e4, 7, 0x13e8,
            3, 0x174c, 7, 0x17c4, 7, 0x1f22, 7, 0x1f44, 7, 0x22f2, 7, 0x22f4, 1, 0x23e4, 
};

int getScore(word pat){
  for( int i=0; i<numPatterns; i+=2 ){
    if( pat==scoreTable[i+1] ) return scoreTable[i];
  }
  return -1;
}

word getComponent(word pos, int x,int y, word visited0){
  word bit = 1<<(x+4*y);
  if( (visited0&bit)!=0 ) return visited0;
  if( (pos&bit)==0 ) return visited0;
  word visited = visited0 | bit;
  if( x>0 ) visited |= getComponent(pos,x-1,y, visited);
  if( x<3 ) visited |= getComponent(pos,x+1,y, visited);
  if( y>0 ) visited |= getComponent(pos,x,y-1, visited);
  if( y<3 ) visited |= getComponent(pos,x,y+1, visited);
  return visited;
}

word normalise(word pos){
  word best=pos;
  word pos2 = pos;
  for( int i=0; i<2; i++){
    for( int j=0; j<4; j++){
      if( pos2<best ) best=pos2;
      pos2 = rotate(pos2);
    }
    pos2 = mirror(pos2);
  }
  return best;
}

word translate(word pos){
  word pos2 = pos;
  if( pos2==0 ) return 0;
  while( (pos2&0xF)==0 ) pos2>>=4;
  while( (pos2&0x1111)==0 ) pos2>>=1;
  return pos2;
}

word rotate(word pos){
  word pos2 = 0;
  for( int y=0; y<4; y++){
    for( int x=0; x<4; x++){
      if( (pos&(1<<(x+4*y)))!=0 ){
        pos2 |= 1<<(3-y+4*x);
      }
    }
  }
  return translate(pos2);
}

word mirror(word pos){
  word pos2 = 0;
  for( int y=0; y<4; y++){
    for( int x=0; x<4; x++){
      if( (pos&(1<<(x+4*y)))!=0 ){
        pos2 |= 1<<(3-x+4*y);
      }
    }
  }
  return translate(pos2);
}

