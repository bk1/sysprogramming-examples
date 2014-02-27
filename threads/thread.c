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
#include <pthread.h>

int pipes[2];
int message_len;

void *thread_run(void *ptr) {
  char message[] = "This is the message to be transmitted";
  int ret_code;
  message_len = strlen(message);
  printf("In child\n");
  ret_code = write(pipes[1], message, strlen(message));
  if (ret_code < 0) {
    printf("Error writing to pipe ret_code=%d errno=%d\n", ret_code, errno);
    exit(2);
  }
  printf("%d chars written\n", ret_code);
  close(pipes[1]);
}

int main(int argc, char *argv[]) {
  char buff[1024];
  int ret_code;
  pid_t pid;
  pid_t ppid;
  pthread_t thread1;

  pid =  getpid();
  ppid =  getppid();
  printf("Started pid=%d ppid=%d\n", pid, ppid);
  ret_code = pipe(pipes);
  if (ret_code != 0) {
    printf("Error creating pipe ret_code=%d errno=%d\n", ret_code, errno);
    exit(1);
  }
  ret_code = pthread_create(&thread1, NULL, thread_run, NULL);
  pid =  getpid();
  ppid =  getppid();
  printf("ret_code=%d pid=%d ppid=%d\n", ret_code, pid, ppid);
  sleep(1);
  printf("In parent\n");
  ret_code = read(pipes[0], buff, message_len);
  if (ret_code < 0) {
    printf("Error reading from pipe ret_code=%d errno=%d\n", ret_code, errno);
    exit(3);
  }
  printf("%d chars read\n", ret_code);
  close(pipes[0]);
  printf("found message=\"%s\"\n", buff);
  pthread_join( thread1, NULL);
  printf("done\n");
}
