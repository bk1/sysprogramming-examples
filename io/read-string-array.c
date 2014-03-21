/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include <itskylib.h>

void usage(char *argv0, char *msg) {
  printf("%s\n", msg);
  printf("Usage:\n\n");
  printf("%s file1 file2 file3 ... filen\n", argv0);
  printf(" reads each file and creates an array of strings from the file in the order read, separating strings by sequences of control characters or spaces\n\n");
  exit(1);
}

int main(int argc, char *argv[]) {
  if (argc >= 2 && strcmp(argv[1], "-h") == 0) {
    usage(argv[0], "");
  }

  for (int i = 1; i < argc; i++) {
    int fd = open(argv[i], O_RDONLY);
    struct string_array result = read_to_array(fd);
    close(fd);
    printf("------------------------------------------------------------\n");
    printf("file=%s %d strings\n", argv[i], result.len);
    printf("------------------------------------------------------------\n");
    for (int j = 0; j < result.len; j++) {
      printf("%4d: \"%s\"\n", j, result.strings[j]);
    }
    printf("------------------------------------------------------------\n\n");
  }
}

