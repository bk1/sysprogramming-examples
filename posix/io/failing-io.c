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
  int n;
  int fdout;

  const char *FILENAME = "/tmpx/out-highlevel.txt";
  const char *CONTENT = "Dies ist ein Beispieltext 2";

  n = strlen(CONTENT) + 1; // include '\000'
  fdout = open(FILENAME, O_WRONLY | O_CREAT);
  printf("fdout=%d\n", fdout);
  handle_error(fdout, "open", PROCESS_EXIT);
  /* ... */
  write(fdout, CONTENT, n);
  close(fdout);
  exit(0);
}
