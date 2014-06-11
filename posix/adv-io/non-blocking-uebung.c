/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Steve Heller
 * Date: 2014-06-02
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>  

enum blocking { BLOCKING, NON_BLOCKING };

int setup = 0;

void do_blocking() {
  while(1) {
    char input_string[256];
    char *p;
    unsigned int inputStringLen; 
    fgets(input_string, sizeof(input_string), stdin);       
    if ((p = strchr(input_string, '\n')) != NULL) {
      *p = '\0';
    }
    printf("You entered: %s\n",input_string);
  }
}

void do_non_blocking() {
  fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
  while(1) {
    char input_string[256];
    char *p;
    unsigned int inputStringLen;
    memset(input_string, 0, sizeof(input_string));
    fgets(input_string, sizeof(input_string), stdin);       
    if ((p = strchr(input_string, '\n')) != NULL) {
      *p = '\0';
    }
    if (input_string[0] == 0) {
      printf("You entered nothing!\n");
    } else {
      printf("You entered: %s\n",input_string);
    }
    sleep(1);
  }
}

void do_stuff() {
  if (setup == BLOCKING) { // blocking
    do_blocking();
  } else if (setup == NON_BLOCKING) { // non-blocking
    do_non_blocking();
  } else {
    printf("WRONG USAGE, TRY AGAIN\n");
  }
}

void main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("usage:\n%s -b for blocking\n%s -n for non-blocking\n",argv[0],argv[0]);
    exit(1);
  }
  if (strcmp(argv[1], "-b") == 0) {
    setup = BLOCKING;
  } else if (strcmp(argv[1], "-n") == 0) {
    setup = NON_BLOCKING;
  }
  do_stuff();
  exit(0);
}
