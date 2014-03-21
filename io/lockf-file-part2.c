/* (C) Max Schrimpf
 *
 * Author: Max Schrimpf
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */


/*
 * Uebung 2 - Systemprogrammierung
 * Max Schrimpf
 */

#include <stdio.h>
// for strerror
#include <errno.h>
#include <string.h>
// for exit()
#include <stdlib.h>
// for lseek
#include <sys/types.h>
// for lockf
#include <unistd.h>

int open_lock_sleep_close( char *path, int from, int to, int sleeptime) {
  FILE *file = fopen(path, "r+");

  if (file == NULL) {
    printf("File can't be opened. Errno: %s\n", strerror(errno));
    exit(1);
  }

  printf("Locking ... \n");
  lseek(fileno(file), (off_t) from, SEEK_SET);
  int offset = to - from;
  lockf(fileno(file), F_LOCK, offset);
  int i;
  for(i = sleeptime; i> 0; i--) {
    sleep(1);
    printf("Sleeping %d remaining \n", i);
  }
  return fclose(file);
}

void usage(char *argv0, char *msg) {
  printf("%s\n\n", msg);
  printf("Usage:\n\n%s x y\n lock part of file from position x (inclusiv) to position y (exclusive), sleep 10 sec\n\n", argv0);
  printf("%s x y sleeptime\n lock part of file from position x (inclusiv) to position y (exclusive), sleep sleeptime sec\n", argv0);
  exit(1);
}

int main ( int argc, char *argv[] ) {
  int from, to, sleeptime;
  if (argc < 4) {
    usage(argv[0], "Not enough parameters specified");
  }
  if (argc < 5) {
    sleeptime = 10;
  } else {
    sleeptime = atoi(argv[4]);
  }
  printf("Working in dir: %s\n", argv[1]);
  from = atoi(argv[2]);
  to = atoi(argv[3]);

  printf("Locking from Byte: %d (sleeptime=%d)\n", from, sleeptime);
  printf("Locking to Byte: %d\n", to);
  return open_lock_sleep_close(argv[1], from, to, sleeptime);
  exit(0);
}
