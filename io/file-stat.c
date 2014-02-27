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

int main() {
  struct stat stat_buf;
  int r;
  r = stat("file-stat.c", &stat_buf);
  if (r < 0) {
    int myerrno = errno;
    const char *error_str = strerror(myerrno);
    printf("errno=%d\nmessage=%s\n", myerrno, error_str);
    exit(1);
  }
  printf("st_dev=%d\n", stat_buf.st_dev);
  printf("st_ino=%d\n", stat_buf.st_ino);
  printf("st_mode=%o\n", stat_buf.st_mode);
  printf("st_nlink=%d\n", stat_buf.st_nlink);
  printf("st_uid=%d\n", stat_buf.st_uid);
  printf("st_gid=%d\n", stat_buf.st_gid);
  printf("st_rdev=%d\n", stat_buf.st_rdev);
  printf("st_size=%d\n", stat_buf.st_size);
  printf("st_blksize=%d\n", stat_buf.st_blksize);
  printf("st_blocks=%d\n", stat_buf.st_blocks);
  printf("st_atime=%d\n", stat_buf.st_atime);
  printf("st_mtime=%d\n", stat_buf.st_mtime);
  printf("st_ctime=%d\n", stat_buf.st_ctime);
}


