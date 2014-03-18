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

#include <itskylib.h>

int main(int argc, char *argv[]) {
  const char *DIRNAME = "abc-dir";
  const char *FILENAME_FORM = "abc_file_%09d";
  char *filename = (char *) malloc(25);
  int r;
  int retcode;

  /* check if the given directory exists already */
  r = check_file(DIRNAME);
  if (r == OTHER || r == REGULAR_FILE) {
    /* if it exists as non-directory, try to remove it */
    retcode = unlink(DIRNAME);
    handle_error(retcode, "unlink", PROCESS_EXIT);
    r = check_file(DIRNAME);
    if (r != NOT_EXISTENT) {
      printf("file %s could not be deleted\n", DIRNAME);
      exit(1);
    }
  }
  if (r != DIRECTORY) {
    /* try to create a directory */
    retcode = mkdir(DIRNAME, 0777);
    handle_error(retcode, "mkdir", PROCESS_EXIT);
  }
  /* change into the directory */
  retcode = chdir(DIRNAME);
  handle_error(retcode, "chdir", PROCESS_EXIT);

  /* directory exists */
  int i;
  for (i = 0; i < 0x7fffffff; i++) {
    /* create a file name and check if it is available */
    sprintf(filename, FILENAME_FORM, i);
    r = check_file(filename);
    if (r == NOT_EXISTENT) {
      break;
    }
  }
  /* according to the previous checks, the file should be non-existent
   * WARNING: this can cause a race condition, if some other program creates a file with the same name at the same moment
   */
  int fd = open(filename, O_WRONLY | O_CREAT | O_EXCL, 0666);
  handle_error(fd, "open", PROCESS_EXIT);

  char *buff = malloc(2);
  buff[1] = '\000';
  char c;
  for (c = 'Z'; c >= 'A'; c--) {
    /* move to the position wher c should be written */
    retcode = lseek(fd, (off_t)(c - 'A'), SEEK_SET);
    handle_error(retcode, "lseek", PROCESS_EXIT);
    buff[0] = c;
    /* write c */
    retcode = write(fd, buff, 1);
    handle_error(retcode, "write", PROCESS_EXIT);
  }
  retcode = close(fd);
  handle_error(retcode, "close", PROCESS_EXIT);
  exit(0);
}
