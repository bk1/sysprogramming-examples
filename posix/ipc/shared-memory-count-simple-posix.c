/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <itskylib.h>

#define PERM 0600

#define BUF_SIZE 16384

#define ALPHA_SIZE 256

#define SEM_NAME "/sem-shared-memory-count-simple-posix"

const char *REF_FILE = "./shm_sem_ref.dat";

struct data {
  long counter[ALPHA_SIZE];
};

const int SIZE = sizeof(struct data);

int create_shm(key_t key, const char *txt, const char *etxt, int flags) {
  int shm_id = shmget(key, SIZE, flags | PERM);
  handle_error(shm_id, etxt, PROCESS_EXIT);
  return shm_id;
}

sem_t *create_sem(const int value, const char *etxt, int flags) {
  sem_t *sem_ptr = sem_open(SEM_NAME, flags, PERM, value);
  handle_ptr_error(sem_ptr, etxt, PROCESS_EXIT);
  return sem_ptr;
}

void cleanup(int shmid, sem_t *sem_ptr) {
  int retcode;
  retcode = shmctl(shmid, IPC_RMID, NULL);
  handle_error(retcode, "removing of shm failed", NO_EXIT);
  retcode = sem_close(sem_ptr);
  handle_error(retcode, "closing of semaphore failed", NO_EXIT);
  retcode = sem_unlink(SEM_NAME);
  handle_error(retcode, "removing of semaphore failed", NO_EXIT);
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

  time_t t0 = time(NULL);

  if (argc > 2) {
    printf("Usage\n\n");
    printf("%s -c\ncleanup ipc\n\n", argv[0]);
    printf("%s -s\nsetup ipc\n\n", argv[0]);
    printf("%s < inputfile\ncout file, show accumulated output\n\n", argv[0]);
    printf("%s name < inputfile\ncout file, show output with name\n\n", argv[0]);
    exit(1);
  }

  int retcode = 0;


  create_if_missing(REF_FILE, S_IRUSR | S_IWUSR);

  key_t shm_key = ftok(REF_FILE, 1);
  if (shm_key < 0) {
    handle_error(-1, "ftok failed", PROCESS_EXIT);
  }

  if (argc == 2 && strcmp(argv[1], "-s") == 0) {
    printf("setting up IPC\n");
    create_shm(shm_key, "create", "shmget failed", IPC_CREAT | IPC_EXCL);
    create_sem(1, "sem_open failed", O_CREAT | O_EXCL);
    printf("done\n");
    exit(0);
  }

  int shm_id = create_shm(shm_key, "create", "shmget failed", 0);

  sem_t *sem_ptr = create_sem(1, "semget failed", 0);

  if (argc == 2 && strcmp(argv[1], "-c") == 0) {
    printf("cleaning up IPC\n");
    cleanup(shm_id, sem_ptr);
    exit(0);
  }

  char *name = "";
  if (argc == 2) {
    name = argv[1];
  }

  struct data *shm_data = (struct data *) shmat(shm_id, NULL, 0);

  char buffer[BUF_SIZE];
  struct data local_data;
  long *counter = local_data.counter;
  for (int i = 0; i < ALPHA_SIZE; i++) {
    counter[i] = 0L;
  }

  while (TRUE) {
    ssize_t size_read = read(STDIN_FILENO, buffer, BUF_SIZE);
    if (size_read == 0) {
      /* end of file */
      break;
    }
    handle_error(size_read, "error while reading stdin", PROCESS_EXIT);
    int i;
    for (i = 0; i < size_read; i++) {
      unsigned char c = buffer[i];
      counter[c]++;
    }
  }

  unsigned int i;

  time_t t1 = time(NULL);
  retcode = sem_wait(sem_ptr);
  handle_error(retcode, "error while getting semaphore", PROCESS_EXIT);
  time_t dt = time(NULL) - t1;

  for (int i = 0; i < ALPHA_SIZE; i++) {
    long *tcounter = shm_data->counter;
    long *scounter = local_data.counter;
    tcounter[i] += scounter[i];
  }

  time_t total = time(NULL) - t0;

  printf("------------------------------------------------------------\n");
  printf("%s: pid=%ld\n", name, (long) getpid());
  printf("total time for calculation: ~ %ld sec; total wait for semaphore: ~ %ld sec\n", (long) total, (long) dt);
  printf("------------------------------------------------------------\n");
  for (i = 0; i < ALPHA_SIZE; i++) {
    long *counter = shm_data->counter;
    long val = counter[i];

    if (! (i & 007)) {
      printf("\n");
      fflush(stdout);
    }
    if ((i & 0177) < 32 || i == 127) {
      printf("\\%03o: %10ld    ", i, val);
    } else {
      printf("%4c: %10ld    ", (char) i, val);
    }
  }
  printf("\n\n");
  printf("------------------------------------------------------------\n\n");
  fflush(stdout);

  retcode = sem_post(sem_ptr);
  handle_error(retcode, "error while releasing semaphore", PROCESS_EXIT);

  retcode = sem_close(sem_ptr);
  handle_error(retcode, "error while closing semaphore", PROCESS_EXIT);

  retcode = shmdt(shm_data);
  handle_error(retcode, "error while detaching shared memory", PROCESS_EXIT);
  /* cleanup(); */
  printf("done\n");
  exit(0);
}
