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
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#include <itskylib.h>

#define PERM 0600

int shmid_for_cleanup = 0;

int ready = FALSE;

struct data {
  int ppid;
  int cpid;
  int pready;
  int cready;
  long x;
  long y;
  char cmd;
};

const int SIZE = sizeof(struct data);

int create_shm(key_t key, const char *txt, const char *etxt) {
  int shm_id = shmget(key, SIZE, IPC_CREAT | PERM);
  handle_error(shm_id, etxt, PROCESS_EXIT);
  printf("%s: shm_id=%d key=%ld\n", txt, shm_id, (long) key);
  return shm_id;
}

void cleanup() {
  if (shmid_for_cleanup > 0) {
    int retcode = shmctl(shmid_for_cleanup, IPC_RMID, NULL);
    shmid_for_cleanup = 0;
    handle_error(retcode, "removing of shm failed", NO_EXIT);
  }
}


void my_handler(int signo) {
  if (signo == SIGUSR1) {
    ready = TRUE;
  } else {
    cleanup();
    exit(1);
  }
}


void show_shm_ctl(int shm_id, const char *txt) {

  int retcode;
  struct shmid_ds shmctl_data;
  retcode = shmctl(shm_id, IPC_STAT, &shmctl_data);
  handle_error(retcode, "child shmctl failed", PROCESS_EXIT);
  struct ipc_perm perms = shmctl_data.shm_perm;
  printf("%s: key=%ld uid=%d gid=%d cuid=%d cgid=%d mode=%d seq=%d\n", txt, (long) perms.__key, (int) perms.uid, (int) perms.gid, (int) perms.cuid, (int) perms.cgid, (int) perms.mode, (int)perms.__seq);
}

int main(int argc, char *argv[]) {
  int retcode = 0;
  int i;

  signal(SIGTERM, my_handler);
  signal(SIGINT, my_handler);
  signal(SIGUSR1, my_handler);

  sigset_t set, oldset, suspendset;
  sigemptyset(&set);
  sigaddset(&set, SIGUSR1);
  // sigprocmask(SIG_BLOCK, &set, &oldset);
  suspendset = oldset;
  sigdelset(&suspendset, SIGUSR1);

  FILE *f = fopen("shmref.dat", "w");
  fwrite("X", 1, 1, f);
  fclose(f);

  key_t shm_key = ftok("./shmref.dat", 1);
  if (shm_key < 0) {
    handle_error(-1, "ftok failed", PROCESS_EXIT);
  }
  
  /* create a worker process */
  int pid = fork();
  handle_error(pid, "fork failed", PROCESS_EXIT);
  if (pid == 0) {
    printf("in worker pid=%d (ppid=%d)\n", getpid(), getppid());

    int shm_id = create_shm(shm_key, "worker", "worker shmget failed");
    struct data *shm_data = (struct data *) shmat(shm_id, NULL, 0);
    printf("worker: address=%ld\n", (long) shm_data);
    shm_data->cpid = getpid();
    shm_data->cready = TRUE;
    for (i = 0; i < 5; i++) {
      printf("W: p=%d c=%d x=%ld y=%ld c=%c\n", shm_data->ppid, shm_data->cpid, shm_data->x, shm_data->y, shm_data->cmd);
      sleep(1);
    }
    // kill(SIGUSR1, getppid());
    while (TRUE) {
      while (! shm_data->pready) {
        sleep(1);
        //retcode = pause();
        // retcode = sigsuspend(&suspendset);
        printf("returned retcode=%d errno=%d\n", retcode, errno);
      }
      shm_data->cready = FALSE;
      ready = FALSE;
      if (shm_data->cmd == 'Q') {
        shmdt(shm_data);
        printf("terminating worker\n");
        exit(0);
      }
      shm_data->y = shm_data->x * shm_data->x;
      shm_data->cready = TRUE;
      printf("worker sending signal to parent\n");
      // kill(SIGUSR1, shm_data->ppid);
    }
  }
  
  /* in parent */
  atexit(cleanup);
  printf("in parent pid=%d\n", getpid());
  int shm_id = create_shm(shm_key, "worker", "worker shmget failed");
  struct data *shm_data = (struct data *) shmat(shm_id, NULL, 0);
  printf("parent: address=%ld\n", (long) shm_data);
  shm_data->ppid = getpid();
  shm_data->pready = FALSE;
  for (i = 0; i < 5; i++) {
    printf("P: p=%d c=%d x=%ld y=%ld c=%c\n", shm_data->ppid, shm_data->cpid, shm_data->x, shm_data->y, shm_data->cmd);
    sleep(1);
  }
  while (! ready) {
    sleep(1);
    // pause();
    // sigsuspend(&suspendset);
  }
  ready = FALSE;
  for (i = 0; i < 100; i++) {
    shm_data->x = i;
    shm_data->cmd = '*';
    shm_data->pready = TRUE;
    //printf("parent sending signal to worker\n");
    // kill(SIGUSR1, pid);
    while (! shm_data->cready) {
      // sigsuspend(&suspendset);
      // pause();
      sleep(1);
    }
    shm_data->pready = FALSE;
    ready = FALSE;
    printf("x=%3ld y=%6ld\n", shm_data->x, shm_data->y);
  }
  shm_data->cmd = 'Q';
  // kill(SIGUSR1, pid);
  int status;
  wait(&status);
  printf("child terminated->terminating parent\n");
  shmdt(shm_data);
  cleanup();
  printf("terminating parent\n");
  exit(0);
}
