#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
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
  printf("file opened\n");
  unlink(FILENAME);
  printf("file \"deleted\"\n");
  sleep(5);
  printf("reading from deleted file\n");
  k = fread((void *) buffer, 1, m, in);
  fclose(in);
  printf("%d byte read:\n%s\n", k, buffer);
}
