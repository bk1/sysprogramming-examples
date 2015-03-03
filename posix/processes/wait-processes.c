/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2015-03-03
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */


#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

#include <itskylib.h>

const int NUMBER_OF_CHILDREN = 5;

int main(int argc, char *argv[]) {

  pid_t child_pids[NUMBER_OF_CHILDREN];
  for (int i = 0; i < NUMBER_OF_CHILDREN; i++) {
    pid_t pid = fork();
    handle_error(pid, "fork", PROCESS_EXIT);
    if (pid == 0) {
      printf("starting child: i=%d pid=%d ppid=%d\n", i, getpid(), getppid());
      sleep(3*(NUMBER_OF_CHILDREN - i));
      printf("ending child: i=%d pid=%d ppid=%d\n", i, getpid(), getppid());
      exit(0);
    } else {
      child_pids[i] = pid;
      printf("parent: started pid=%d\n", pid);
      sleep(1);
    }
  }
  printf("in parent\n");
  //for (int i = 0; i < NUMBER_OF_CHILDREN; i++) {
  for (int i = NUMBER_OF_CHILDREN -1; i>= 0; i--) {
    int status;
    printf("parent waiting for pid=%d\n", child_pids[i]);
    pid_t pid = waitpid(child_pids[i], &status, 0);
    printf("parent: i=%d waited for pid=%d exited=%d es=%d\n", i, pid, WIFEXITED(status), WEXITSTATUS(status));
  }
}
