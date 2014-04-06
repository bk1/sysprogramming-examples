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
  const char *FILENAME = "/tmp/abc987654";
  int retcode;

  FILE *file = fopen(FILENAME, "a");
  handle_ptr_error(file, "fopen", PROCESS_EXIT);

  fwrite("abcdefghijklmnopqrstuvwxyz", 1, 26, file);
  fwrite("ABCDEFGHIJKLMNOPQRSTUVWXYZ", 1, 26, file);

  int fd = fileno(file);
  int pid = getpid();
  /* try non-blocking-lock */
  retcode = lseek(fd, 0, SEEK_SET);
  handle_error(retcode, "lseek", PROCESS_EXIT);
  int pos = 0;
  printf("pid=%d trying to lock position %d\n", pid, pos);
  retcode = lockf(fd, F_LOCK, 1);
  handle_error(retcode, "lockf", PROCESS_EXIT);
  printf("pid=%d could lock position %d\n", pid, pos);
  pos++;
  sleep(5);
  retcode = lockf(fd, F_ULOCK, 1);
  handle_error(retcode, "lockf", PROCESS_EXIT);
  printf("pid=%d unlocked position %d\n", pid, pos);
  retcode = close(fd);
  handle_error(retcode, "close", PROCESS_EXIT);
  exit(0);
}
