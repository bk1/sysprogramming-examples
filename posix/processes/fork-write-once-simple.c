/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <itskylib.h>

int main(int argc, char *argv[]) {
  int fd = open("myfile", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
  char *TEXT = "B";
  write(fd, TEXT, strlen(TEXT));
  fork();
  exit(0);
}
