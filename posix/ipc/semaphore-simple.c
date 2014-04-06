/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <itskylib.h>

#define ERROR_SIZE 16384

#define MSGPERM 0600

const char *REF_FILE = "./semref.dat";

int create_sem(key_t key) {
  int semaphore_id = semget(key, 1, IPC_CREAT | MSGPERM);
  handle_error(semaphore_id, "failed with semget", PROCESS_EXIT);
  printf("semaphore_id=%d key=%ld\n", semaphore_id, (long) key);
  return semaphore_id;
}

int main(int argc, char *argv[]) {
  int retcode;
  struct sembuf sem;

  create_if_missing(REF_FILE, S_IRUSR | S_IWUSR);

  key_t sem_key = ftok(REF_FILE, 1);
  handle_error(sem_key, "ftok failed", PROCESS_EXIT);

  int semaphore_id = create_sem(sem_key);
  short semval[1];
  semval[0] = (short) 5;
  retcode = semctl(semaphore_id, 1, SETALL, &semval);
  handle_error(retcode, "changing of semaphore failed", PROCESS_EXIT);

  int j;
  for (j = 0; j < 50; j++) {

    sem.sem_num = 0;
    sem.sem_op  = -1;
    sem.sem_flg = SEM_UNDO;
    printf("%d obtaining resource\n", j);
    retcode = semop(semaphore_id, &sem, 1);
    handle_error(retcode, "semop failed", PROCESS_EXIT);
  }
  exit(0);
}
