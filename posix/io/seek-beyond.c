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

#include <itskylib.h>

int main(int argc, char *argv[]) {
  const char *FILENAME = "/tmp/out-seek-beyond.txt";
  const char *CONTENT_1 = "Exampletext\n........(1)\n";
  const char *CONTENT_2 = "Exampletext\n(2)\n";

  int fdout1 = open(FILENAME, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP);
  handle_error(fdout1, "open() fdout1", PROCESS_EXIT);
  int n = strlen(CONTENT_1);
  int m = write(fdout1, (void *) CONTENT_1, n);
  printf("%d of %d bytes written\n", m, n);
  handle_error(m-n, "incomplete write fdout1", PROCESS_EXIT);
  int retcode = lseek(fdout1, 4096, SEEK_SET);
  n = strlen(CONTENT_2);
  m = write(fdout1, (void *) CONTENT_2, n);
  printf("%d of %d bytes written\n", m, n);
  handle_error(m-n, "incomplete write fdout2", PROCESS_EXIT);
  retcode = close(fdout1);
  handle_error(retcode, "close(fdout1)", PROCESS_EXIT);
  exit(0);
}
