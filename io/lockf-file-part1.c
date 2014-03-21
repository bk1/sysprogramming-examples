/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */


 /* http://www.brodowsky.com/main.c */
/** bk1@gmx.net */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int mylock(const char *filename, int start, int end) {
  int fd = open(filename, O_RDWR | O_APPEND);
  /* mit lseek an start */
  /* mit lockf end-start bytes locken */
  /* file offen lassen!! */
  return fd;
}

void usage(char *argv0, char *msg) {
  printf("%s\n\n", msg);
  printf("Usage:\n\n%s\n create file to be dealt with and exit\n%s x y\n lock part of file from position x (inclusiv) to position y (exclusive), sleep 10 sec\n");
  exit(1);
}

int main(int argc, char *argv[]) {
  const char *FILENAME="/tmp/myfile-1337";
  if (argc == 1) {
    char text[1337];
    int i;
    for (i = 0; i < 1337; i++) {
      text[i] = (char) ((i % 90) + 32);
    }
    FILE *file = fopen(FILENAME, "w");
    fwrite(text, 1, 1337, file);
    fclose(file);
    exit(0);
  }
  if (argc < 3) {
    usage(argv[0], "wrong number of arguments");
  }
  long x = atol(argv[1]);
  long y = atol(argv[2]);
  long len = y - x;
  printf("x=%ld y=%ld len=%ld\n", x, y, len);

  int fd = mylock(FILENAME, x, y);
  sleep(10);
  close(fd);
  exit(0);
}
