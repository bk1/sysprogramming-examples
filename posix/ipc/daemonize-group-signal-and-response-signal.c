/* (C) IT Sky Consulting GmbH 2015
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2015-03-11
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
#include <syslog.h>


#include <itskylib.h>

int daemonized = FALSE;

int status = -1;

int parent_pid;

void signal_failure() {
  int retcode = kill(parent_pid, SIGCHLD);
  handle_error_syslog(retcode, "daemon: kill SIGCHLD", PROCESS_EXIT);
}

void daemon_failure_exit_handler() {
  syslog(LOG_NOTICE, "exit_handler: notifying parent\n");
  signal_failure();
  syslog(LOG_NOTICE, "exit_handler: exiting");
}

void signal_success() {
  int retcode = kill(parent_pid, SIGUSR2);
  handle_error_syslog(retcode, "daemon: kill SIGUSR2", PROCESS_EXIT);
}

void daemonize() {
  int retcode = atexit(daemon_failure_exit_handler);
  handle_error_syslog(retcode, "daemon: atexit", PROCESS_EXIT);
  retcode = close(STDIN_FILENO);
  handle_error_syslog(retcode, "daemon: close stdin", PROCESS_EXIT);
  retcode = close(STDOUT_FILENO);
  handle_error_syslog(retcode, "daemon: close stdout", PROCESS_EXIT);
  retcode = close(STDERR_FILENO);
  handle_error_syslog(retcode, "daemon: close stderr", PROCESS_EXIT);
  daemonized = TRUE;
}

void daemonize_signal_handler(int signo) {
  int pid = getpid();
  int pgid = getpgid(pid);
  int ppid = getppid();
  if (pid == parent_pid) {
    printf("parent: signal %d (%s) received pid=%d pgid=%d ppid=%d\n", signo, strsignal(signo), pid, pgid, ppid);
  } else {
    printf("daemon: signal %d (%s) received pid=%d pgid=%d ppid=%d\n", signo, strsignal(signo), pid, pgid, ppid);
    daemonize();
  }
}

void parent_signal_handler(int signo) {
  if (signo == SIGCHLD) {
    printf("parent: error in daemon: see syslog for details\n");
    status = 1;
  } else if (signo == SIGUSR2) {
    printf("parent: daemon successfully activated, parent is exiting\n");
    status = 0;
  }
}

int main(int argc, char *argv[]) {
  int retcode;

  int fork_result;
  int status2;
  int pid = getpid();
  int pgid = pid;
  parent_pid = pid;

  /* set pgid to pid */
  retcode = setpgid(pid, pgid);
  handle_error(retcode, "setpgid", PROCESS_EXIT);

  printf("started pid=%d pgid=%d\n", pid, getpgid(pid));

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
  new_sigaction.sa_handler = daemonize_signal_handler;
  new_sigaction.sa_mask = sig_mask;
  new_sigaction.sa_flags = SA_NOCLDSTOP | SA_RESTART;
  retcode = sigaction(SIGUSR1, &new_sigaction, &old_sigaction);
  handle_error(retcode, "sigaction (1)", PROCESS_EXIT);

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
    printf("in grand child (daemon)\n");
    pid = getpid();
    while (! daemonized) {
      pause();
    }
    signal_success();
    syslog(LOG_NOTICE, "daemonized: daemon has pid=%d pgid=%d ppid=%d\n", pid, getpgid(pid), getppid());

    /* restoring signal handler to what it has been before */
    retcode = sigaction(SIGUSR1, &old_sigaction, NULL);
    handle_error_syslog(retcode, "sigaction (2)", PROCESS_EXIT);

    /* do daemon stuff */
    syslog(LOG_NOTICE, "doing daemon stuff\n");
    sleep(30);
    syslog(LOG_NOTICE, "done with daemon\n");
    exit(0);
  } else {
    printf("parent waiting for child\n");
    retcode = wait(&status2);
    handle_error(retcode, "wait", PROCESS_EXIT);
    printf("child terminated\n");

    sigset_t parent_sig_mask;
    retcode = sigemptyset(&parent_sig_mask);
    handle_error(retcode, "sigemptyset", PROCESS_EXIT);
    retcode = sigaddset(&parent_sig_mask, SIGUSR2);
    handle_error(retcode, "sigaddset", PROCESS_EXIT);
    retcode = sigaddset(&parent_sig_mask, SIGCHLD);
    handle_error(retcode, "sigaddset", PROCESS_EXIT);

    struct sigaction new_parent_sigaction;
    struct sigaction old_parent_sigaction1;
    struct sigaction old_parent_sigaction2;
    /* assign unused fields to null *first*, so if there is a union the real values will supersede */
    new_parent_sigaction.sa_sigaction = NULL;
    new_parent_sigaction.sa_restorer = NULL;
    new_parent_sigaction.sa_handler = parent_signal_handler;
    new_parent_sigaction.sa_mask = sig_mask;
    new_parent_sigaction.sa_flags = SA_NOCLDSTOP | SA_RESTART;
    retcode = sigaction(SIGUSR2, &new_parent_sigaction, &old_parent_sigaction1);
    retcode = sigaction(SIGCHLD, &new_parent_sigaction, &old_parent_sigaction2);
    handle_error(retcode, "parent: sigaction (1)", PROCESS_EXIT);
    
    retcode = kill(-pgid, SIGUSR1);
    handle_error(retcode, "kill", PROCESS_EXIT);
    while (TRUE) {
      pause();
      if (status >= 0) {
        break;
      }
    }
    printf("parent done status=%d\n", status);
    exit(status);
  }
  exit(0);
}
