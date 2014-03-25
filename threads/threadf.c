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

#include <itskylib.h>

int pipes[2];
int message_len;

void *thread_run(void *ptr) {
  char message[] = "This is the message to be transmitted";
  int retcode;
  sleep(1);
  message_len = strlen(message);
  pid_t pid =  getpid();
  pid_t ppid =  getppid();
  pthread_t tid = pthread_self();
  printf("In child: retcode=%d tid=%ld pid=%d ppid=%d\n", retcode, (long) tid, pid, ppid);
  printf("In child\n");
  retcode = write(pipes[1], message, strlen(message));
  handle_error(retcode, "Error writing to pipe retcode=%d errno=%d\n", THREAD_EXIT);
  printf("%d chars written\n", retcode);
  close(pipes[1]);
  return (void *) NULL;
}

void usage(const char *argv0, const char *msg) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("Usage\n\n");
  printf("%s\n\tExample of creating a thread and communicating from new thread to original thread via anonymous pipe\n\n", argv0);
  exit(1);
}

int main(int argc, char *argv[]) {

  if (is_help_requested(argc, argv)) {
    usage(argv[0], "");
  }

  char buff[1024];
  int retcode;
  pid_t pid;
  pid_t ppid;
  pthread_t thread1;

  pid =  getpid();
  ppid =  getppid();
  printf("Started pid=%d ppid=%d\n", pid, ppid);
  retcode = pipe(pipes);
  handle_error(retcode, "Error creating pipe", PROCESS_EXIT);
  retcode = pthread_create(&thread1, NULL, thread_run, NULL);
  handle_thread_error(retcode, "Error creating thread", PROCESS_EXIT);
  pthread_t tid = pthread_self();
  printf("In parent: retcode=%d tid=%ld pid=%d ppid=%d\n", retcode, (long) tid, pid, ppid);
  retcode = read(pipes[0], buff, message_len);
  handle_error(retcode, "Error reading from pipe", PROCESS_EXIT);
  printf("%d chars read\n", retcode);
  close(pipes[0]);
  printf("found message=\"%s\"\n", buff);
  pthread_join(thread1, NULL);
  printf("done\n");
  exit(0);
}
