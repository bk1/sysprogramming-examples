/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/*
 * library functions commonly used for the system programming examples
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
#include <syslog.h>

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


void handle_error_myerrno_internal(long return_code, int myerrno, const char *msg, enum exit_type et, enum log_type lt) {
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
    switch (lt) {
    case LT_SYSLOG:
      syslog(LOG_ERR, error_msg);
      break;
    case LT_STDOUT:
      write(STDOUT_FILENO, error_msg, strlen(error_msg));
      break;
    case LT_STDERR:
      write(STDERR_FILENO, error_msg, strlen(error_msg));
      break;
    default:
      handle_error_myerrno_internal(return_code, myerrno, msg, NO_EXIT, LT_STDERR);
      handle_error_myerrno_internal(return_code, myerrno, "wrong log_type", NO_EXIT, LT_STDERR);
    }
    exit_by_type(et);
  }
}

/* helper function for dealing with errors */
void handle_error_myerrno(long return_code, int myerrno, const char *msg, enum exit_type et) {
  handle_error_myerrno_internal(return_code, myerrno, msg, et, LT_STDOUT);
}

/* helper function for dealing with errors */
void handle_error_myerrno_syslog(long return_code, int myerrno, const char *msg, enum exit_type et) {
  handle_error_myerrno_internal(return_code, myerrno, msg, et, LT_SYSLOG);
}

void handle_thread_error_internal(int retcode, const char *msg, enum exit_type et, enum log_type lt) {
  if (retcode != 0) {
    handle_error_myerrno_internal(-abs(retcode), retcode, msg, et, lt);
  }
}

void handle_thread_error(int retcode, const char *msg, enum exit_type et) {
  handle_thread_error_internal(retcode, msg, et, LT_STDERR);
}

void handle_thread_error_syslog(int retcode, const char *msg, enum exit_type et) {
  handle_thread_error_internal(retcode, msg, et, LT_SYSLOG);
}

void handle_error_internal(long return_code, const char *msg, enum exit_type et, enum log_type lt) {
  int myerrno = errno;
  handle_error_myerrno_internal(return_code, myerrno, msg, et, lt);
}

/* helper function for dealing with errors */
void handle_error(long return_code, const char *msg, enum exit_type et) {
  handle_error_internal(return_code, msg, et, LT_STDOUT);
}

void handle_error_syslog(long return_code, const char *msg, enum exit_type et) {
  handle_error_internal(return_code, msg, et, LT_SYSLOG);
}

void handle_ptr_error_internal(void *ptr, const char *msg, enum exit_type et, enum log_type lt) {
  if (ptr == NULL) {
    handle_error_internal(-1L, msg, et, lt);
  }
}


void handle_ptr_error(void *ptr, const char *msg, enum exit_type et) {
  handle_ptr_error_internal(ptr, msg, et, LT_STDOUT);
}

void handle_ptr_error_syslog(void *ptr, const char *msg, enum exit_type et) {
  handle_ptr_error_internal(ptr, msg, et, LT_SYSLOG);
}


void die_with_error(char *error_message) {
    perror(error_message);
    exit(1);
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

/* create a given file if it is not there */
int create_if_missing(const char *pathname, mode_t mode) {
  int fd = creat(pathname, mode);
  if (fd < 0) {
    char s[1024];
    sprintf(s, "could not create file=\"%s\".", pathname);
    handle_error(fd, s, NO_EXIT);
    return fd;
  }
  int retcode = close(fd);
  handle_error(fd, "close", NO_EXIT);
  return retcode;
}

/* check if --help or similar is indicated */
int is_help_requested(int argc, char *argv[]) {
  return (argc >= 2
          && (strcmp(argv[1], "-h") == 0
              || strcmp(argv[1], "-H") == 0
              || strcmp(argv[1], "-help") == 0
              || strcmp(argv[1], "--help") == 0));
}

/* get a timestamp that is s seconds and n nanoseconds into the future from now */
struct timespec get_future(time_t sec, long nsec) {
  int retcode;

  struct timespec ts;
  retcode = clock_gettime(CLOCK_REALTIME, &ts);
  handle_error(retcode, "clock_gettime", PROCESS_EXIT);
  // printf("timout(%ld sec %ld nsec)\n", (long) ts.tv_sec, (long) ts.tv_nsec);
  ts.tv_sec  += sec;
  ts.tv_nsec += nsec;
  // printf("timout(%ld sec %ld nsec)\n", (long) ts.tv_sec, (long) ts.tv_nsec);
  return ts;
}

int is_string_char(char c) {
  return ('A' <= c && c <= 'Z' || '0' <= c && c <= '9' || 'a' <= c && c <= 'z' || '\240' <= c && c <= '\377');
}

/* read the contents of a file and convert it to an array of strings containing the readable characters interpreted as 8-bit-charset */
struct string_array read_to_array(int fd) {
  struct stat stat;
  long retcode = fstat(fd, &stat);
  handle_error(retcode, "stat", PROCESS_EXIT);
  if (! S_ISREG(stat.st_mode)) {
    handle_error(-1, "no regular file", PROCESS_EXIT);
  }
  off_t size = stat.st_size;
  // printf("read_to_array size=%ld\n", (long)size);
  char *content = malloc(size + 1);
  content[size] = '\000';
  retcode = read(fd, content, size);
  // printf("read %ld bytes\n", retcode);
  handle_error(retcode, "read", PROCESS_EXIT);
  if (retcode < size) {
    char error[1024];
    sprintf(error, "read %ld characters instead of expected %ld", (long) retcode, (long) size);
    /* TODO we should be more generous with read not taking all of the file contents at once */
    handle_error(-1, error, PROCESS_EXIT);
  }
  return split_to_array(content, size);
}

struct string_array split_to_array(char *content, off_t size) {
  /* temporarily make a two-level structure for the string pointers */
  struct string_array blocks[MAX_BLOCK_COUNT]; // 1048576]; // 1024*1024
  // printf("blocks created\n");
  for (int i = 0; i < MAX_BLOCK_COUNT; i++) {
    blocks[i].len = 0;
    blocks[i].strings = (char **) NULL;
  }

  //printf("blocks initialized\n");
  long string_count = 0;
  long block_count = 0;
  long string_pos = 0;
  long block_pos = 0;

  char *source_pointer = content;
  char *target_pointer = content;
  char *end_pointer = content + size;
  char *string_pointer = content;
  /* go through everything */
  while (source_pointer <= end_pointer) {
    /* go through one string, ends latest at end_pointer */
    string_pointer = target_pointer;
    while (source_pointer <= end_pointer) {
      char c = *source_pointer;
      source_pointer++;
      if (is_string_char(c)) {
        *target_pointer = c;
        target_pointer++;
      } else {
        *target_pointer = '\000';
        target_pointer++;
        break;
      }
    }
    if (target_pointer > string_pointer) {
      // printf("found string %s (sc=%ld bc=%ld sp=%ld bp=%ld)\n", string_pointer, string_count, block_count, string_pos, block_pos);
      /* real string found */
      struct string_array *block = &(blocks[block_pos]);
      if (block->strings == NULL) {
        // printf("initialzing strings in block_pos=%ld\n", block_pos);
        char **ptr =  (char **) malloc(MAX_BLOCK_SIZE * sizeof(char *));
        handle_ptr_error(ptr, "malloc", PROCESS_EXIT);
        block->strings = ptr;
        block_count++;
      }
      block->strings[string_pos] = string_pointer;
      block->len++;
      string_pos++;
      string_count++;
      if (string_pos >= MAX_BLOCK_SIZE) {
        //printf("incrementing block_pos sp=%ld bp=%ld\n", string_pos, block_pos);
        string_pos = 0;
        block_pos++;
        //printf("incremented block_pos sp=%ld bp=%ld\n", string_pos, block_pos);
      }
    }
    while (source_pointer <= end_pointer) {
      char c = *source_pointer;
      if (is_string_char(c)) {
        break;
      }
      source_pointer++;
    }
  }
  struct string_array result;
  result.len = string_count;
  result.strings = malloc(string_count * sizeof(char *));
  string_pos = 0;
  block_pos = 0;
  for (int i = 0; i < string_count; i++) {
    result.strings[i] = blocks[block_pos].strings[string_pos];
    string_pos++;
    if (string_pos >= MAX_BLOCK_SIZE) {
      string_pos = 0;
      block_pos++;
    }
  }
  for (int i = 0; i < block_count; i++) {
    free(blocks[i].strings);
  }
  return result;
}


ssize_t readn(int fd, void *buf, size_t total_size) {
  if (fd < 0) {
    return fd;
  }
  size_t rest_size = total_size;
  size_t read_size = 0;
  void *buf_ptr = buf;
  while (rest_size > 0) {
    ssize_t partial_size = read(fd, buf_ptr, rest_size);
    if (partial_size < 0) {
      if (errno == EINTR) {
        partial_size = 0;
        continue;
      }
      return partial_size;
    }
    if (partial_size == 0) {
      return read_size;
    }
    read_size += partial_size;
    rest_size -= partial_size;
    buf_ptr += partial_size;
  }
  return read_size;
}

ssize_t writen(int fd, void *buf, size_t total_size) {
  if (fd < 0) {
    return fd;
  }
  size_t rest_size = total_size;
  size_t written_size = 0;
  void *buf_ptr = buf;
  while (rest_size > 0) {
    ssize_t partial_size = write(fd, buf_ptr, rest_size);
    if (partial_size < 0) {
      if (errno == EINTR) {
        partial_size = 0;
        continue;
      }
      return partial_size;
    }
    if (partial_size == 0) {
      return written_size;
    }
    written_size += partial_size;
    rest_size -= partial_size;
    buf_ptr += partial_size;
  }
  return written_size;
}


/* end of file lib.c */
