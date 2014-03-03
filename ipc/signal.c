/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void my_handler(int signo) {
  if (signo == SIGTERM) {
    printf("SIGTERM erhalten und ignoriert\n");
  } else if (signo == SIGINT) {
    printf("Ctrl-C ignoriert\n");
  } else {
    printf("unbekanntes Signal %d ignoriert\n", signo);
  }
}

int main(int argc, char *argv[]) {
  signal(SIGTERM, my_handler);
  signal(SIGINT, my_handler);
  signal(SIGTSTP, SIG_IGN);
  int i;
  for (i = 0; i < 100; i++) {
    printf("%2d^2 = %4d\n", i, i*i);
    sleep(1);
  }
  exit(0);
}
