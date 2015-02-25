/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  printf("regular reading\n");
  while (1) {
    char c;
    int b = read(STDIN_FILENO, &c, 1);
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

  tcgetattr(STDIN_FILENO, &orig_termios_struct);
  tcgetattr(STDIN_FILENO, &termios_struct);
  // could be done like this also:
  // termios_struct = orig_termios_struct;

  termios_struct.c_lflag &= (~ICANON) & (~ECHO);
  termios_struct.c_cc[VTIME] = 0;
  termios_struct.c_cc[VMIN] = 1;
  tcsetattr(STDIN_FILENO, TCSANOW, &termios_struct);
  sleep(1);

  printf("changed reading\n");
  while (1) {
    char c;
    int b = read(STDIN_FILENO, &c, 1);
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
    char c = getchar();
    printf("%c\n", c);
    fflush(stdout);
    if (c == '\n') {
      break;
    }
  }

  printf("regular reading\n");
  tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios_struct);

  while (1) {
    char c;
    int b = read(STDIN_FILENO, &c, 1);
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
  exit(0);
}
