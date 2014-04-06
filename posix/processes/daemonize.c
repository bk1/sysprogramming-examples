/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

/* example of daemonization */

void handle_error_myerrno(int return_code, int myerrno, char *msg) {
    const char *error_str = strerror(myerrno);
    printf("%s return_code=%d\nerrno=%d\nmessage=%s\n", msg, return_code, myerrno, error_str);
    exit(1);
}

void handle_error(int return_code, char *msg) {
  if (return_code < 0) {
    int myerrno = errno;
    handle_error_myerrno(return_code, myerrno, msg);
  }
}

int main(int argc, char *argv[]) {
  printf("started parent\n");
  printf("starting child\n");
  int pid = fork();
  if (pid < 0) {
    handle_error(pid, "could not fork child from parent\n");
    exit(1);
  } else if (pid > 0) {
    printf("in parent child-pid=%d\n", pid);
    sleep(10);
    printf("finishing parent\n");
    exit(0);
  } else {
    // now assume else if (pid == 0) {
    printf("in child pid=%d pgid=%d ppid=%d\n", getpid(), getpgid(getpid()), getppid());
    printf("starting daemon (grand child)");
    int pid = fork();
    if (pid < 0) {
      handle_error(pid, "could not fork grand child from child\n");
      exit(1);
    } else if (pid > 0) {
      printf("in child grand-child-pid=%d\n", pid);
      printf("finishing child\n");
      exit(0);
    } else {
      printf("in grand child pid=%d pgid=%d ppid=%d\n", getpid(), getpgid(getpid()), getppid());
      while (getppid() != 1) {
        printf("waiting for init to become ppid\n");
        sleep(1);
      }
      printf("daemonized");
      int i;
      for (i = 0; i < 100; i++) {
        printf("in daemon: %3d**2 = %5d\n", i, i*i);
        sleep(1);
      }
      printf("done with daemon terminating\n");
      exit(0);
    }
  }

}
