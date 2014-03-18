/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/* use mutex for IPC, but do preparation separately */

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

#define REF_FILE "./shm_sem_ref.dat"

int shmid_for_cleanup = 0;

struct data {
  pthread_mutex_t mutex;
};

const int SIZE = sizeof(struct data);

int create_shm(key_t key, const char *txt, const char *etxt, int flags) {
  int shm_id = shmget(key, SIZE, flags | PERM);
  handle_error(shm_id, etxt, PROCESS_EXIT);
  return shm_id;
}

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
  if (shmid_for_cleanup > 0) {
    int retcode = shmctl(shmid_for_cleanup, IPC_RMID, NULL);
    shmid_for_cleanup = 0;
    handle_error(retcode, "removing of shm failed", NO_EXIT);
  }
}

int main(int argc, char *argv[]) {

  int retcode;

  key_t shm_key = ftok(REF_FILE, 1);
  if (shm_key < 0) {
    handle_error(-1, "ftok failed", PROCESS_EXIT);
  }

  struct timespec timeout;
  timeout.tv_sec  = (time_t) 20;
  timeout.tv_nsec = (long) 0;

  pid_t pid = fork();
  if (pid == 0) {
    printf("in child: sleeping\n");
    sleep(2);
    int shm_id = create_shm(shm_key, "create", "shmget failed", 0);
    struct data *data = (struct data *) shmat(shm_id, NULL, 0);
    printf("in child: getting mutex\n");
    retcode = pthread_mutex_timedlock(&(data->mutex), &timeout);
    handle_thread_error(retcode, "child failed timedlock", PROCESS_EXIT);
    printf("child got mutex\n");
    sleep(5);
    printf("child releases mutex\n");
    pthread_mutex_unlock(&(data->mutex));
    printf("child released mutex\n");
    sleep(10);
    retcode = shmdt(data);
    handle_error(retcode, "child: error while detaching shared memory", PROCESS_EXIT);
    printf("child exiting\n");
    exit(0);
  } else {
    printf("in parent: setting up\n");
    int shm_id = create_shm(shm_key, "create", "shmget failed", IPC_CREAT);
    shmid_for_cleanup = shm_id;

    struct data *data = (struct data *) shmat(shm_id, NULL, 0);
    pthread_mutex_init(&(data->mutex), NULL);
    sleep(2);
    printf("in parent: getting mutex\n");
    retcode = pthread_mutex_timedlock(&(data->mutex), &timeout);
    handle_thread_error(retcode, "parent failed timedlock", PROCESS_EXIT);
    printf("parent got mutex\n");
    sleep(5);
    printf("parent releases mutex\n");
    pthread_mutex_unlock(&(data->mutex));
    printf("parent released mutex\n");
    printf("parent waiting for child to terminate\n");
    int status;
    waitpid(pid, &status, 0);
    pthread_mutex_destroy(&(data->mutex));
    retcode = shmdt(data);
    handle_error(retcode, "parent: error while detaching shared memory", PROCESS_EXIT);
    cleanup();
    printf("done\n");
    exit(0);
  }
}
