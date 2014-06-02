/* Author: Steve Heller
 * Date: 02.06.14
 * Homework: KW 23
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>  

// 1 = blocking
// 2 = non-blocking
int setup = 0;

void doStuff() {
  if (setup == 1) { // blocking
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
  } else if (setup == 2) { // non-blocking
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
  } else {
    printf("WRONG USAGE, TRY AGAIN\n");
  }
}

void main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("usage:\n%s -b for blocking\n%s -n for non-blocking\n",argv[0],argv[0]);
  } else {
    if (strcmp(argv[1], "-b") == 0) {
      setup = 1;
    } else if (strcmp(argv[1], "-n") == 0) {
      setup = 2;
    }
  doStuff();
  }
}
