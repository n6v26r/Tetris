#include "utils.h"
#include "tetris.h"
#include "display.h"
#include <ctype.h>
#include <curses.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#pragma optimize GCC("O2")

#define INPUT_DELAY 0.07f

double lastInputRead = 0.0f;

gameManager game;

bool parseInput(gameManager *game){
  bool updatedPiece = false;
  int input = getAsyncInput();
  if(!(isalpha(input) || isspace(input))) return false;

  if(getTime()-lastInputRead<=INPUT_DELAY)
    return false;

  lastInputRead = getTime();
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
    case 'B':
      moveDown(game);
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
    case 'p': // pause
      waitFor('p');
      break;
    case 'e':
      exitDisplay();
      exit(0);
      break;
    default: 
      break;
  }
  return updatedPiece;
}

int main(){
  signal(SIGINT, SIG_IGN); // ignore ctrl-c
  srand(time(NULL)); // set rand seed to unix time
  debug(stdout, info, "Starting game...");
  
  initDisplay(); // hide cursor and stuff
  initGame(&game);
  getNextPiece(&game);
  spawnPiece(&game);
 
  // will implement levels soon
  game.level = 16;
  while(1){
    // Get input
    if(parseInput(&game)) drawBoard(&game);

    if(update(&game)) drawBoard(&game);
  }
  exitDisplay();
  return 0;
}
