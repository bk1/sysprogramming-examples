/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include <itskylib.h>

void exit_handler() {
  printf("exting (in exit_handler)\n");
}

void usage(const char *argv0, const char *msg) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("Usage\n\n");
  printf("%s -d\n\texample of terminating program with pthread_exit instead of exit\n\n", argv0);
  exit(1);
}

int main(int argc, char *argv[]) {

  if (is_help_requested(argc, argv)) {
    usage(argv[0], "");
  }

  atexit(exit_handler);
  printf("main stared\n");
  pthread_exit(NULL);
  printf("after exit");
  return 0;
}
