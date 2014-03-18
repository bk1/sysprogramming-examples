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

int main(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    int fd = open(argv[i], O_RDONLY);
    struct char_array result = read_to_array(fd);
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

