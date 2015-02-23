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

#define READ_BUF_SIZE 20


void usage(char *argv[]) {
    printf("usage\n\n%s -r\nreads file\n\n%s -w\nwrites one line to the end of the file\n\n", argv[0], argv[0]);
    exit(1);
}

enum mode { READ, WRITE };

int main(int argc, char *argv[]) {

  if (argc != 2) {
    usage(argv);
  }
  enum mode mode;
  if (strcmp(argv[1], "-r") == 0) {
    mode = READ;
  } else if (strcmp(argv[1], "-w") == 0) {
    mode = WRITE;
  } else {
    usage(argv);
  }

  const char *FILENAME = "/tmp/flock-example-uvw987654";
  int fd = open(FILENAME, O_RDWR | O_CREAT | O_APPEND, 0666);
  handle_error(fd, "fopen", PROCESS_EXIT);

  int pid = getpid();
  /* try non-blocking-lock */

  int retcode;

  if (mode == READ) {
    printf("trying to lock (sh) file pid=%d\n", pid);
    retcode = flock(fd, LOCK_SH);
    handle_error(retcode, "flock LOCK_SH", PROCESS_EXIT);
    printf("locked file (sh) pid=%d\n", pid);
    sleep(2);
    off_t pos = lseek(fd, 0, SEEK_SET);
    handle_error(pos, "lseek for read", PROCESS_EXIT);
    while (TRUE) {
      
      char buf[READ_BUF_SIZE];
      ssize_t s = read(fd, buf, READ_BUF_SIZE);
      handle_error(s, "read", PROCESS_EXIT);
      if (s == 0) {
        break;
      }
      write(STDOUT_FILENO, buf, s);
    }
    write(STDOUT_FILENO, "\n", 1);
    sleep(2);
    printf("unlocking file pid=%d\n", pid);
    retcode = flock(fd, LOCK_UN);
    handle_error(retcode, "flock LOCK_UN", PROCESS_EXIT);
    printf("unlocked file pid=%d\n", pid);
  } else {
    printf("trying to lock (ex) file pid=%d\n", pid);
    retcode = flock(fd, LOCK_EX);
    handle_error(retcode, "flock LOCK_EX", PROCESS_EXIT);
    printf("locked (ex) file pid=%d\n", pid);
    sleep(2);
    off_t poso = lseek(fd, 0, SEEK_END);
    handle_error(poso, "lseek write", PROCESS_EXIT);
    int pos = ((int) poso) / 5;
    char out_buf[6];
    sprintf(out_buf, "%4d\n", pos*pos);
    write(fd, out_buf, 5);
    sleep(2);
    printf("unlocking file pid=%d\n", pid);
    retcode = flock(fd, LOCK_UN);
    handle_error(retcode, "flock LOCK_UN", PROCESS_EXIT);
    printf("unlocked file pid=%d\n", pid);
  }
  close(fd);
  exit(0);
}
