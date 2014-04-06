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

#define SIZE 1024

int main(int argc, char *argv[]) {

  int retcode;

  char buff[SIZE];

  /* read password from file
   * REMARK: this should off course be encrypted for real life software
   */
  FILE *pfile = fopen("passwd", "r");
  handle_ptr_error(pfile, "fopen", PROCESS_EXIT);
  const char *read_passwd = fgets(buff, SIZE, pfile);
  char *passwd = strdup(read_passwd);
  retcode = fclose(pfile);
  handle_error(retcode, "fclose", PROCESS_EXIT);

  /* remove trailing newline */
  int last_pos = strlen(passwd) - 1;
  if (passwd[last_pos] == '\n') {
    passwd[last_pos] = '\000';
  }
  int plen = strlen(passwd);

  /* open tty for reading password from user */
  int tty_fd = open("/dev/tty", O_RDONLY);
  handle_error(tty_fd, "open tty", PROCESS_EXIT);
  struct termios termios_struct;
  struct termios orig_termios_struct;

  retcode = tcgetattr(tty_fd, &orig_termios_struct);
  handle_error(retcode, "tcgetattr (1)", PROCESS_EXIT);
  retcode = tcgetattr(tty_fd, &termios_struct);
  handle_error(retcode, "tcgetattr (2)", PROCESS_EXIT);
  termios_struct.c_lflag &= (~ICANON) & (~ECHO);
  termios_struct.c_cc[VTIME] = 0;
  termios_struct.c_cc[VMIN] = 1;
  retcode = tcsetattr(tty_fd, TCSANOW, &termios_struct);
  handle_error(retcode, "tcsetattr", PROCESS_EXIT);

  /* read password  from keyboard */
  FILE *keyboard = fdopen(tty_fd, "r");
  int authenticated = FALSE;
  while (! authenticated) {
    printf("please enter the password:\n");
    int i;
    authenticated = TRUE;
    for (i = 0; TRUE; i++) {
      char c = fgetc(keyboard);
      write(STDOUT_FILENO, "*", 1);
      if (c == '\n') {
        write(STDOUT_FILENO, "\n", 1);
        if (i < plen) {
          authenticated = FALSE;
        }
        break;
      }
      if (i >= plen || c != passwd[i]) {
        authenticated = FALSE;
      }
    }
    if (! authenticated) {
      printf("wrong password!\n");
    }
  }
  printf("password is ok!\n");

  /* go linewise through file from stdin */
  while (TRUE) {
    fgetc(keyboard);
    char *line = fgets(buff, SIZE, stdin);
    if (line == NULL) {
      break;
    }
    printf("%s", line);
  }
  printf("\n");
  tcsetattr(tty_fd, TCSANOW, &orig_termios_struct);
  exit(0);
}

