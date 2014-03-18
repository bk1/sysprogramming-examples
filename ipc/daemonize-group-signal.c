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

int daemonized = FALSE;

void signal_handler(int signo) {
  daemonized = TRUE;
  int pid = getpid();
  int pgid = getpgid(pid);
  int ppid = getppid();
  printf("signal %d received pid=%d pgid=%d ppid=%d\n", signo, pid, pgid, ppid);
}

int main(int argc, char *argv[]) {
  int retcode;

  int fork_result;
  int status;
  int pid = getpid();
  int pgid = pid;

  /* set pgid to pid */
  retcode = setpgid(pid, pgid);
  handle_error(retcode, "setpgid", PROCESS_EXIT);

  /* set signal handler */
  void *ptr = signal(SIGUSR1, signal_handler);
  handle_ptr_error(ptr, "signal", PROCESS_EXIT);

  /* first fork() to create child */
  fork_result = fork();
  handle_error(fork_result, "fork (1)", PROCESS_EXIT);
  if (fork_result == 0) {
    printf("In child\n");

    /* second fork to create grand child */
    fork_result = fork();
    handle_error(fork_result, "fork (2)", PROCESS_EXIT);
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
  retcode = wait(&status);
  handle_error(retcode, "wait", PROCESS_EXIT);
  printf("child terminated status=%d\n", status);
  retcode = kill(-pgid, SIGUSR1);
  handle_error(retcode, "kill", PROCESS_EXIT);
  printf("parent done\n");
  exit(0);
}
