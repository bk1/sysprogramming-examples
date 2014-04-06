/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <itskylib.h>

int main(int argc, char *argv[]) {
  struct stat stat_buf;
  int r;
  r = fstat(STDIN_FILENO, &stat_buf);
  if (r < 0) {
    int myerrno = errno;
    if (myerrno != EEXIST) {
      handle_error_myerrno(r, myerrno, "fstat", PROCESS_EXIT);
    }
  }
  printf("st_dev=%ld\n", (long) stat_buf.st_dev);
  printf("st_ino=%ld\n", (long) stat_buf.st_ino);
  printf("st_mode=%o\n", stat_buf.st_mode);
  printf("st_nlink=%ld\n", (long) stat_buf.st_nlink);
  printf("st_uid=%ld\n", (long) stat_buf.st_uid);
  printf("st_gid=%ld\n", (long) stat_buf.st_gid);
  printf("st_rdev=%ld\n", (long) stat_buf.st_rdev);
  printf("st_size=%ld\n", (long) stat_buf.st_size);
  printf("st_blksize=%ld\n", (long) stat_buf.st_blksize);
  printf("st_blocks=%ld\n", (long) stat_buf.st_blocks);
  printf("st_atime=%ld\n", (long) stat_buf.st_atime);
  printf("st_mtime=%ld\n", (long) stat_buf.st_mtime);
  printf("st_ctime=%ld\n", (long) stat_buf.st_ctime);
  exit(0);
}
