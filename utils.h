#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/time.h>
#include <stdbool.h>
#include <curses.h>

#pragma once

#define COLOR_RESET "\033[0m"
#define ASSERT_FILE stdout
#define EXITONERR 1

#define ENDL "\n\r" // curses mode

enum Type{
  info = 0,
  warn = 1,
  err = 2
};

const char* colors[] = { "\033[34m", "\033[33m", "\033[31m"};
const char* prefix[] = {"[INFO] ", "[WARN] ", "[ERR] "};


/* @debug
* type:
* 0 -> info
* 1 -> warn
*  -> error
* 
*/
void debug(FILE* file, const enum Type type, const char* msg, ...);

void terminate(int code);

int getAsyncInput(){
  int input = getch();
  return input;
}

void waitFor(int input){
  int c;
  do{
    c = getAsyncInput();
  }while(c!=input);
}

// log a message to log file
void logger(FILE* file, const enum Type type, const char* msg, va_list args){
  fprintf(file, "%s", colors[type]);
  fprintf(file, "%s", prefix[type]);
  vfprintf(file, msg, args);
  fprintf(file, "%s", ENDL);
  fprintf(file, COLOR_RESET);
}

// self explanatory
void debug(FILE* file, const enum Type type, const char* msg, ...){
  va_list args;
  va_start(args, msg);
  logger(file, type, msg, args);
  va_end(args);
}

// custom assert function, uk how it works
void assert(bool expressionValue, const enum Type type, const char* msg, ...){
  if(expressionValue) return;
  va_list args;
  va_start(args, msg);
  logger(ASSERT_FILE, type, msg, args);
  va_end(args);
  if(type==err && EXITONERR) terminate(3);
}

// convert time from timeval to a simple double (seconds)
double getTime(){
  struct timeval moment;
  gettimeofday(&moment, NULL);
  double timeInSeconds = moment.tv_sec + moment.tv_usec*1e-6;
  return timeInSeconds;
}

void terminate(int code){
  debug(stdout, info, "Terminating...");
  printf("\33[?25h");
  system("stty echo");
  exit(code);
}
