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

#include <itskylib.h>

int main(int argc, char *argv[]) {
  int fork_result;
  int pipes[2];
  char buff[1024];
  int retcode;
  char message[] = "This is the message to be transmitted";
  pid_t pid;
  pid_t ppid;

  pid =  getpid();
  ppid =  getppid();
  printf("Started pid=%d ppid=%d\n", pid, ppid);
  retcode = pipe(pipes);
  handle_error(retcode, "pipe", PROCESS_EXIT);
  fork_result = fork();

  pid =  getpid();
  ppid =  getppid();
  printf("fork_result=%d pid=%d ppid=%d\n", fork_result, pid, ppid);
  if (fork_result == 0) {
    printf("In child\n");
    close(pipes[0]);
    retcode = write(pipes[1], message, strlen(message));
    handle_error(retcode, "Error writing to pipe", PROCESS_EXIT);
    printf("%d chars written\n", retcode);
    close(pipes[1]);
  } else {
    printf("In parent: child_pid=%d\n", fork_result);
    close(pipes[1]);
    retcode = read(pipes[0], buff, strlen(message));
    handle_error(retcode, "Error reading from pipe", PROCESS_EXIT);
    printf("%d chars read\n", retcode);
    close(pipes[0]);
    printf("found message=\"%s\"\n", buff);
  }
  printf("sleeping\n");
  sleep(20);
  printf("done\n");
  exit(0);
}
