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
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define FALSE 0
#define TRUE  1

#define ERROR_SIZE 16384

#define MSGPERM 0600

#define WORKER_TYPE 1000

int msgqid_for_cleanup = 0;
int sempid_for_cleanup = 0;

void handle_error(int return_code, const char *msg);

int create_sem(key_t key, const char *txt, const char *etxt) {
  int semaphore_id = semget(key, 1, IPC_CREAT | MSGPERM);
  handle_error(semaphore_id, etxt);
  printf("%s: semaphore_id=%d key=%ld\n", txt, semaphore_id, (long) key);
  fflush(stdout);
  return semaphore_id;
}

/* helper function for dealing with errors */
void handle_error(int return_code, const char *msg) {
  if (return_code < 0) {
    char extra_txt[ERROR_SIZE];
    char error_msg[ERROR_SIZE];
    char *extra_msg = extra_txt;
    int myerrno = errno;
    const char *error_str = strerror(myerrno);
    if (msg != NULL) {
      sprintf(extra_msg, "%s\n", msg);
    } else {
      extra_msg = "";
    }
    sprintf(error_msg, "%sreturn_code=%d\nerrno=%d\nmessage=%s\n", extra_msg, return_code, myerrno, error_str);
    write(STDOUT_FILENO, error_msg, strlen(error_msg));
    exit(1);
  }
}


void show_sem_ctl(int semaphore_id, const char *txt) {

  int retcode;
  struct semid_ds semctl_data;
  retcode = semctl(semaphore_id, 0, IPC_STAT, &semctl_data);
  handle_error(retcode, "child semctl failed");
  struct ipc_perm perms = semctl_data.sem_perm;
  printf("%s: key=%ld uid=%d gid=%d cuid=%d cgid=%d mode=%d seq=%d\n", txt, (long) perms.__key, (int) perms.uid, (int) perms.gid, (int) perms.cuid, (int) perms.cgid, (int) perms.mode, (int)perms.__seq);
  fflush(stdout);
}

int main(int argc, char *argv[]) {
  int i;
  int retcode;
  struct sembuf sem;

  FILE *f = fopen("semref.dat", "w");
  fwrite("X", 1, 1, f);
  fclose(f);

  key_t msg_key = ftok("./semref.dat", 0);
  if (msg_key < 0) {
    handle_error(-1, "ftok failed");
  }
  key_t sem_key = ftok("./semref.dat", 1);
  if (sem_key < 0) {
    handle_error(-1, "ftok failed");
  }

  int semaphore_id = create_sem(sem_key, "parent", "parent semget failed");
  short semval[1];
  semval[0] = (short) 5;
  retcode = semctl(semaphore_id, 1, SETALL, &semval);
  handle_error(retcode, "changing of semaphore failed");

  int j;
  for (j = 0; j < 50; j++) {

    sem.sem_num = 0;
    sem.sem_op  = -1;
    sem.sem_flg = SEM_UNDO;
    printf("%d obtaining resource\n", j);
    fflush(stdout);
    retcode = semop(semaphore_id, &sem, 1);
    handle_error(retcode, "semop failed");
  }
  exit(0);
}
