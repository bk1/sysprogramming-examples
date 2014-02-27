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
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>

int main(int argc, char *argv[]) {
  int k,m,n,r;
  int fdout;
  int fdin;
  const char *DIRNAME = "/tmp/test-dir-9876";
  char *buffer = (char *) malloc(1000);
  struct dirent *entry;
  r = mkdir(DIRNAME, 0777);
  if (r < 0) {
    int myerrno = errno;
    if (myerrno != EEXIST) {
      const char *error_str = strerror(myerrno);
      printf("errno=%d\nmessage=%s\n", myerrno, error_str);
      exit(1);
    }
  }
  fdin = open(DIRNAME, O_RDONLY);
  printf("fdin=%d\n", fdin);
  if (fdin < 0) {
    int myerrno = errno;
    const char *error_str = strerror(myerrno);
    printf("errno=%d\nmessage=%s\n", myerrno, error_str);
    exit(1);
  }
  DIR *dir = fdopendir(fdin);
  if (dir == NULL) {
    int myerrno = errno;
    const char *error_str = strerror(myerrno);
    printf("errno=%d\nmessage=%s\n", myerrno, error_str);
    exit(1);
  }
  while (1) {
    entry = readdir(dir);
    if (entry == NULL) {
      int myerrno = errno;
      if (myerrno != 0) {
        const char *error_str = strerror(myerrno);
        printf("errno=%d\nmessage=%s\n", myerrno, error_str);
        exit(1);
      } else {
        break;
      }
    }
    printf("inode=%12d offset=%12d reclen=%6d type=%4d name=%s\n", entry->d_ino, entry->d_off, entry->d_reclen, entry->d_type, entry->d_name);
  }
  r = closedir(dir);
  printf("r=%d\n", r);
  if (r < 0) {
    int myerrno = errno;
    const char *error_str = strerror(myerrno);
    printf("errno=%d\nmessage=%s\n", myerrno, error_str);
    exit(1);
  }
  r = close(fdin);
  printf("r=%d\n", r);
  if (r < 0) {
    int myerrno = errno;
    const char *error_str = strerror(myerrno);
    printf("errno=%d\nmessage=%s\n", myerrno, error_str);
    exit(1);
  }
}
