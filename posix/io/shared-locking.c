/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */


#include <sys/file.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#include <itskylib.h>

int main(int argc, char *argv[]) {
  int retcode;

  const char *FILENAME = "/tmp/uvw987654";
  FILE *file = fopen(FILENAME, "a");
  handle_ptr_error(file, "fopen", PROCESS_EXIT);

  fputc('a', file);
  int fd = fileno(file);
  int pid = getpid();
  /* try non-blocking-lock */
  printf("trying to lock (sh) file pid=%d\n", pid);
  retcode = flock(fd, LOCK_SH);
  handle_error(retcode, "flock LOCK_SH", PROCESS_EXIT);
  printf("locked file (sh) pid=%d\n", pid);
  sleep(5);
  printf("unlocking file pid=%d\n", pid);
  retcode = flock(fd, LOCK_UN);
  handle_error(retcode, "flock LOCK_UN", PROCESS_EXIT);
  printf("unlocked file pid=%d\n", pid);
  printf("trying to lock (ex) file pid=%d\n", pid);
  retcode = flock(fd, LOCK_EX);
  handle_error(retcode, "flock LOCK_EX", PROCESS_EXIT);
  printf("locked (ex) file pid=%d\n", pid);
  sleep(5);
  printf("unlocking file pid=%d\n", pid);
  retcode = flock(fd, LOCK_UN);
  handle_error(retcode, "flock LOCK_UN", PROCESS_EXIT);
  printf("unlocked file pid=%d\n", pid);
  close(fd);
  exit(0);
}
