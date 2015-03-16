/* (C) IT Sky Consulting GmbH 2015
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2015-03-16
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
#include <time.h>
#include <sys/stat.h>

#include <itskylib.h>

#define SIZE 20

#define MSGPERM 0600

int msgqid_for_cleanup = 0;

const char *REF_FILE = "./msgref.dat";

const int N = 2;

void cleanup_queue() {
  if (msgqid_for_cleanup > 0) {
    int retcode = msgctl(msgqid_for_cleanup, IPC_RMID, NULL);
    msgqid_for_cleanup = 0;
    handle_error(retcode, "removing of msgqueue failed", NO_EXIT);
  }
}

void my_handler(int signo) {
  cleanup_queue();
  exit(1);
}

void show_msg_ctl(int id, const char *txt) {

  int retcode;
  struct msqid_ds msgctl_data;
  retcode = msgctl(id, IPC_STAT, &msgctl_data);
  handle_error(retcode, "child msgctl failed", PROCESS_EXIT);
  struct ipc_perm perms = msgctl_data.msg_perm;
  printf("%s: key=%ld uid=%d gid=%d cuid=%d cgid=%d mode=%d seq=%d\n", txt, (long) perms.__key, (int) perms.uid, (int) perms.gid, (int) perms.cuid, (int) perms.cgid, (int) perms.mode, (int)perms.__seq);
  printf("%s: bytes=%ld qnum=%ld qbytes=%ld\n", txt, msgctl_data.__msg_cbytes, msgctl_data.msg_qnum, msgctl_data.msg_qbytes);
}


struct data {
  char c;
  int x;
  int y;
};

struct msg {
  long type;
  struct data data;
};

void parent(int id) {
  int retcode;
  struct msg msg;
  for (int i = 0; i < 100; i++) {
    msg.data.c = 'C';
    msg.data.x = i;
    msg.data.y = i*i;
    msg.type = i % 2 +1;
    retcode = msgsnd(id, &msg, SIZE, 0);
    handle_error(retcode, "msgsnd failed", PROCESS_EXIT);
    struct timespec duration;
    duration.tv_sec = (time_t) 0;
    duration.tv_nsec = 100000000L;
    nanosleep(&duration, NULL);
  }
  msg.data.c = 'Q';
  for (int i = 1; i <= N; i++) {
    msg.type   = i;
    msgsnd(id, &msg, SIZE, 0);
    printf("terminating child %d\n", i);
  }
  exit(0);
}

void child(int id, long type) {

  int retcode;

  struct msg msg;
  while (TRUE) {
    retcode = msgrcv(id, &msg, SIZE, type, 0);
    handle_error(retcode, "parent msgrcv failed", PROCESS_EXIT);
    char c = msg.data.c;
    if (c == 'Q') {
      break;
    }
    int x = msg.data.x;
    int y = msg.data.y;
    printf("child[%ld]: %4d^2 = %6d\n", type, x, y);
  }
  printf("terminating child %ld\n", type);
  exit(0);

}

int main(int argc, char *argv[]) {

  signal(SIGTERM, my_handler);
  signal(SIGINT, my_handler);

  create_if_missing(REF_FILE, S_IRUSR | S_IWUSR);

  key_t key = ftok(REF_FILE, 0);
  handle_error(key, "ftok failed", PROCESS_EXIT);

  int id = msgget(key, IPC_CREAT | MSGPERM);
  msgqid_for_cleanup = id;

  handle_error(id, "child msgget failed", PROCESS_EXIT);
  printf("id=%d key=%ld\n", id, (long) key);

  for (int i = 1; i <= N; i++) {
    /* create a child process so communication can be tested */
    int pid = fork();
    handle_error(pid, "fork failed", PROCESS_EXIT);
    if (pid == 0) {
      printf("in child pid=%d (ppid=%d)\n", getpid(), getppid());
      child(id, i);
    }
  }
  atexit(cleanup_queue);
  parent(id);
}
