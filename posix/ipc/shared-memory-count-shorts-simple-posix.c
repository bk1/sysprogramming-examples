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

const char *SEM_NAME = "/shared-memory-count-shorts-simple-posix";
const char *SEM_NAME_OUT = "/shared-memory-count-shorts-simple-posix:out";

int shmid_for_cleanup = 0;

struct data {
  long counter[ARRAY_SIZE];
};

const int SIZE = sizeof(struct data);

int create_shm(key_t key, const char *txt, const char *etxt, int flags) {
  int shm_id = shmget(key, SIZE, flags | PERM);
  handle_error(shm_id, etxt, PROCESS_EXIT);
  return shm_id;
}

sem_t *create_sem(const char *sem_name, const int value, const char *etxt, int flags) {
  sem_t *sem_ptr = sem_open(sem_name, flags, PERM, value);
  handle_ptr_error(sem_ptr, etxt, PROCESS_EXIT);
  return sem_ptr;
}

void cleanup(int shmid, const char *sem_name1, sem_t *sem_ptr1, const char *sem_name2, sem_t *sem_ptr2) {
  int retcode;
  retcode = shmctl(shmid, IPC_RMID, NULL);
  handle_error(retcode, "removing of shm failed", NO_EXIT);
  retcode = sem_close(sem_ptr1);
  handle_error(retcode, "closing of semaphore failed", NO_EXIT);
  retcode = sem_close(sem_ptr2);
  handle_error(retcode, "closing of semaphore failed", NO_EXIT);
  retcode = sem_unlink(sem_name1);
  handle_error(retcode, "removing of semaphore 1 failed", NO_EXIT);
  retcode = sem_unlink(sem_name2);
  handle_error(retcode, "removing of semaphore 2 failed", NO_EXIT);
}

void show_shm_ctl(int shm_id, const char *txt) {

  int retcode;
  struct shmid_ds shmctl_data;
  retcode = shmctl(shm_id, IPC_STAT, &shmctl_data);
  handle_error(retcode, "child shmctl failed", PROCESS_EXIT);
  struct ipc_perm perms = shmctl_data.shm_perm;
  printf("%s: key=%ld uid=%d gid=%d cuid=%d cgid=%d mode=%d seq=%d\n", txt, (long) perms.__key, (int) perms.uid, (int) perms.gid, (int) perms.cuid, (int) perms.cgid, (int) perms.mode, (int)perms.__seq);
}

void usage(char *argv0, char *msg) {
  if (msg != NULL) {
    printf("%s\n\n", msg);
  }
  printf("Usage\n\n");
  printf("%s -c\ncleanup ipc\n\n", argv0);
  printf("%s -s\nsetup ipc\n\n", argv0);
  printf("%s -n\nsetup ipc nonexclusively\n\n", argv0);
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

  int shm_id;

  sem_t *sem_ptr;
  sem_t *sem_out_ptr;

  if (argc == 2 && strcmp(argv[1], "-s") == 0) {
    printf("setting up IPC\n");
    shm_id = create_shm(shm_key, "create", "shmget failed", IPC_CREAT);
    shmid_for_cleanup = shm_id;

    sem_ptr = create_sem(SEM_NAME, 1, "create_sem of sem_ptr failed", O_CREAT | O_EXCL );
    sem_out_ptr = create_sem(SEM_NAME_OUT, 1, "create_sem of sem_out_ptr failed", O_CREAT | O_EXCL );

    printf("done\n");
    exit(0);
  } else if (argc == 2 && strcmp(argv[1], "-n") == 0) {
    printf("setting up IPC\n");
    shm_id = create_shm(shm_key, "create", "shmget failed", IPC_CREAT);
    shmid_for_cleanup = shm_id;

    sem_ptr = create_sem(SEM_NAME, 1, "create_sem of sem_ptr failed", O_CREAT);
    sem_out_ptr = create_sem(SEM_NAME_OUT, 1, "create_sem of sem_out_ptr failed", O_CREAT);

    printf("done\n");
    exit(0);
  } else {
    shm_id = create_shm(shm_key, "create", "shmget failed", 0);
    sem_ptr = create_sem(SEM_NAME, 1, "semget failed", 0);
    sem_out_ptr = create_sem(SEM_NAME_OUT, 1, "semget failed", 0);
  }

  if (argc == 2 && strcmp(argv[1], "-c") == 0) {
    printf("cleaning up IPC\n");
    cleanup(shm_id, SEM_NAME, sem_ptr, SEM_NAME_OUT, sem_out_ptr);
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
  for (unsigned int i = 0; i < ARRAY_SIZE; i++) {
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

  long *tcounter = shm_data->counter;
  long *scounter = local_data.counter;

  time_t t1 = time(NULL);
  retcode = sem_wait(sem_ptr);
  handle_error(retcode, "error while getting semaphore", PROCESS_EXIT);
  time_t dt = time(NULL) - t1;

  for (unsigned int i = 0; i < ARRAY_SIZE; i++) {
    tcounter[i] += scounter[i];
    scounter[i] = tcounter[i];
  }

  retcode = sem_post(sem_ptr);
  handle_error(retcode, "error while releasing semaphore", PROCESS_EXIT);

  time_t total = time(NULL) - t0;


  t1 = time(NULL);
  retcode = sem_wait(sem_ptr);
  handle_error(retcode, "error while getting semaphore", PROCESS_EXIT);
  dt = time(NULL) - t1;

  int k = 0;
  printf("------------------------------------------------------------\n");
  printf("%s: pid=%ld\n", name, (long) getpid());
  printf("total time for calculation: ~ %ld sec; total wait for semaphore: ~ %ld sec\n", (long) total, (long) dt);
  printf("------------------------------------------------------------\n");
  for (unsigned int i = 0; i < ARRAY_SIZE; i++) {
    long val = scounter[i];
    if (val > 0) {
      if ((k & 0x0f) == 0x0f) {
        printf("\n");
        fflush(stdout);
      }
      printf("%5d: %10ld    ", i, val);
      k++;
    }
  }
  printf("\n\n");
  printf("------------------------------------------------------------\n\n");
  fflush(stdout);

  retcode = sem_post(sem_ptr);
  handle_error(retcode, "error while releasing semaphore", PROCESS_EXIT);

  retcode = shmdt(shm_data);
  handle_error(retcode, "error while detaching shared memory", PROCESS_EXIT);
  printf("done\n");
  exit(0);
}
