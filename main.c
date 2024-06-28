#include "utils.h"
#include "tetris.h"
#include "display.h"
#include <curses.h>
#include <time.h>
#include <unistd.h>
#pragma optimize GCC("O2")

#define INPUT_DELAY 1000 // microseconds

double lastInputRead;

gameManager game;

bool parseInput(gameManager *game){
  bool updatedPiece= false;
  int input = getAsyncInput();
  switch(input){
    case 'A':
      rotatePiece(game);
      updatedPiece = true;
      break;
    case 'D':
      moveL(game);
      updatedPiece = true;
      break;
    case 'C':
      moveR(game);
      updatedPiece = true;
      break;
    case ' ':
      drop(game);
      updatedPiece = true;
      break;
    case 'c':
      hold(game);
      updatedPiece = true;
      break;
    default: // still have to implement force drop
      break;
  }
  return updatedPiece;
}

int main(){
  srand(time(NULL)); // set rand seed to unix time
  initDisplay(); // hide cursor and stuff
 
  // Configure for async input using curses
  WINDOW* w = initscr();
  noecho(); 
  nodelay(w, 1);

  debug(stdout, info, "Starting game...");
  initGame(&game);
  getNextPiece(&game);
  spawnPiece(&game);
  
  // make debbugging faster
  game.level = 16;
  while(1){
    // Get input
    if(parseInput(&game)) drawBoard(&game);

    if(update(&game)) drawBoard(&game);
    usleep(1000); 
  }
  exitDisplay();
  return 0;
}
