/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2015-02-22
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include <itskylib.h>

int main(int argc, char *argv[]) {
  const char *FILENAME = "square_file.txt";
  int retcode;

  int fd = open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  handle_error(fd, "open", PROCESS_EXIT);

  char *buff = malloc(6);
  buff[5] = '\000';
  int i;
  for (i = 99; i >= 0; i--) {
    /* move to the position wher c should be written */
    retcode = lseek(fd, (off_t)(5 * i), SEEK_SET);
    handle_error(retcode, "lseek", PROCESS_EXIT);
    sprintf(buff, "%4d\n", i*i);
    /* write c */
    retcode = write(fd, buff, 5);
    handle_error(retcode, "write", PROCESS_EXIT);
  }
  retcode = close(fd);
  handle_error(retcode, "close", PROCESS_EXIT);
  exit(0);
}
