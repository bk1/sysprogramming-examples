/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <unistd.h>

#define FALSE 0
#define TRUE  1
#define SIZE 20

/* helper function for dealing with errors */
void handle_error(int return_code, const char *msg) {
  if (return_code < 0) {
    int myerrno = errno;
    const char *error_str = strerror(myerrno);
    if (msg != NULL) {
      printf("%s\n", msg);
    }
    printf("return_code=%d\nerrno=%d\nmessage=%s\n", return_code, myerrno, error_str);
    exit(1);
  }
}


struct msg {
  long type;
  char text[SIZE];
};

int main(int argc, char *argv[]) {
  key_t key = ftok("./msgqueue.c", 0);
  int i;
  int retcode;

  /* create a second process so communication can be tested */
  int pid = fork();
  handle_error(pid, "fork failed");
  if (pid == 0) {
    printf("in child pid=%d (ppid=%d)\n", getpid(), getppid());
    struct msg msg;
    int id = msgget(key, IPC_CREAT);
    handle_error(id, "msgget failed");
    printf("child: id=%d key=%ld", id, (long) key);
    for (i = 0; i < 100; i++) {
      sprintf(msg.text, "C%d^2=%d", i, i*i);
      msg.type = 10;
      retcode = msgsnd(id, &msg, SIZE, 0);
      handle_error(retcode, "msgsnd failed");
      sleep(1);
    }
    sprintf(msg.text, "Q0=0");
    msgsnd(id, &msg, SIZE, 0);
    printf("terminating child\n");
    exit(0);

  } else {
    printf("in parent pod=%d (child_pid=%d)\n", getpid(), pid);
    struct msg msg;
    int id = msgget(key, IPC_CREAT);
    handle_error(id, "msgget failed");
    printf("parent: id=%d key=%ld\n", id, (long) key);
    while (TRUE) {
      retcode = msgrcv(id, &msg, SIZE, 10, 0);
      handle_error(retcode, "msgrcv failed");
      char c;
      int x;
      int y;
      sscanf(msg.text, "%c%d=%d", &c, &x, &y);
      if (c == 'Q') {
        break;
      }
      printf("%4d^2 = %6d\n", x, y);
    }
    printf("terminating parent\n");
    exit(0);
  }
}
