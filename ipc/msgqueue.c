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

#define FALSE 0
#define TRUE  1
#define SIZE 20

#define ERROR_SIZE 16384

#define MSGPERM 0600

int msgqid_for_cleanup = 0;

void handle_error(int return_code, const char *msg);

void cleanup_queue() {
  if (msgqid_for_cleanup > 0) {
    int retcode = msgctl(msgqid_for_cleanup, IPC_RMID, NULL);
    msgqid_for_cleanup = 0;
    handle_error(retcode, "removing of msgqueue failed");
  }
}


void my_handler(int signo) {
  cleanup_queue();
  exit(1);
}

/* helper function for dealing with errors */
void handle_error(int return_code, const char *msg) {
  if (return_code < 0) {
    char extra_msg[ERROR_SIZE];
    char error_msg[ERROR_SIZE];
    int myerrno = errno;
    const char *error_str = strerror(myerrno);
    if (msg != NULL) {
      sprintf(extra_msg, "%s\n", msg);
    } else {
      extra_msg[0] = '\000';
    }
    sprintf(error_msg, "%sreturn_code=%d\nerrno=%d\nmessage=%s\n", extra_msg, return_code, myerrno, error_str);
    write(STDOUT_FILENO, error_msg, strlen(error_msg));
    cleanup_queue();
    exit(1);
  }
}


void show_msg_ctl(int id, const char *txt) {

  int retcode;
  struct msqid_ds msgctl_data;
  retcode = msgctl(id, IPC_STAT, &msgctl_data);
  handle_error(retcode, "child msgctl failed");
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

int main(int argc, char *argv[]) {
  int i;
  int retcode;

  signal(SIGTERM, my_handler);
  signal(SIGINT, my_handler);

  FILE *f = fopen("msgref.dat", "w");
  fwrite("X", 1, 1, f);
  fclose(f);

  key_t key = ftok("./msgref.dat", 0);
  if (key < 0) {
    handle_error(-1, "ftok failed");
  }

  /* create a second process so communication can be tested */
  int pid = fork();
  handle_error(pid, "fork failed");
  if (pid == 0) {
    printf("in child pid=%d (ppid=%d)\n", getpid(), getppid());
    struct msg msg;

    int id = msgget(key, IPC_CREAT | MSGPERM);
    handle_error(id, "child msgget failed");

    printf("child: id=%d key=%ld\n", id, (long) key);

    show_msg_ctl(id, "child");

    msg.type = 10;

    for (i = 0; i < 100; i++) {
      msg.data.c = 'C';
      msg.data.x = i;
      msg.data.y = i*i;
      retcode = msgsnd(id, &msg, SIZE, 0);
      handle_error(retcode, "msgsnd failed");
      sleep(1);
    }

    msg.data.c = 'Q';
    retcode = msgsnd(id, &msg, SIZE, 0);
    handle_error(retcode, "msgsnd failed");
    printf("terminating child\n");
    exit(0);

  } else {
    printf("in parent pod=%d (child_pid=%d)\n", getpid(), pid);
    sleep(1);
    printf("reading in parent\n");

    struct msg msg;
    int id = msgget(key, IPC_CREAT | MSGPERM);
    msgqid_for_cleanup = id;
    handle_error(id, "parent msgget failed");

    printf("parent: id=%d key=%ld\n", id, (long) key);

    show_msg_ctl(id, "child");

    while (TRUE) {
      retcode = msgrcv(id, &msg, SIZE, 10, 0);
      handle_error(retcode, "parent msgrcv failed");
      char c = msg.data.c;
      if (c == 'Q') {
        break;
      }
      int x = msg.data.x;
      int y = msg.data.y;
      printf("%4d^2 = %6d\n", x, y);
    }
    printf("terminating parent\n");
    cleanup_queue();
    exit(0);
  }
}
