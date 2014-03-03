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


int main(int argc, char *argv[]) {
  int fork_result;
  int pipes[2];
  char buff[1024];
  int ret_code;
  char message[] = "This is the message to be transmitted";
  printf("Started parent\n");
  ret_code = pipe(pipes);
  if (ret_code != 0) {
    printf("Error creating pipe ret_code=%d errno=%d\n", ret_code, errno);
    exit(1);
  }
  fork_result = fork();
  if (fork_result == 0) {
    printf("In child\n");
    fork_result = fork();
    if (fork_result != 0) {
      printf("exiting child\n");
      exit(0);
    }
    close(pipes[1]);
    printf("in grandchild: waiting for pipe\n");
    ret_code = read(pipes[0], buff, strlen(message));
    printf("received message=%s daemonized\n", message);
    close(pipes[0]);
    printf("ppid=%d\n", getppid());
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
  ret_code = write(pipes[1], message, strlen(message));
  if (ret_code < 0) {
    printf("Error writing to pipe ret_code=%d errno=%d\n", ret_code, errno);
    exit(2);
  }
  printf("parent done\n");
}
