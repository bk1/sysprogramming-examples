/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#ifndef _LIB_ITSKY_SYSPROGRAMMING
#define _LIB_ITSKY_SYSPROGRAMMING

enum exit_type { PROCESS_EXIT, THREAD_EXIT };

enum file_type { NOT_EXISTENT, DIRECTORY, REGULAR_FILE, OTHER };

void exit_by_type(enum exit_type et);

void handle_thread_error(int retcode, const char *msg, enum exit_type et);

/* helper function for dealing with errors */
void handle_error(long return_code, const char *msg, enum exit_type et);

void handle_ptr_error(void *ptr, const char *msg, enum exit_type et);

int open_retry_mode(char *file, int flags, mode_t mode, enum exit_type et);

int open_retry(char *file, int flags, enum exit_type et);

enum file_type check_file(const char *file_or_dir_name);

#endif

