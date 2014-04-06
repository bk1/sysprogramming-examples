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
  int retcode;

  int k;
  int fdin;
  const char *DIRNAME = "/tmp/test-dir-9876";
  char *buffer = (char *) malloc(1000);
  retcode = mkdir(DIRNAME, 0777);
  if (retcode < 0) {
    int myerrno = errno;
    if (myerrno != EEXIST) {
      handle_error_myerrno(retcode,myerrno, "mkdir", PROCESS_EXIT);
    }
  }
  fdin = open(DIRNAME, O_RDONLY);
  printf("fdin=%d\n", fdin);
  handle_error(fdin, "open", PROCESS_EXIT);
  k = read(fdin, (void *) buffer, 999);
  printf("k=%d\n", k);
  handle_error(k, "read", PROCESS_EXIT);
  retcode = close(fdin);
  printf("r=%d\n", retcode);
  handle_error(retcode, "close", PROCESS_EXIT);
  buffer[999] = (char) 0;
  printf("%d bytes read:\n%s\n", k, buffer);
  exit(0);
}
