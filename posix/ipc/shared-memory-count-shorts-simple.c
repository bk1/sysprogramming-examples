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
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <itskylib.h>

#define PERM 0600

#define BUF_SIZE 16384

#define ARRAY_SIZE 65536

const char *REF_FILE = "./shm_sem_ref.dat";

int shmid_for_cleanup = 0;
int semid_for_cleanup = 0;

struct data {
  long counter[ARRAY_SIZE];
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
  if (semid_for_cleanup > 0) {
    int retcode = semctl(semid_for_cleanup, 0, IPC_RMID, NULL);
    semid_for_cleanup = 0;
    handle_error(retcode, "removing of semaphore failed", NO_EXIT);
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

void show_sem_ctl(int semaphore_id, int idx, const char *txt) {

  int retcode;
  struct semid_ds semctl_data;
  // struct semun    semctl_extra;
  retcode = semctl(semaphore_id, idx, IPC_STAT, &semctl_data);
  handle_error(retcode, "child semctl failed", PROCESS_EXIT);
  int val = semctl(semaphore_id, idx, GETVAL, &semctl_data);
  handle_error(retcode, "child semctl failed", PROCESS_EXIT);
  struct ipc_perm perms = semctl_data.sem_perm;
  printf("%6d %s: t=%ld key=%ld idx=%d val=%d nsems=%d uid=%d gid=%d cuid=%d cgid=%d mode=%d seq=%d\n", (int) getpid(), txt, (long) time(NULL), (long) perms.__key, idx, (int) val, (int) semctl_data.sem_nsems, (int) perms.uid, (int) perms.gid, (int) perms.cuid, (int) perms.cgid, (int) perms.mode, (int)perms.__seq);
  fflush(stdout);
}

void usage(char *argv0, char *msg) {
  if (msg != NULL) {
    printf("%s\n\n", msg);
  }
  printf("Usage\n\n");
  printf("%s -c\ncleanup ipc\n\n", argv0);
  printf("%s -s\nsetup ipc\n\n", argv0);
  printf("%s < inputfile\ncout file, show accumulated output\n\n", argv0);
  printf("%s name < inputfile\ncout file, show output with name\n\n", argv0);
  exit(1);
}

int main(int argc, char *argv[]) {

  time_t t0 = time(NULL);

  if (argc > 2) {
    usage(argv[0], "wrong number of arguments");
  }

  if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-?") == 0)) {
    usage(argv[0], NULL);
  }

  int retcode = 0;

  create_if_missing(REF_FILE, S_IRUSR | S_IWUSR);

  key_t shm_key = ftok(REF_FILE, 1);
  if (shm_key < 0) {
    handle_error(-1, "ftok failed", PROCESS_EXIT);
  }

  key_t sem_key = ftok(REF_FILE, 2);
  if (sem_key < 0) {
    handle_error(-1, "ftok failed", PROCESS_EXIT);
  }

  if (argc == 2 && strcmp(argv[1], "-s") == 0) {
    printf("setting up IPC\n");
    int shm_id = create_shm(shm_key, "create", "shmget failed", IPC_CREAT);
    shmid_for_cleanup = shm_id;
    int semaphore_id = create_sem(sem_key, 1, "create", "semget failed", IPC_CREAT);
    semid_for_cleanup = semaphore_id;

    show_sem_ctl(semaphore_id, 0, "semaphore before setup");
    setup_sem(semaphore_id, "semaphore setup failed");
    show_sem_ctl(semaphore_id, 0, "semaphore after setup");
    printf("done\n");
    exit(0);
  }

  int shm_id = create_shm(shm_key, "create", "shmget failed", 0);
  shmid_for_cleanup = shm_id;

  int semaphore_id = create_sem(sem_key, 1, "create", "semget failed", 0);
  semid_for_cleanup = semaphore_id;

  if (argc == 2 && strcmp(argv[1], "-c") == 0) {
    printf("cleaning up IPC\n");
    cleanup();
    exit(0);
  }

  if (argc == 2 && argv[1][0] == '-') {
    char *msg = (char *) malloc(20+strlen(argv[1]));
    sprintf(msg, "unknown option %s", argv[1]);
    usage(argv[0], msg);
  }

  char *name = "";
  if (argc == 2) {
    name = argv[1];
  }

  struct data *shm_data = (struct data *) shmat(shm_id, NULL, 0);

  char buffer[BUF_SIZE];
  struct data local_data;
  long *counter = local_data.counter;
  for (int i = 0; i < ARRAY_SIZE; i++) {
    counter[i] = 0L;
  }

  // read shorts from stdin
  size_t pos = 0;
  while (TRUE) {
    ssize_t size_read = read(STDIN_FILENO, buffer + pos, BUF_SIZE - pos);
    handle_error(size_read, "error while reading stdin", PROCESS_EXIT);
    unsigned short *sarr = (unsigned short *) buffer;
    size_t byte_size = size_read + pos;
    if (size_read == 0 && pos > 0) {
      buffer[pos] = (char) 0;
      pos++;
    }
    size_t short_size = byte_size / 2;
    pos = byte_size % 2;
    for (unsigned int i = 0; i < short_size; i++) {
      unsigned short s = sarr[i];
      counter[s]++;
    }
    if (size_read == 0) {
      /* end of file */
      break;
    }
    if (pos > 0) {
      buffer[0] = buffer[byte_size - 1];
    }
  }

  unsigned int i;

  struct sembuf semops;
  semops.sem_num = 0;
  semops.sem_op  = -1;
  semops.sem_flg = SEM_UNDO;
  time_t t1 = time(NULL);
  // reserve semaphore
  retcode = semop(semaphore_id, &semops, 1);
  handle_error(retcode, "error while getting semaphore", PROCESS_EXIT);
  time_t dt = time(NULL) - t1;
  // show_sem_ctl(semaphore_id, 0, "semaphore reserved");

  long *tcounter = shm_data->counter;
  long *scounter = local_data.counter;
  for (int i = 0; i < ARRAY_SIZE; i++) {
    tcounter[i] += scounter[i];
  }

  time_t total = time(NULL) - t0;

  printf("------------------------------------------------------------\n");
  printf("%s: pid=%ld\n", name, (long) getpid());
  printf("total time for calculation: ~ %ld sec; total wait for semaphore: ~ %ld sec\n", (long) total, (long) dt);
  printf("------------------------------------------------------------\n");
  for (i = 0; i < ARRAY_SIZE; i++) {
    long *counter = shm_data->counter;
    long val = counter[i];

    if (! (i & 0x0f)) {
      printf("\n");
      fflush(stdout);
    }
    printf("%5d: %10ld    ", i, val);
  }
  printf("\n\n");
  printf("------------------------------------------------------------\n\n");
  fflush(stdout);

  semops.sem_num = 0;
  semops.sem_op  = 1;
  semops.sem_flg = SEM_UNDO;
  // show_sem_ctl(semaphore_id, 0, "freeing semaphore");
  // release semaphore
  retcode = semop(semaphore_id, &semops, 1);
  handle_error(retcode, "error while releasing semaphore", PROCESS_EXIT);
  // show_sem_ctl(semaphore_id, 0, "semaphore freed");

  retcode = shmdt(shm_data);
  handle_error(retcode, "error while detaching shared memory", PROCESS_EXIT);
  /* cleanup(); */
  printf("done\n");
  exit(0);
}
