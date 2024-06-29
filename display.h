// This is made with stdout in mind as output
#include "tetris.h"
#include "utils.h"
#include <asm-generic/ioctls.h>
#include <curses.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define MINROWS 30
#define MINCOLS 20

WINDOW* w;

void clearScreen();

void drawBaord();

void initDisplay(){
  struct winsize win;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);
  assert(win.ws_row>=MINROWS && win.ws_col>=MINCOLS, err, "Window size must be at least %dx%d",MINROWS, MINCOLS);
  system("stty -echo"); // disable text echo
  // TODO: Find a bttr way to disable cursor
  printf("\33[?25l");
  // Configure for async input using curses
  w = initscr();
  noecho(); 
  nodelay(w, 1);
}

void drawBoard(gameManager* game){
  clearScreen();
  for(int i=game->sizeY-1; i>=0; i--){
    for(int j=0; j<game->sizeX; j++){
      // in projection box bounds
      if(game->projectionPos.y-i<size(game->curr) && game->projectionPos.y-i>=0 && j-game->projectionPos.x<size(game->curr) && j-game->projectionPos.x>=0){
        if(game->currPieceState[game->projectionPos.y-i][j-game->projectionPos.x] && !game->board[i][j]) printf("..");
	else if(game->board[i][j]>0) printf("[]");
        else if(game->board[i][j]==0)printf("  ");
        else printf("##");
      }
      else{
        if(game->board[i][j]>0) printf("[]");
        else if(game->board[i][j]==0)printf("  ");
        else printf("##");
      }
    }
    printf("%s", ENDL);
  }
}

void clearScreen(){
  system("clear");
}

void exitDisplay(){
  echo();
  nodelay(w, 0);
  endwin();
  printf("\33[?25h");
  system("stty echo");
}
