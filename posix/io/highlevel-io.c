/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int k,m,n;
  FILE *out;
  FILE *in;
  const char *FILENAME = "/tmp/out-highlevel.txt";
  const char *CONTENT = "Dies ist ein Beispieltext";
  char *buffer;
  n = strlen(CONTENT) + 1; // include '\000'
  out = fopen(FILENAME, "w");
  m = fwrite((void *) CONTENT, 1, n, out);
  fclose(out);
  printf("%d bytes written (lenght %d)\n", m, n);
  buffer = (char *) malloc(m);
  in = fopen(FILENAME, "r");
  k = fread((void *) buffer, 1, m, in);
  fclose(in);
  printf("%d byte read:\n%s\n", k, buffer);
  exit(0);
}
