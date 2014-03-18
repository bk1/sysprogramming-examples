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

const int SIZE = 1024;
int daemonized = FALSE;

void signal_handler(int signo) {
  daemonized = TRUE;
  int pid = getpid();
  int ppid = getppid();
  printf("signal %d (%s) received pid=%d ppid=%d\n", signo, strsignal(signo), pid, ppid);
}

int main(int argc, char *argv[]) {
  int retcode;

  int fork_result;
  int status;
  int pid = getpid();

  int pipes[2];
  char buff[SIZE];

  printf("started pid=%d pgid=%d\n", pid, getpgid(pid));

  /* create pipes */
  retcode = pipe(pipes);
  handle_error(retcode, "pipe", PROCESS_EXIT);

  sigset_t sig_mask;
  retcode = sigemptyset(&sig_mask);
  handle_error(retcode, "sigemptyset", PROCESS_EXIT);
  retcode = sigaddset(&sig_mask, SIGUSR1);
  handle_error(retcode, "sigaddset", PROCESS_EXIT);

  struct sigaction new_sigaction;
  struct sigaction old_sigaction;
  /* assign unused fields to null *first*, so if there is a union the real values will supersede */
  new_sigaction.sa_sigaction = NULL;
  new_sigaction.sa_restorer = NULL;
  new_sigaction.sa_handler = signal_handler;
  new_sigaction.sa_mask = sig_mask;
  new_sigaction.sa_flags = SA_NOCLDSTOP;
  retcode = sigaction(SIGUSR1, &new_sigaction, &old_sigaction);
  handle_error(retcode, "sigaction", PROCESS_EXIT);

  /* first fork() to create child */
  fork_result = fork();
  handle_error(fork_result, "fork (1)", PROCESS_EXIT);
  if (fork_result == 0) {
    printf("In child\n");

    /* second fork to create grand child */
    fork_result = fork();
    handle_error(fork_result, "fork (2)", PROCESS_EXIT);
    if (fork_result != 0) {
      retcode = close(pipes[0]);
      handle_error(retcode, "close(pipes[0])", PROCESS_EXIT);
      sprintf(buff, "%d", fork_result);
      printf("transmitting daemon_pid=%d=%s from child to parent\n", fork_result, buff);
      retcode = write(pipes[1], buff, strlen(buff));
      handle_error(retcode, "write", PROCESS_EXIT);
      retcode = close(pipes[1]);
      handle_error(retcode, "close(pipes[1])", PROCESS_EXIT);

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
    handle_error(retcode, "sigaction (2)", PROCESS_EXIT);

    /* do daemon stuff */
    printf("doing daemon stuff\n");
    sleep(30);
    printf("done with daemon\n");
    exit(0);
  }
  /* in parent */
  retcode = close(pipes[1]);
  handle_error(retcode, "close(pipes[1])", PROCESS_EXIT);
  read(pipes[0], buff, SIZE);
  retcode = close(pipes[0]);
  handle_error(retcode, "close(pipes[0])", PROCESS_EXIT);
  
  int daemon_pid;
  sscanf(buff, "%d", &daemon_pid);
  printf("parent has daemon_pid=%s=%d\n", buff, daemon_pid);
  printf("parent waiting for child\n");
  retcode = wait(&status);
  handle_error(retcode, "wait", PROCESS_EXIT);
  printf("child terminated\n");
  retcode = kill(daemon_pid, SIGUSR1);
  handle_error(retcode, "kill", PROCESS_EXIT);
  printf("parent done\n");
  exit(0);
}
