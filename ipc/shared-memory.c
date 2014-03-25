/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/* WARNING: does not work currently */
/* start a worker thread.  Put assignments for worker thread into shared memory.  Extract result from there and print it */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
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

const char *REF_FILE = "./shmref.dat";

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
    printf("received SIGUSR1\n");
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

void parent(key_t shm_key, pid_t child_pid, sigset_t *suspendset_ptr) {
  int retcode;

  atexit(cleanup);
  printf("in parent child_pid=%d\n", getpid());
  int shm_id = create_shm(shm_key, "worker", "worker shmget failed");
  struct data *shm_data = (struct data *) shmat(shm_id, NULL, 0);
  printf("parent: address=%ld\n", (long) shm_data);
  shm_data->ppid = getpid();
  shm_data->pready = FALSE;
  for (int i = 0; i < 5; i++) {
    printf("P: p=%d c=%d x=%ld y=%ld c=%c\n", shm_data->ppid, shm_data->cpid, shm_data->x, shm_data->y, shm_data->cmd);
    sleep(1);
  }
  while (! ready) {
    retcode = sigsuspend(suspendset_ptr);
    handle_error(retcode, "parent: sigsuspend", PROCESS_EXIT);
  }
  ready = FALSE;
  for (int i = 0; i < 100; i++) {
    shm_data->x = i;
    shm_data->cmd = '*';
    shm_data->pready = TRUE;
    printf("P: x=%3ld y=%6ld\n", shm_data->x, shm_data->y);
    printf("parent sending signal to worker\n");
    retcode = kill(SIGUSR1, child_pid);
    handle_error(retcode, "parent: kill", PROCESS_EXIT);
    while (! shm_data->cready) {
      sigsuspend(suspendset_ptr);
    }
    shm_data->pready = FALSE;
    ready = FALSE;
    printf("P: x=%3ld y=%6ld\n", shm_data->x, shm_data->y);
  }
  shm_data->cmd = 'Q';
  retcode = kill(SIGUSR1, child_pid);
  handle_error(retcode, "child: kill", PROCESS_EXIT);
  int status;
  wait(&status);
  printf("child terminated->terminating parent\n");
  shmdt(shm_data);
  printf("terminating parent: DONE\n");
}

void child(key_t shm_key, sigset_t *suspendset_ptr) {
  int retcode;

  printf("in worker pid=%d (ppid=%d)\n", getpid(), getppid());

  int shm_id = create_shm(shm_key, "worker", "worker shmget failed");
  struct data *shm_data = (struct data *) shmat(shm_id, NULL, 0);
  printf("worker: address=%ld\n", (long) shm_data);
  shm_data->cpid = getpid();
  shm_data->cready = TRUE;
  for (int i = 0; i < 5; i++) {
    printf("W: p=%d c=%d x=%ld y=%ld c=%c\n", shm_data->ppid, shm_data->cpid, shm_data->x, shm_data->y, shm_data->cmd);
    sleep(1);
  }
  pid_t ppid = getppid();
  printf("child sending signal to ppid=%ld indicate readiness\n", (long) ppid);
  kill(SIGUSR1, ppid);
  while (TRUE) {
    while (! shm_data->pready) {
      // retcode = pause();
      // handle_error(retcode, "pause", PROCESS_EXIT);
      retcode = sigsuspend(suspendset_ptr);
      printf("returned retcode=%d errno=%d\n", retcode, errno);
    }
    shm_data->cready = FALSE;
    ready = FALSE;
    if (shm_data->cmd == 'Q') {
      shmdt(shm_data);
      printf("terminating worker: DONE\n");
      exit(0);
    }
    shm_data->y = shm_data->x * shm_data->x;
    shm_data->cready = TRUE;
    printf("worker sending signal to parent\n");
    kill(SIGUSR1, shm_data->ppid);
  }
}

void usage(char *argv0, char *msg) {
  printf("%s\n\n", msg);
  printf("Usage:\n\n%s\n use shared memory to communcate between two processes\n\n", argv0);
  exit(1);
}

int main(int argc, char *argv[]) {

  int retcode = 0;

  if (is_help_requested(argc, argv)) {
    usage(argv[0], "");
  }

  signal(SIGTERM, my_handler);
  signal(SIGINT, my_handler);
  signal(SIGUSR1, my_handler);

  sigset_t set, oldset, suspendset;
  retcode = sigemptyset(&set);
  handle_error(retcode, "main: sigemptyset", PROCESS_EXIT);
  retcode = sigaddset(&set, SIGUSR1);
  handle_error(retcode, "main: sigaddset", PROCESS_EXIT);
  retcode = sigprocmask(SIG_BLOCK, &set, &oldset);
  handle_error(retcode, "main: sigprocmask", PROCESS_EXIT);
  suspendset = oldset;
  sigset_t *suspendset_ptr = &suspendset;
  retcode = sigdelset(suspendset_ptr, SIGUSR1);
  handle_error(retcode, "main: sigdelset", PROCESS_EXIT);

  create_if_missing(REF_FILE, S_IRUSR | S_IWUSR);

  key_t shm_key = ftok(REF_FILE, 1);
  if (shm_key < 0) {
    handle_error(-1, "ftok failed", PROCESS_EXIT);
  }

  /* create a worker process */
  pid_t child_pid = fork();
  handle_error(child_pid, "fork failed", PROCESS_EXIT);
  if (child_pid == 0) {
    child(shm_key, suspendset_ptr);
    exit(0); // done with child
  }

  /* in parent */
  parent(shm_key, child_pid, suspendset_ptr);
  exit(0);
}
