// __n6v26r was here
#include "utils.h"
#include <stdio.h>
#include <unistd.h>
#pragma once

#define MAX_LVLS 20
#define SIZEX 11
#define SIZEY 25
#define TETRIMINOS 7

#define SPWNY 25
#define SPWNX 5

enum Tetriminos{
  _NULL = 0,
  I = 1,
  J = 2,
  L = 3,
  O = 4,
  S = 5,
  T = 6,
  Z = 7
};

enum Borders{
  left = -1,
  right = -2,
  bottom = -3
};

int piece[TETRIMINOS+1][4][4] = {
  {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  },
  {
    {0, 0, 0, 0},
    {1, 1, 1, 1},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  },
  {
    {2, 0, 0, 0},
    {2, 2, 2, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  },
  {
    {0, 0, 3, 0},
    {3, 3, 3, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  },
  {
    {0, 0, 0, 0},
    {0, 4, 4, 0},
    {0, 4, 4, 0},
    {0, 0, 0, 0}
  },
  {
    {0, 5, 5, 0},
    {5, 5, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  },
  {
    {0, 6, 0, 0},
    {6, 6, 6, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  },
  {
    {7, 7, 0, 0},
    {0, 7, 7, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  },
};

typedef struct _Pos{
  int x;
  int y;
} pos;

typedef struct _GameManager{
  double time;
  double lastDescend;
  unsigned int score;
  unsigned int linesCleared;
  unsigned int sizeX; //
  unsigned int sizeY; // dimensions
  unsigned int level;
  bool pieceHeld;
  enum Tetriminos hold;
  enum Tetriminos curr; // current piece
  int currPieceState[4][4];
  pos currPos;
  pos projectionPos;
  enum Tetriminos* next; // pointer to the piece bag
  // This implementation uses the original random bag extract method as the original tetris game.
  enum Tetriminos pieceBag[TETRIMINOS];
  short int bagSize;
  int** board;
} gameManager;

const double pieceFallDelay[MAX_LVLS] = {1, 0.95, 0.90, 0.85, 0.80, 0.75, 0.70, 0.65, 0.60, 0.55, 0.50, 0.45, 0.40, 0.35, 0.30, 0.25, 0.20, 0.15, 0.10, 0.05}; // higher lvl => faster falling pieces

// Using the original NES formula, score = multiplier[linesCleared]*(level+1)
const int scoreMultiplier[] = {0, 40, 100, 300, 1200};

const enum Tetriminos pieces[] = {_NULL, I, J, L, O, S, T, Z};

void refreshBag(gameManager* game); 

int collision(gameManager *game);

void hold(gameManager *game);
bool update(gameManager* game);

void rotatePiece(gameManager *game);
void moveUp(gameManager *game);
void moveDown(gameManager* game);
void moveL(gameManager *game);
void moveR(gameManager *game);
void drop(gameManager *game);
void getNextPiece(gameManager *game);
void spawnPiece(gameManager *game);

bool checkLine(gameManager *game, int y);
void clearLine(gameManager *game, int y);

void freezePiece(gameManager *game);

void calculateProj(gameManager *game);

bool inBounds(int x, int y){
  return x>=0 && y>=0 && x<SIZEX && y<SIZEY;
}

void initGame(gameManager* game){
  game->sizeX = SIZEX;
  game->sizeY = SIZEY;
  game->time = 0.0f;
  game->level = 0;
  game->score = 0;
  game->hold = _NULL;
  game->currPos = game->projectionPos = (pos){0,  0};
  game->bagSize = 0;
  game->pieceHeld = false;
  refreshBag(game);

  // create game boartd
  game->board = calloc(game->sizeY+5, sizeof(int*));
  for(int i=0; i<game->sizeY; i++)
    game->board[i] = calloc(game->sizeX+5, sizeof(int));
  
  // draw borders
  // bottom -> -3
  // left -> -1
  // right -> -2
  for(int i=0; i<game->sizeX; i++)
    game->board[0][i] =  bottom;
  for(int i=0; i<game->sizeY; i++){
    game->board[i][0] = left;
    game->board[i][SIZEX-1] = right;
  }
}

void exitGame(gameManager* game){
  for(int i=0; i<game->sizeY; i++)
    free(game->board[i]);
  free(game->board);
}

void refreshBag(gameManager* game){
  assert(game->bagSize==0, err, " {BUG} refreshBag() but bagsize not null! (bagsize=%d)", game->bagSize);

  bool used[TETRIMINOS+1] = {0, 0, 0, 0, 0, 0, 0, 0}; // inlcludes a position for a NULL tetrimino

  while(game->bagSize<TETRIMINOS){
    int piece;
    do{
      piece = rand()%7+1; 
    }while(used[piece]);
    used[piece] = 1;
    game->pieceBag[game->bagSize++] = pieces[piece];
  }
  game->next = game->pieceBag;
}

int size(enum Tetriminos piece){
  if(piece!=I && piece!=O) return 3;
  return 4;
}

void removePiece(gameManager* game){
  int cx = game->currPos.x;
  int cy = game->currPos.y;
  assert(game->curr!=_NULL, err, "{BUG} Tried to remove NULL piece.");
  for(int y=0; y<size(game->curr); y++)
    for(int x=0; x<size(game->curr); x++)
      if(game->currPieceState[y][x]>0) game->board[cy-y][x+cx] = 0;
}

void drawPiece(gameManager* game){
  for(int y = 0; y<size(game->curr); y++)
    for(int x = 0; x<size(game->curr); x++)
      if(game->currPieceState[y][x])
        game->board[game->currPos.y-y][x+game->currPos.x] = game->currPieceState[y][x];
}

void rotatePiece(gameManager *game){
  int n = size(game->curr);
  int buffer[4][4];

  for(int i = 0; i < n; i++){
    for(int j = 0, k = n-1; j<n; j++, k--)
      buffer[i][j] = game->currPieceState[k][i];
  }
  removePiece(game);
  
  // swap buffer and shape
  // buffer will hold the prev rotation in case it has to be reversed
  for(int y=0; y<n; y++)
    for(int x=0; x<n; x++){
      int aux = game->currPieceState[y][x];
      game->currPieceState[y][x] = buffer[y][x];
      buffer[y][x] = aux;
    }

  // move piece if it hits wall
  pos savePos = game->currPos;
  int coll;
  do{
    coll = collision(game);
    if(coll==left) game->currPos.x++;
    if(coll==right) game->currPos.x--;
    if(coll==bottom) game->currPos.y++;
  }while(coll<0);
  
  if(coll) {
    game->currPos = savePos;
    for(int y=0; y<n; y++)
      for(int x=0; x<n; x++)
        game->currPieceState[y][x] = buffer[y][x];
    drawPiece(game);
    return; // cannot rotate
  }

  drawPiece(game);
} 

void moveR(gameManager *game){
  removePiece(game);
  game->currPos.x++;
  int coll = collision(game);
  if(coll && coll!=-1) game->currPos.x--;
  drawPiece(game);
}

void moveL(gameManager *game){
  removePiece(game);
  game->currPos.x--;
  int coll = collision(game);
  if(coll && coll!=-2) game->currPos.x++;
  drawPiece(game);
}

void moveUp(gameManager *game){ // only for rotations
  removePiece(game);
  game->currPos.y++;
  int coll = collision(game);
  if(coll && coll!=-3) game->currPos.y--;
  drawPiece(game);
}

void moveDown(gameManager *game){
  removePiece(game);
  game->currPos.y--;
  int coll = collision(game);
  if(coll){
    game->currPos.y++;
    freezePiece(game);
  }
  drawPiece(game);
}

void drop(gameManager *game){
  removePiece(game);
  do{
    game->currPos.y--;
  }while(!collision(game));
  game->currPos.y++;
  freezePiece(game);
  drawPiece(game);
}

void spawnPiece(gameManager* game){
  pos center = {.x=SPWNX, .y=SPWNY};
  pos topleft = {.x=center.x-1, .y=center.y-1};
  game->currPos = topleft;

  // set state
  for(int y=0; y<size(game->curr); y++)
    for(int x=0; x<size(game->curr); x++){
      game->currPieceState[y][x] = piece[game->curr][y][x];
      assert(game->board[game->currPos.y-y][game->currPos.x+x]==0, err, "{GAME OVER} SPWN obstructed"); 
    }
  drawPiece(game);
}

void getNextPiece(gameManager* game){
  game->pieceHeld = false;
  game->bagSize--;
  game->curr = *(game->next);
  if(game->bagSize==0) refreshBag(game);
  else game->next++;
}

void cancelCurrPiece(gameManager* game){
  assert(game->curr!=_NULL, err, "{BUG} Tried to cancel NULL piece.");
  removePiece(game);
}

bool update(gameManager* game){
  bool rtrn = false;
  removePiece(game);
  if(getTime()-game->lastDescend>pieceFallDelay[game->level]){
    game->lastDescend = getTime();
    game->currPos.y--;

    int coll = collision(game);
    if(coll==-3 || coll>0){ // reached bottom
      game->currPos.y++;
      freezePiece(game);
    }
    rtrn = true;
  }
  calculateProj(game);
  drawPiece(game);
  return rtrn;
}

void freezePiece(gameManager *game){
  drawPiece(game);
  // clear all possible lines
  int lines = 0;
  for(int y = 0; y<size(game->curr); y++){
    if(checkLine(game, game->currPos.y-y)){
      clearLine(game, game->currPos.y-y);
      lines++;
    }
  }
  game->linesCleared+=lines;
  game->score+=scoreMultiplier[lines]*(game->level+1);

  getNextPiece(game);
  spawnPiece(game);
}

// check if line is full
bool checkLine(gameManager *game, int y){
  if(y<=0) return false;
  for(int x = 1; x<SIZEX-1; x++)
    if(!game->board[y][x]) return false;
  return true;
}

void clearLine(gameManager *game, int y){
  // clear line at y
  for(int x = 1; x<SIZEX-1; x++)
    game->board[y][x] = 0;
  
  // move all lines down
  for(int yy = y+1; yy<SIZEY; yy++)
    for(int x=1; x<SIZEX-1; x++)
      game->board[yy-1][x] = game->board[yy][x];
}

void hold(gameManager* game){
  if(game->pieceHeld) return; 
  cancelCurrPiece(game);
  if(game->hold==_NULL){
    game->hold = game->curr;
    getNextPiece(game);
  }
  else {
    enum Tetriminos aux = game->hold;
    game->hold = game->curr;
    game->curr = aux;
  }
  game->pieceHeld = true;
  spawnPiece(game);
  drawPiece(game);
}

int collision(gameManager *game){
  int cx = game->currPos.x;
  int cy = game->currPos.y;
  for(int y=0; y<size(game->curr); y++)
    for(int x=0; x<size(game->curr); x++){
      if(game->currPieceState[y][x])
        if(inBounds(x+cx, cy-y) && game->board[cy-y][cx+x]) return game->board[cy-y][cx+x];
    }
  return 0;
}

void calculateProj(gameManager *game){
  pos savePos = game->currPos;
  int coll;
  do{
    game->currPos.y--;
    coll = collision(game);
  }while(!coll);
  game->currPos.y++;
  game->projectionPos = game->currPos;
  game->currPos = savePos;
}
