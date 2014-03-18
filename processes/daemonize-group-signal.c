/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#include <itskylib.h>

int daemonized = 0;

void signal_handler(int signo) {
  daemonized = 1;
}

int main(int argc, char *argv[]) {
  int fork_result;
  // int retcode;
  int status;
  int pid = getpid();

  /* set pgid to pid */
  setpgid(pid, pid);
  signal(SIGUSR1, signal_handler);

  /* first fork() to create child */
  fork_result = fork();
  if (fork_result == 0) {
    printf("In child\n");

    /* second fork to create grand child */
    fork_result = fork();
    if (fork_result != 0) {
      /* exit child, make grand child a daemon */
      printf("exiting child\n");
      exit(0);
    }
    
    /* in daemon (grand child) */
    pid = getpid();
    while (! daemonized) {
      pause();
    }

    printf("daemon has pid=%d pgid=%d ppid=%d\n", pid, getpgid(pid), getppid());

    /* do daemon stuff */
    sleep(30);
    printf("done with daemon\n");
    exit(0);
  }
  printf("parent waiting for child\n");
  wait(&status);
  printf("child terminated\n");
  kill(-getpid(), SIGUSR1);
  printf("parent done\n");
}
