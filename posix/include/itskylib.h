/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#ifndef _LIB_ITSKY_SYSPROGRAMMING
#define _LIB_ITSKY_SYSPROGRAMMING

#include <time.h>
#include <fcntl.h>

#define TRUE 1
#define FALSE 0

#define MAX_BLOCK_COUNT 100000
#define MAX_BLOCK_SIZE  100000

typedef char *char_ptr;

struct string_array {
  char **strings;
  int len;
};

enum exit_type { PROCESS_EXIT, THREAD_EXIT, NO_EXIT };

enum log_type { LT_SYSLOG, LT_STDOUT, LT_STDERR };

enum file_type { NOT_EXISTENT, DIRECTORY, REGULAR_FILE, OTHER };

/* check if --help or similar is indicated */
int is_help_requested(int argc, char *argv[]);

struct timespec get_future(time_t sec, long nsec);

void exit_by_type(enum exit_type et);

void handle_thread_error(int retcode, const char *msg, enum exit_type et);

void handle_thread_error_syslog(int retcode, const char *msg, enum exit_type et);

/* helper function for dealing with errors */
void handle_error(long return_code, const char *msg, enum exit_type et);

/* helper function for dealing with errors */
void handle_error_syslog(long return_code, const char *msg, enum exit_type et);

void handle_error_myerrno(long return_code, int myerrno, const char *msg, enum exit_type et);

void handle_error_myerrno_syslog(long return_code, int myerrno, const char *msg, enum exit_type et);

void handle_ptr_error(void *ptr, const char *msg, enum exit_type et);

void handle_ptr_error_syslog(void *ptr, const char *msg, enum exit_type et);

void die_with_error(char *error_message);

int open_retry_mode(char *file, int flags, mode_t mode, enum exit_type et);

int open_retry(char *file, int flags, enum exit_type et);

enum file_type check_file(const char *file_or_dir_name);

/* creates a file if it does not exist */
int create_if_missing(const char *pathname, mode_t mode);

int is_string_char(char c);

/* read the contents of a file and convert it to an array of strings containing the readable characters interpreted as 8-bit-charset */
struct string_array read_to_array(int fd);

struct string_array split_to_array(char *content, off_t size);

ssize_t readn(int fd, void *buf, size_t count);

ssize_t writen(int fd, void *buf, size_t count);

#endif

