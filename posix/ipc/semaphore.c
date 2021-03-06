/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <itskylib.h>

#define ERROR_SIZE 16384

#define MSGPERM 0600

#define WORKER_TYPE 1000

int msgqid_for_cleanup = 0;
int sempid_for_cleanup = 0;

const char *REF_FILE = "./semref.dat";

int create_sem(key_t key, const char *txt, const char *etxt) {
  int semaphore_id = semget(key, 1, IPC_CREAT | MSGPERM);
  handle_error(semaphore_id, etxt, PROCESS_EXIT);
  printf("%s: semaphore_id=%d key=%ld\n", txt, semaphore_id, (long) key);
  fflush(stdout);
  return semaphore_id;
}

int create_msg(key_t key, const char *txt, const char *etxt) {
  int msgqueue_id = msgget(key, IPC_CREAT | MSGPERM);
  handle_error(msgqueue_id, "child msgget failed", PROCESS_EXIT);
  return msgqueue_id;
}

void cleanup_queue() {
  if (msgqid_for_cleanup > 0) {
    int retcode = msgctl(msgqid_for_cleanup, IPC_RMID, NULL);
    msgqid_for_cleanup = 0;
    handle_error(retcode, "removing of msgqueue failed", PROCESS_EXIT);
  }
  if (sempid_for_cleanup > 0) {
    int retcode = semctl(sempid_for_cleanup, 0, IPC_RMID, NULL);
    sempid_for_cleanup = 0;
    handle_error(retcode, "removing of semaphore failed", PROCESS_EXIT);
  }
}


void my_handler(int signo) {
  cleanup_queue();
  exit(1);
}


void show_sem_ctl(int semaphore_id, const char *txt) {

  int retcode;
  struct semid_ds semctl_data;
  retcode = semctl(semaphore_id, 0, IPC_STAT, &semctl_data);
  handle_error(retcode, "child semctl failed",  PROCESS_EXIT);
  struct ipc_perm perms = semctl_data.sem_perm;
  printf("%s: key=%ld uid=%d gid=%d cuid=%d cgid=%d mode=%d seq=%d\n", txt, (long) perms.__key, (int) perms.uid, (int) perms.gid, (int) perms.cuid, (int) perms.cgid, (int) perms.mode, (int)perms.__seq);
  fflush(stdout);
}

struct data {
  char ctrl;
  int origin;
  long value;
};

struct msg {
  long type;
  struct data data;
};

union semun {
  int              val;    /* Value for SETVAL */
  struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
  unsigned short  array[1];  /* Array for GETALL, SETALL */
  struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};

void prepare_sem(key_t sem_key) {
  int retcode;
  int tmp_semaphore_id = create_sem(sem_key, "parent", "parent semget failed");
  short semval[1];
  semval[0] = (short) 2;
  retcode = semctl(tmp_semaphore_id, 1, SETALL, &semval);
  handle_error(retcode, "changing of semaphore failed", PROCESS_EXIT);
  union semun semunx;
  semctl(tmp_semaphore_id, 1, GETALL, &semunx);
  printf("limit=%d\n", semunx.array[0]);
  fflush(stdout);
}

const int SIZE = sizeof(struct data);

int main(int argc, char *argv[]) {
  int i;
  int retcode;
  struct msg msg;
  struct sembuf sem;

  signal(SIGTERM, my_handler);
  signal(SIGINT, my_handler);

  create_if_missing(REF_FILE, S_IRUSR | S_IWUSR);

  key_t msg_key = ftok(REF_FILE, 0);
  if (msg_key < 0) {
    handle_error(-1, "ftok failed", PROCESS_EXIT);
  }
  key_t sem_key = ftok(REF_FILE, 1);
  if (sem_key < 0) {
    handle_error(-1, "ftok failed", PROCESS_EXIT);
  }

  prepare_sem(sem_key);

  /* create a worker process */
  int pid = fork();
  handle_error(pid, "fork failed", PROCESS_EXIT);
  if (pid == 0) {
    printf("in worker pid=%d (ppid=%d)\n", getpid(), getppid());
    fflush(stdout);

    int msgqueue_id = create_msg(msg_key, "worker", "worker msgget failed");

    while (TRUE) {
      retcode = msgrcv(msgqueue_id, &msg, SIZE, WORKER_TYPE, 0);
      handle_error(retcode, "parent msgrcv failed", PROCESS_EXIT);
      char c = msg.data.ctrl;
      if (c == 'Q') {
        break;
      }
      long x = msg.data.value;
      long y = x*x;
      printf("worker: x=%ld y=%ld (o=%d c=%c)\n", x, y, msg.data.origin, msg.data.ctrl);
      fflush(stdout);

      msg.type = msg.data.origin;
      msg.data.value  = y;
      msg.data.origin = WORKER_TYPE;
      msg.data.ctrl   = 'Y';
      sleep(1);
      printf("worker sending: y=%ld (o=%d c=%c t=%ld)\n", y, msg.data.origin, msg.data.ctrl, msg.type);
      fflush(stdout);
      retcode = msgsnd(msgqueue_id, &msg, SIZE, 0);
      handle_error(retcode, "msgsnd failed", PROCESS_EXIT);
    }
    printf("terminating worker\n");
    fflush(stdout);
    exit(0);
  }

  sleep(1);

  for (i = 1; i <= 5; i++) {
    int pid = fork();
    handle_error(pid, "fork failed", PROCESS_EXIT);
    if (pid == 0) {
      printf("in child %d\n", i);
      fflush(stdout);

      int msgqueue_id = create_msg(msg_key, "child", "child msgget failed");

      int semaphore_id = create_sem(sem_key, "child", "child semget failed");

      show_sem_ctl(semaphore_id, "child");

      int j;

      for (j = 0; j < 15; j++) {

        sem.sem_num = 0;
        sem.sem_op  = -1;
        sem.sem_flg = SEM_UNDO;
        printf("%d obtaining resource\n", i);
        fflush(stdout);
        retcode = semop(semaphore_id, &sem, 1);
        handle_error(retcode, "semop failed", PROCESS_EXIT);

        printf("%d: sending msg x=%d\n", i, j);
        fflush(stdout);
        msg.data.ctrl = 'X';
        msg.data.value = j;
        msg.data.origin = i;
        msg.type = WORKER_TYPE;
        retcode = msgsnd(msgqueue_id, &msg, SIZE, 0);
        handle_error(retcode, "msgsnd failed", PROCESS_EXIT);

        retcode = msgrcv(msgqueue_id, &msg, SIZE, i, 0);
        handle_error(retcode, "msgrcv failed", PROCESS_EXIT);

        sem.sem_num = 0;
        sem.sem_op  = 1;
        sem.sem_flg = SEM_UNDO;
        printf("%d freeing resource\n", i);
        fflush(stdout);
        retcode = semop(semaphore_id, &sem, 1);
        handle_error(retcode, "semop failed", PROCESS_EXIT);

        printf("%3d: %3d^2 = %6ld\n", i, j, msg.data.value);
        fflush(stdout);
      }
      printf("done with child %d\n", i);
      fflush(stdout);
      exit(0);
    }
  }

  int msgqueue_id = create_msg(msg_key, "parent", "parent msgget failed");
  msgqid_for_cleanup = msgqueue_id;

  int semaphore_id = create_sem(sem_key, "parent", "parent semget failed");
  sempid_for_cleanup = semaphore_id;

  for (i = 1; i <= 5; i++) {
    printf("waiting for child i=%d\n", i);
    fflush(stdout);
    int status;
    retcode = wait(&status);
    handle_error(retcode, "wait failed", PROCESS_EXIT);
  }
  msg.data.ctrl = 'Q';
  msg.type = WORKER_TYPE;

  retcode = msgsnd(msgqueue_id, &msg, SIZE, 0);
  handle_error(retcode, "msgsnd failed", PROCESS_EXIT);

  printf("terminating parent\n");
  exit(0);
}
