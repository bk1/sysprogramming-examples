/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */


 /* www.brodowsky.com/tty-read.c */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

int main() {
  int tty_fd = open("/dev/tty", O_RDONLY);
  printf("regular reading\n");
  while (1) {
    char c;
    int b = read(tty_fd, &c, 1);
    if (b < 1) {
      printf("nothing read\n");
      continue;
    }
    printf("%c\n", c);
    fflush(stdout);
    if (c == '\n') {
      break;
    }
  }

  struct termios termios_struct;
  struct termios orig_termios_struct;

  tcgetattr(tty_fd, &orig_termios_struct);
  tcgetattr(tty_fd, &termios_struct);
  termios_struct.c_lflag &= (~ICANON) & (~ECHO);
  termios_struct.c_cc[VTIME] = 0;
  termios_struct.c_cc[VMIN] = 1;
  tcsetattr(tty_fd, TCSANOW, &termios_struct);
  sleep(1);

  printf("changed reading\n");
  while (1) {
    char c;
    int b = read(tty_fd, &c, 1);
    if (b < 1) {
      printf("nothing read\n");
      continue;
    }
    printf("%c\n", c);
    fflush(stdout);
    if (c == '\n') {
      break;
    }
  }

  while (1) {
    FILE *tty_file = fdopen(tty_fd, "r");
    char c = getc(tty_file);
    printf("%c\n", c);
    fflush(stdout);
    if (c == '\n') {
      break;
    }
  }

  printf("regular reading\n");
  tcsetattr(tty_fd, TCSANOW, &orig_termios_struct);

  while (1) {
    char c;
    int b = read(tty_fd, &c, 1);
    if (b < 1) {
      printf("nothing read\n");
      continue;
    }
    printf("%c\n", c);
    fflush(stdout);
    if (c == '\n') {
      break;
    }
  }
  close(tty_fd);
}
