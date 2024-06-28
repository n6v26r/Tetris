// This is made with stdout in mind as output
#include "tetris.h"
#include "utils.h"
#include <asm-generic/ioctls.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define MINROWS 30
#define MINCOLS 20

void clearScreen();

void drawBaord();

void initDisplay(){
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  assert(w.ws_row>=MINROWS && w.ws_col>=MINCOLS, err, "Window size must be at least %dx%d",MINROWS, MINCOLS);
  system("stty -echo"); // disable text echo
  printf("\33[?25l");
}

void drawBoard(gameManager* game){
  clearScreen();
  for(int i=game->sizeY-1; i>=0; i--){
    for(int j=0; j<game->sizeX; j++){
      if(game->board[i][j]>0) printf("%d", game->board[i][j]);
      else if(game->board[i][j]==0)printf(" ");
      else printf("#");
    }
    printf("%s", ENDL);
  }
}

void clearScreen(){
  system("clear");
}

void exitDisplay(){
  printf("\33[?24h");
  system("stty echo");
}
