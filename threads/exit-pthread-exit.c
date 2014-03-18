/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void exit_handler() {
  printf("exting\n");
}

int main(int argc, char *argv[]) {
  atexit(exit_handler);
  printf("main stared\n");
  pthread_exit(NULL);
  printf("after exit");
  return 0;
}
