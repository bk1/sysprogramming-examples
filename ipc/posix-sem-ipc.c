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
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <fcntl.h>

#include <itskylib.h>

#define PERM 0600

#define BUF_SIZE 16384

#define ALPHA_SIZE 256

#define SEM_NAME "/sem-posix-sem-ipc"


int main(int argc, char *argv[]) {

  int retcode;

  pid_t pid = fork();
  if (pid == 0) {
    printf("in child: preparing semaphore\n");
    sem_t *semaphore = sem_open(SEM_NAME, O_CREAT, PERM, 1);
    handle_ptr_error(semaphore, "child sem_open", PROCESS_EXIT);
    printf("in child: getting semaphore\n");
    retcode = sem_wait(semaphore);
    handle_error(retcode, "child failed waiting", PROCESS_EXIT);
    printf("child got semaphore\n");
    sleep(5);
    printf("child releases semaphore\n");
    retcode = sem_post(semaphore);
    handle_error(retcode, "child failed posting", PROCESS_EXIT);
    printf("child released semaphore\n");
    retcode = sem_close(semaphore);
    handle_error(retcode, "child failed closing", PROCESS_EXIT);
    printf("child closed semaphore\n");
    sleep(10);
    printf("child exiting\n");
    exit(0);
  } else {
    printf("in parent: getting semaphore\n");
    sem_t *semaphore = sem_open(SEM_NAME, O_CREAT, PERM, 1);
    handle_ptr_error(semaphore, "parent sem_open", PROCESS_EXIT);
    printf("in parent: getting semaphore\n");
    retcode = sem_wait(semaphore);
    handle_error(retcode, "parent failed waiting", PROCESS_EXIT);
    printf("parent got semaphore\n");
    sleep(5);
    printf("parent releases semaphore\n");
    retcode = sem_post(semaphore);
    handle_error(retcode, "parent failed posting", PROCESS_EXIT);
    printf("parent released semaphore\n");
    int status;
    printf("parent waiting for child to terminate\n");
    waitpid(pid, &status, 0);
    handle_error(retcode, "parent: error while detaching shared memory", PROCESS_EXIT);
    retcode = sem_close(semaphore);
    handle_error(retcode, "parent failed closing", PROCESS_EXIT);
    printf("parent closed semaphore\n");
    retcode = sem_unlink(SEM_NAME);
    handle_error(retcode, "parent failed unlinking", PROCESS_EXIT);
    printf("parent unlinked semaphore\n");
    printf("done\n");
    exit(0);
  }
}
