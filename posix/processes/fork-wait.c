/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int first_child = fork();
  if (first_child == 0) {
    printf("in first child\n");
    sleep(10);
    printf("terminating first child\n");
    exit(0);
  }
  int second_child = fork();
  if (second_child == 0) {
    printf("in second child\n");
    sleep(30);
    printf("terminating second child\n");
    exit(0);
  }
  printf("in parent, children started\n");
  sleep(20);
  printf("collecting children\n");
  int i;
  int status;
  for (i = 0; i < 2; i++) {
    int pid = wait(&status);
    printf("child %d terminated (first_child=%d second_child=%d)\n", pid, first_child, second_child);
  }
  printf("done\n");
  exit(0);
}
