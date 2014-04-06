/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/* shows that semaphore stored in shared memory does not work for IPC */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>

#include <itskylib.h>

#define PERM 0600

#define BUF_SIZE 16384

#define ALPHA_SIZE 256

int semid_for_cleanup = 0;

int create_sem(key_t key, const int sem_size, const char *txt, const char *etxt, int flags) {
  int semaphore_id = semget(key, sem_size, flags | PERM);
  handle_error(semaphore_id, etxt, PROCESS_EXIT);
  return semaphore_id;
}

int setup_sem(int semaphore_id, char *etxt) {
  short semval[1] = { 1 };
  int retcode = semctl(semaphore_id, 1, SETALL, &semval);
  handle_error(retcode, etxt, PROCESS_EXIT);
  return retcode;
}

void cleanup() {
  if (semid_for_cleanup > 0) {
    int retcode = semctl(semid_for_cleanup, 0, IPC_RMID, NULL);
    semid_for_cleanup = 0;
    handle_error(retcode, "removing of semaphore failed", NO_EXIT);
  }
}

int main(int argc, char *argv[]) {

  int retcode;

  key_t sem_key = ftok(argv[0], 2);
  if (sem_key < 0) {
    handle_error(-1, "ftok failed", PROCESS_EXIT);
  }

  struct sembuf semops_lock;
  semops_lock.sem_num = 0;
  semops_lock.sem_op  = -1;
  semops_lock.sem_flg = SEM_UNDO;

  struct sembuf semops_unlock;
  semops_unlock.sem_num = 0;
  semops_unlock.sem_op  = 1;
  semops_unlock.sem_flg = SEM_UNDO;

  pid_t pid = fork();
  if (pid == 0) {
    printf("in child: sleeping\n");
    sleep(2);
    int semaphore_id = create_sem(sem_key, 1, "create", "semget failed", IPC_CREAT);

    printf("in child: getting semaphore\n");
    retcode = semop(semaphore_id, &semops_lock, 1);
    handle_error(retcode, "child failed lock", PROCESS_EXIT);
    printf("child got semaphore\n");
    sleep(5);
    printf("child releases semaphore\n");
    retcode = semop(semaphore_id, &semops_unlock, 1);
    handle_error(retcode, "child failed unlock", PROCESS_EXIT);
    printf("child released semaphore\n");
    sleep(10);
    handle_error(retcode, "child: error while detaching shared memory", PROCESS_EXIT);
    printf("child exiting\n");
    exit(0);
  } else {
    printf("in parent: setting up semaphore\n");
    int semaphore_id = create_sem(sem_key, 1, "create", "semget failed", IPC_CREAT);
    semid_for_cleanup = semaphore_id;
    setup_sem(semaphore_id, "semaphore setup failed");
    sleep(2);

    printf("in parent: getting semaphore\n");
    retcode = semop(semaphore_id, &semops_lock, 1);
    handle_error(retcode, "child failed lock", PROCESS_EXIT);
    printf("parent got semaphore\n");
    sleep(5);
    printf("parent releases semaphore\n");
    retcode = semop(semaphore_id, &semops_unlock, 1);
    handle_error(retcode, "child failed unlock", PROCESS_EXIT);
    printf("parent released semaphore\n");
    printf("parent waiting for child to terminate\n");
    int status;
    waitpid(pid, &status, 0);
    handle_error(retcode, "parent: error while detaching shared memory", PROCESS_EXIT);
    cleanup();
    printf("done\n");
    exit(0);
  }
}
