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
#include <signal.h>

#define MSGPERM 0600
#define SIZE 20

struct data {
  char c;
  int x;
  int y;
};

struct msg {
  long type;
  struct data data;
};

int main(int argc, char *argv[]) {
  int i, retcode;
  key_t key = ftok("./msgref.dat", 0);
  printf("key=%ld\n", (long) key);     /* error handling... */
  int pid = fork();
  printf("pid=%d\n", pid);     /* error handling... */
  if (pid == 0) {
    struct msg msg;
    int id = msgget(key, IPC_CREAT | MSGPERM);
    printf("id=%d\n", id);     /* error handling... */
    for (i = 0; i < 20; i++) {
      msg.data.c = 'C';
      msg.data.x = i;
      msg.data.y = i*i;
      msg.type = 10;
      retcode = msgsnd(id, &msg, SIZE, 0);
      printf("retcode=%d\n", retcode);     /* error handling... */
      sleep(1);
    }
    msg.data.c = 'Q';
    retcode = msgsnd(id, &msg, SIZE, 0);
    printf("retcode=%d\n", retcode);     /* error handling... */
    exit(0);
  } else {
    sleep(1);
    struct msg msg;
    int id = msgget(key, IPC_CREAT | MSGPERM);
    printf("id=%d\n", id);     /* error handling... */
    while (1) {
      retcode = msgrcv(id, &msg, SIZE, 10, 0);
      printf("retcode=%d\n", retcode);     /* error handling... */
      char c = msg.data.c;
      if (c == 'Q') { break; }
      int x = msg.data.x;
      int y = msg.data.y;
      printf("%4d^2 = %6d\n", x, y);
    }
    exit(0);
  }
}
