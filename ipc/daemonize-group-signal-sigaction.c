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

#define FALSE 0
#define TRUE  1

int daemonized = FALSE;

/* helper function for dealing with errors */
void handle_error(int return_code) {
  if (return_code < 0) {
    int myerrno = errno;
    const char *error_str = strerror(myerrno);
    printf("return_code=%d\nerrno=%d\nmessage=%s\n", return_code, myerrno, error_str);
    exit(1);
  }
}

void signal_handler(int signo) {
  daemonized = TRUE;
  int pid = getpid();
  int pgid = getpgid(pid);
  int ppid = getppid();
  printf("signal %d (%s) received pid=%d pgid=%d ppid=%d\n", signo, strsignal(signo), pid, pgid, ppid);
}

int main(int argc, char *argv[]) {
  int retcode;

  int fork_result;
  int status;
  int pid = getpid();
  int pgid = pid;

  /* set pgid to pid */
  retcode = setpgid(pid, pgid);
  handle_error(retcode);

  printf("started pid=%d pgid=%d\n", pid, getpgid(pid));

  sigset_t sig_mask;
  retcode = sigemptyset(&sig_mask);
  handle_error(retcode);
  retcode = sigaddset(&sig_mask, SIGUSR1);
  handle_error(retcode);

  struct sigaction new_sigaction;
  struct sigaction old_sigaction;
  /* assign unused fields to null *first*, so if there is a union the real values will supersede */
  new_sigaction.sa_sigaction = NULL;
  new_sigaction.sa_restorer = NULL;
  new_sigaction.sa_handler = signal_handler;
  new_sigaction.sa_mask = sig_mask;
  new_sigaction.sa_flags = SA_NOCLDSTOP | SA_RESTART;
  retcode = sigaction(SIGUSR1, &new_sigaction, &old_sigaction);
  handle_error(retcode);

  /* first fork() to create child */
  fork_result = fork();
  handle_error(fork_result);
  if (fork_result == 0) {
    printf("In child\n");

    /* second fork to create grand child */
    fork_result = fork();
    handle_error(fork_result);
    if (fork_result != 0) {
      /* exit child, make grand child a daemon */
      printf("exiting child\n");
      exit(0);
    }

    /* in daemon (grand child) */
    printf("in grand child (daemon)\n");
    pid = getpid();
    while (! daemonized) {
      pause();
    }

    printf("daemonized: daemon has pid=%d pgid=%d ppid=%d\n", pid, getpgid(pid), getppid());

    /* restoring signal handler to what it has been before */
    retcode = sigaction(SIGUSR1, &old_sigaction, NULL);
    handle_error(retcode);

    /* do daemon stuff */
    printf("doing daemon stuff\n");
    sleep(30);
    printf("done with daemon\n");
    exit(0);
  }
  printf("parent waiting for child\n");
  retcode = wait(&status);
  handle_error(retcode);
  printf("child terminated\n");
  retcode = kill(-pgid, SIGUSR1);
  handle_error(retcode);
  printf("parent done\n");
  exit(0);
}
