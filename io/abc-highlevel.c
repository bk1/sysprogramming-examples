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

#define NOT_EXISTENT  0
#define DIRECTORY  1
#define REGULAR_FILE       2
#define OTHER      3

/* check if file exits and what type it is
 * exit with error if errors occur during stat
 * return NOT_EXISTENT, DIRECTORY, REGULAR_FILE or OTHER
 */
int check_file(const char *file_or_dir_name) {
  int r;
  struct stat stat_buf;
  r = lstat(file_or_dir_name, &stat_buf);
  if (r < 0) {
    int myerrno = errno;
    if (myerrno == ENOENT) {
      return NOT_EXISTENT;
    } else {
      const char *error_str = strerror(myerrno);
      printf("errno=%d\nmessage=%s\n", myerrno, error_str);
      exit(1);
    }
  }
  mode_t st_mode = stat_buf.st_mode;
  if (S_ISDIR(st_mode)) {
    return DIRECTORY;
  } else if (S_ISREG(st_mode)) {
    return REGULAR_FILE;
  } else {
    return OTHER;
  }
}

/* helper function for dealing with errors */
void handle_error(int return_code) {
  if (return_code < 0) {
    int myerrno = errno;
    const char *error_str = strerror(myerrno);
    printf("return_code=%d\nerrno=%d\nmessage=%s\n", return_code, myerrno, error_str);
    exit(1);
  }
}

/* helper function for dealing with errors concerning FILE pointers */
void handle_errorf(FILE *ptr) {
  if (ptr == NULL) {
    handle_error(-1);
  }
}

int main(int argc, char *argv[]) {
  const char *DIRNAME = "abc-dir";
  const char *FILENAME_FORM = "abc_file_%09d";
  char *filename = (char *) malloc(25);
  int r;
  int return_code;

  /* check if the given directory exists already */
  r = check_file(DIRNAME);
  if (r == OTHER || r == REGULAR_FILE) {
    /* if it exists as non-directory, try to remove it */
    return_code = unlink(DIRNAME);
    handle_error(return_code);
    r = check_file(DIRNAME);
    if (r != NOT_EXISTENT) {
      printf("file %s could not be deleted\n", DIRNAME);
      exit(1);
    }
  }
  if (r != DIRECTORY) {
    /* try to create a directory */
    return_code = mkdir(DIRNAME, 0777);
    handle_error(return_code);
  }
  /* change into the directory */
  return_code = chdir(DIRNAME);
  handle_error(return_code);

  /* directory exists, find file name that is not in use */
  int i;
  for (i = 0; i < 0x7fffffff; i++) {
    sprintf(filename, FILENAME_FORM, i);
    r = check_file(filename);
    if (r == NOT_EXISTENT) {
      break;
    }
  }
  /* according to the previous checks, the file should be non-existent
   * WARNING: this can cause a race condition, if some other program creates a file with the same name at the same moment
   */
  FILE *file = fopen(filename, "w");
  handle_errorf(file);

  char c;
  for (c = 'Z'; c >= 'A'; c--) {
    /* move to the position wher c should be written */
    return_code = fseek(file, (off_t)(c - 'A'), SEEK_SET);
    handle_error(return_code);
    /* write c */
    return_code = fputc(c, file);
    handle_error(return_code);
  }
  return_code = fclose(file);
  handle_error(return_code);
  exit(0);
}
