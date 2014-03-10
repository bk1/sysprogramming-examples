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

#define FALSE 0
#define TRUE  1

#define ERROR_SIZE 16384

#define PERM 0600

#define BUF_SIZE 16384

int shmid_for_cleanup = 0;

struct data {
  long counter[256];
};

const int SIZE = sizeof(struct data);

void handle_error(int return_code, const char *msg);

int create_shm(key_t key, const char *txt, const char *etxt) {
  int shm_id = shmget(key, SIZE, IPC_CREAT | PERM);
  handle_error(shm_id, etxt);
  printf("%s: shm_id=%d key=%ld\n", txt, shm_id, (long) key);
  return shm_id;
}

void cleanup() {
  if (shmid_for_cleanup > 0) {
    int retcode = shmctl(shmid_for_cleanup, IPC_RMID, NULL);
    shmid_for_cleanup = 0;
    handle_error(retcode, "removing of shm failed");
  }
}


void my_handler(int signo) {
  cleanup();
  exit(1);
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
    cleanup();
    exit(1);
  }
}


void show_shm_ctl(int shm_id, const char *txt) {

  int retcode;
  struct shmid_ds shmctl_data;
  retcode = shmctl(shm_id, IPC_STAT, &shmctl_data);
  handle_error(retcode, "child shmctl failed");
  struct ipc_perm perms = shmctl_data.shm_perm;
  printf("%s: key=%ld uid=%d gid=%d cuid=%d cgid=%d mode=%d seq=%d\n", txt, (long) perms.__key, (int) perms.uid, (int) perms.gid, (int) perms.cuid, (int) perms.cgid, (int) perms.mode, (int)perms.__seq);
}

int main(int argc, char *argv[]) {
  // int retcode = 0;

  signal(SIGTERM, my_handler);
  signal(SIGINT, my_handler);

  FILE *f = fopen("shmref.dat", "w");
  fwrite("X", 1, 1, f);
  fclose(f);

  key_t shm_key = ftok("./shmref.dat", 1);
  if (shm_key < 0) {
    handle_error(-1, "ftok failed");
  }
  int shm_id = create_shm(shm_key, "create", "shmget failed");
  struct data *shm_data = (struct data *) shmat(shm_id, NULL, 0);
  
  char buffer[BUF_SIZE];
  while (TRUE) {
    read(STDIN_FILENO, buffer, BUF_SIZE);
  }

  shmdt(shm_data);
  exit(0);
}
