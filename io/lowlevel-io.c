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

int main() {
  int k,m,n;
  int fdout;
  int fdin;
  const char *FILENAME = "/tmp/out-highlevel.txt";
  const char *CONTENT = "Dies ist ein Beispieltext 2";
  char *buffer;
  n = strlen(CONTENT) + 1; // include '\000'
  fdout = open(FILENAME, O_WRONLY | O_CREAT);
  printf("fdout=%d\n", fdout);
  m = write(fdout, (void *) CONTENT, n);
  close(fdout);
  printf("%d bytes written (lenght %d)\n", m, n);
  buffer = (char *) malloc(m);
  fdin = open(FILENAME, O_RDONLY);
  printf("fdin=%d\n", fdin);
  k = read(fdin, (void *) buffer, m);
  close(fdin);
  printf("%d byte read:\n%s\n", k, buffer);
}
