/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>

#include <itskylib.h>

int main(int argc, char *argv[]) {
  int r;
  const char *DIRNAME = "/tmp/test-dir-9876";
  struct dirent *entry;
  r = mkdir(DIRNAME, 0777);
  if (r < 0) {
    int myerrno = errno;
    if (myerrno != EEXIST) {
      handle_error_myerrno(r, myerrno, "mkdir", PROCESS_EXIT);
    }
  }
  DIR *dir = opendir(DIRNAME);
  handle_ptr_error(dir, "opendir", PROCESS_EXIT);
  while (1) {
    entry = readdir(dir);
    if (entry == NULL) {
      int myerrno = errno;
      if (myerrno != 0) {
        handle_error_myerrno(-1, myerrno, "readdir", PROCESS_EXIT);
      } else {
        break;
      }
    }
    printf("inode=%12ld offset=%12ld reclen=%6ld type=%4ld name=%s\n", (long) entry->d_ino, (long) entry->d_off, (long) entry->d_reclen, (long) entry->d_type, entry->d_name);
  }
  r = closedir(dir);
  printf("r=%d\n", r);
  handle_error(r, "closedir", PROCESS_EXIT);
  exit(0);
}
