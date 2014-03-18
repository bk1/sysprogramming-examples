/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <itskylib.h>

#define ERROR_SIZE 4096

void exit_by_type(enum exit_type et) {
  switch (et) {
  case PROCESS_EXIT: 
    exit(1);
    break;
  case THREAD_EXIT:
    pthread_exit(NULL);
    break;
  case NO_EXIT:
    printf("continuing\n");
    break;
  default:
    printf("unknown exit_type=%d\n", et);
    exit(2);
    break;
  }
}


/* helper function for dealing with errors */
void handle_error_myerrno(long return_code, int myerrno, const char *msg, enum exit_type et) {
  if (return_code < 0) {
    char extra_msg[ERROR_SIZE];
    char error_msg[ERROR_SIZE];
    const char *error_str = strerror(myerrno);
    if (msg != NULL) {
      sprintf(extra_msg, "%s\n", msg);
    } else {
      extra_msg[0] = '\000';
    }
    sprintf(error_msg, "%sreturn_code=%ld\nerrno=%d\nmessage=%s\n", extra_msg, return_code, myerrno, error_str);
    write(STDOUT_FILENO, error_msg, strlen(error_msg));
    exit_by_type(et);
  }
}

void handle_thread_error(int retcode, const char *msg, enum exit_type et) {
  if (retcode != 0) {
    handle_error_myerrno(-abs(retcode), retcode, msg, et);
  }
}

/* helper function for dealing with errors */
void handle_error(long return_code, const char *msg, enum exit_type et) {
  int myerrno = errno;
  handle_error_myerrno(return_code, myerrno, msg, et);
}


void handle_ptr_error(void *ptr, const char *msg, enum exit_type et) {
  if (ptr == NULL) {
    handle_error(-1L, msg, et);
  }
}

int open_retry_mode(char *file, int flags, mode_t mode, enum exit_type et) {

  int fd = -1;
  while (fd == -1) {
    fd = open(file, flags, mode);
    if (fd < 0 && errno == EMFILE) {
      sleep(1);
      continue;
    }
    if (fd < 0) {
      char msg[1024];
      sprintf(msg, "error while opening file=%s", file);
      handle_error(fd, msg, et);
    }
  }
  return fd;
}

int open_retry(char *file, int flags, enum exit_type et) {

  int fd = -1;
  while (fd == -1) {
    fd = open(file, flags);
    if (fd < 0 && errno == EMFILE) {
      sleep(1);
      continue;
    }
    if (fd < 0) {
      char msg[1024];
      sprintf(msg, "error while opening file=%s", file);
      handle_error(fd, msg, et);
    }
  }
  return fd;
}


/* check if file exits and what type it is
 * exit with error if errors occur during stat
 * return NOT_EXISTENT, DIRECTORY, REGULAR_FILE or OTHER
 */
enum file_type check_file(const char *file_or_dir_name) {
  int r;
  struct stat stat_buf;
  r = lstat(file_or_dir_name, &stat_buf);
  if (r < 0) {
    int myerrno = errno;
    if (myerrno == ENOENT) {
      /* not existing should be handled as legitimate result, not as error */
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

/* end of file lib.c */
