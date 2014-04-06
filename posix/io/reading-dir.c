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

#include <itskylib.h>

int main(int argc, char *argv[]) {
  int r;

  int fdin;
  const char *DIRNAME = "/tmp/test-dir-9876";

  struct dirent *entry;
  r = mkdir(DIRNAME, 0777);
  handle_error(r, "mkdir", PROCESS_EXIT);
  fdin = open(DIRNAME, O_RDONLY);
  printf("fdin=%d\n", fdin);
  handle_error(fdin, "open", PROCESS_EXIT);
  DIR *dir = fdopendir(fdin);
  handle_ptr_error(dir, "fdopendir", PROCESS_EXIT);
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
  r = close(fdin);
  printf("r=%d\n", r);
  handle_error(r, "close", PROCESS_EXIT);
  exit(0);
}
