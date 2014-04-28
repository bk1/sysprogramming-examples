/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 *
 * lowlevel IO compiles, but crashes
 */




#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
  const char *msg = "Hello, World!\n(open/write/close)\n";
  int fd = open("../temp/test.txt", O_CREAT | O_WRONLY, 0777);
  write(fd, msg, strlen(msg));
  close(fd);
}

