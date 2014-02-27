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

void handle_error_myerrno(int return_code, int myerrno) {
    const char *error_str = strerror(myerrno);
    printf("return_code=%d\nerrno=%d\nmessage=%s\n", return_code, myerrno, error_str);
    exit(1);
}

void handle_error(int return_code) {
  if (return_code < 0) {
    int myerrno = errno;
    handle_error_myerrno(return_code, myerrno);
  }
}

int main() {
  const char *FILENAME = "/tmp/abc987654";
  FILE *file = fopen(FILENAME, "a");
  int return_code;
  int myerrno;
  if (file == NULL) {
    handle_error(-1);
  }
  fwrite("abcdefghijklmnopqrstuvwxyz", 1, 26, file);
  fwrite("ABCDEFGHIJKLMNOPQRSTUVWXYZ", 1, 26, file);
  int fd = fileno(file);
  int pid = getpid();
  /* try non-blocking-lock */
  lseek(fd, 0, SEEK_SET);
  int pos = 0;
  while (1) {
    return_code = lockf(fd, F_TLOCK, 1);
    if (return_code < 0) {
      myerrno = errno;
      if (myerrno == EACCES || myerrno == EAGAIN) {
        printf("pid=%d could not lock position %d\n", pid, pos);
      } else {
        handle_error_myerrno(return_code, myerrno);
      }
    } else {
      printf("pid=%d could lock position %d\n", pid, pos);
      break;
    }
    pos++;
    lseek(fd, pos, SEEK_SET);
  }
  sleep(5);
  lockf(fd, F_ULOCK, 1);
  printf("pid=%d unlocked position %d\n", pid, pos);
  close(fd);
}
