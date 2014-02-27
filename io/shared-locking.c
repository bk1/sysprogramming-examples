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

int main(int argc, char *argv[]) {
  const char *FILENAME = "/tmp/uvw987654";
  FILE *file = fopen(FILENAME, "a");
  int return_code;
  int myerrno;
  if (file == NULL) {
    handle_error(-1);
  }
  fputc('a', file);
  int fd = fileno(file);
  int pid = getpid();
  /* try non-blocking-lock */
  printf("trying to lock (sh) file pid=%d\n", pid);
  return_code = flock(fd, LOCK_SH);
  handle_error(return_code);
  printf("locked file (sh) pid=%d\n", pid);
  sleep(5);
  printf("unlocking file pid=%d\n", pid);
  return_code = flock(fd, LOCK_UN);
  handle_error(return_code);
  printf("unlocked file pid=%d\n", pid);
  printf("trying to lock (ex) file pid=%d\n", pid);
  return_code = flock(fd, LOCK_EX);
  handle_error(return_code);
  printf("locked (ex) file pid=%d\n", pid);
  sleep(5);
  printf("unlocking file pid=%d\n", pid);
  return_code = flock(fd, LOCK_UN);
  handle_error(return_code);
  printf("unlocked file pid=%d\n", pid);
  close(fd);
}
