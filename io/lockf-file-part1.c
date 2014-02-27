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
    printf("wrong number of arguments\n");
    exit(1);
  }
  long x, y, len;
  sscanf(argv[1], "%ld", &x);
  printf("x=%ld\n", x);
  sscanf(argv[2], "%ld", &y);
  printf("y=%ld\n", y);
  len = y - x;
  
  int fd = mylock(FILENAME, x, y);
  sleep(10);
  close(fd);
  exit(0);
}
