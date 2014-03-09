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
#include <unistd.h>
#include <sys/wait.h>

/* helper function for dealing with errors */
void handle_error(int return_code, char *msg) {
  if (return_code < 0) {
    int myerrno = errno;
    const char *error_str = strerror(myerrno);
    printf("%s\nreturn_code=%d\nerrno=%d\nmessage=%s\n", msg, return_code, myerrno, error_str);
    exit(1);
  }
}

int main(int argc, char *argv[]) {
  int fork_result;
  int pipes[2];
  char buff[1024];
  int retcode;
  /* message content is not relevant... */
  char message[] = "X";
  printf("Started parent\n");
  retcode = pipe(pipes);
  handle_error(retcode, "Error creating pipe");
  
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
    
    /* in grand child */
    /* close write end of pipe */
    retcode = close(pipes[1]);
    handle_error(retcode, "error closing pipe in grandchild (daemon)");

    /* read from pipe */
    printf("in grandchild: waiting for pipe\n");
    retcode = read(pipes[0], buff, strlen(message));
    handle_error(retcode, "error reading from pipe in grandchild");

    printf("grandchild has received message from grand parent: daemonized\n");
    /* pipe has done its purpose, close it */
    retcode = close(pipes[0]);
    handle_error(retcode, "error closing pipe in daemon");

    printf("daemon has ppid=%d\n", getppid());

    /* do daemon stuff */
    sleep(30);
    printf("done with daemon\n");
    exit(0);
  }

  /* in parent */
  close(pipes[0]);
  int status;
  printf("parent waiting for child\n");
  wait(&status);
  printf("child terminated\n");
  retcode = write(pipes[1], message, strlen(message));
  handle_error(retcode, "error writing to pipe in parent");

  retcode = close(pipes[1]);
  handle_error(retcode, "error closing pipe in parent");
  
  printf("parent done\n");
  exit(0);
}
