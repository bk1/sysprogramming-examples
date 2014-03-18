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

#include <itskylib.h>

#define TXT_SIZE 1024

#define ERROR_SIZE 16384

#define MSGPERM 0600

int msgqid_for_cleanup = 0;

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

  atexit(cleanup_queue);

  int retcode;
  struct msqid_ds msgctl_data;
  retcode = msgctl(id, IPC_STAT, &msgctl_data);
  handle_error(retcode, "msgctl failed", PROCESS_EXIT);
  struct ipc_perm perms = msgctl_data.msg_perm;
  printf("%s: key=%ld uid=%d gid=%d cuid=%d cgid=%d mode=%d seq=%d\n", txt, (long) perms.__key, (int) perms.uid, (int) perms.gid, (int) perms.cuid, (int) perms.cgid, (int) perms.mode, (int)perms.__seq);
  printf("%s: bytes=%ld qnum=%ld qbytes=%ld\n", txt, msgctl_data.__msg_cbytes, msgctl_data.msg_qnum, msgctl_data.msg_qbytes);
}

void usage(char *argv0) {
  printf("Usage:\n\n%s -r\n\n%s -s\n\n%s -c\n", argv0, argv0, argv0);
  exit(0);
}

struct data {
  char text[TXT_SIZE];
};

struct msg {
  long type;
  struct data data;
};

const int SIZE = sizeof(struct data);

void send(int id) {
  struct msg msg;
  int retcode;
  msg.type = 10;
  strncpy(msg.data.text, "Hello, world!\n", SIZE);
  printf("sending %s", msg.data.text);
  retcode = msgsnd(id, &msg, SIZE, 0);
  handle_error(retcode, "msgsnd failed", PROCESS_EXIT);
}

void receive(int id) {
  struct msg msg;
  int retcode;
  retcode = msgrcv(id, &msg, SIZE, 10, 0);
  handle_error(retcode, "parent msgrcv failed", PROCESS_EXIT);
  printf("received \"%s\"\n", msg.data.text);
}

int main(int argc, char *argv[]) {

  // int retcode;

  signal(SIGTERM, my_handler);
  // handle_error(retcode, "registration of sighandler for SIGTERM", PROCESS_EXIT);
  signal(SIGINT, my_handler);
  // handle_error(retcode, "registration of sighandler for SIGINT", PROCESS_EXIT);

  FILE *f = fopen("msgref.dat", "w");
  fwrite("X", 1, 1, f);
  fclose(f);

  key_t key = ftok("./msgref.dat", 0);
  if (key < 0) {
    handle_error(-1, "ftok failed", PROCESS_EXIT);
  }

  int id = msgget(key, IPC_CREAT | MSGPERM);
  msgqid_for_cleanup = id;
  handle_error(id, "msgget failed", PROCESS_EXIT);
  show_msg_ctl(id, "");

  if (argc < 2) {
    usage(argv[0]);
  }
  if (strlen(argv[1]) != 2) {
    usage(argv[0]);
  }
  if (argv[1][0] != '-') {
    usage(argv[0]);
  }
  switch (argv[1][1]) {
  case 'r' :
    receive(id);
    break;
  case 's' :
    send(id);
    break;
  case 'c' :
    cleanup_queue();
    break;
  default:
    usage(argv[0]);
    break;
  }

  exit(0);
}
