/* (C) IT Sky Consulting GmbH 2015
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2015-03-09
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <itskylib.h>

void usage(char *argv[], char *msg) {
  printf("%s\n\n", msg);
  printf("Usage:\n\n%s signo\n", argv[0]);
  exit(1);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    usage(argv, "wrong number of arguments");
  }
  int signo = atoi(argv[1]);
  if (signo <= 0) {
    usage(argv, "signo must be > 0");
  }
  if (signo >= 256) {
    usage(argv, "signo must be < 256");
  }
  char *signame = strsignal(signo);
  if (signame == NULL) {
    usage(argv, "signo must be defined:\nsee man 7 signal\n");
  }
  printf("using signo=%d (%s)\n", signo, signame);

  int pid = fork();
  if (pid == 0) {
    printf("in child: pid=%d ppid=%d\n", getpid(), getppid());
    while (1) {
      sleep(1);
    }
  } else {
    printf("in parent\n");
    sleep(1);
    int retcode = kill(pid, signo);
    handle_error(retcode, "kill", PROCESS_EXIT);
  }
  exit(0);
}
