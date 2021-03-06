/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2015-02-25
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <errno.h>
// #include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include <itskylib.h>

#define SIZE 1024
#define FILE_SIZE (64*1024*1024)

void usage(char *argv0, char *msg) {
  printf("Usage:\n\n%s file\n\n%s < file\n", argv0, argv0);
  if (msg != NULL) {
    printf("\n\n%s\n", msg);
  }
  exit(1);
}

/*
 * poor mans less
 * support one file at a time
 * max size is FILE_SIZE
 */
int main(int argc, char *argv[]) {

  if (argc > 2) {
    usage(argv[0], "wrong number of arguments");
  }

  struct string_array strings;
  long retcode;

  if (argc == 1) {
    struct stat stat;
    retcode = fstat(STDIN_FILENO, &stat);
    handle_error(retcode, "stat", PROCESS_EXIT);
    if (S_ISREG(stat.st_mode)) {
      strings = read_to_array(STDIN_FILENO);
    } else {
      char *buff = malloc(FILE_SIZE+1);
      handle_ptr_error(buff, "malloc", PROCESS_EXIT);
      int size = read(STDIN_FILENO, buff, FILE_SIZE);
      buff[size] = '\000';
      handle_error(size, "read stdin", PROCESS_EXIT);
      strings = split_to_array(buff, size);
    }
  } else {
    int fd = open(argv[1], O_RDONLY);
    handle_error(fd, "open", PROCESS_EXIT);
    strings = read_to_array(fd);
    retcode = close(fd);
    handle_error(retcode, "close fd", PROCESS_EXIT);
  }

  /* close stdin */
  retcode = close(STDIN_FILENO);
  handle_error(retcode, "close stdin", PROCESS_EXIT);
  /* open /dev/tty. Will go to stdin-fileno */
  int tty_fd = open("/dev/tty", O_RDONLY);
  handle_error(tty_fd, "open tty", PROCESS_EXIT);
  if (tty_fd != STDIN_FILENO) {
    fprintf(stderr, "tty_fd=%d stdin_fileno=%d should match", tty_fd, STDIN_FILENO);
    exit(1);
  }

  struct termios termios_struct;
  struct termios orig_termios_struct;

  retcode = tcgetattr(tty_fd, &orig_termios_struct);
  handle_error(retcode, "tcgetattr (1)", PROCESS_EXIT);
  /* retain for cleanup */
  termios_struct = orig_termios_struct;

  /* go to key-press-mode */
  termios_struct.c_lflag &= (~ICANON) & (~ECHO);
  termios_struct.c_cc[VTIME] = 0;
  termios_struct.c_cc[VMIN] = 1;
  termios_struct.c_cc[VINTR] = 0;
  termios_struct.c_cc[VQUIT] = 0;
  termios_struct.c_cc[VSUSP] = 0;
  retcode = tcsetattr(tty_fd, TCSANOW, &termios_struct);
  handle_error(retcode, "tcsetattr", PROCESS_EXIT);

  size_t pos = 0;
  size_t new_pos = 0;

  char kbd_buf[10];
  char prev_c = '\000';
  while (TRUE) {
    int s = read(STDIN_FILENO, kbd_buf, 1);
    handle_error(s, "read tty", PROCESS_EXIT);
    if (s == 0) {
      printf("end of tty-transmission\n");
      break;
    }
    char c = kbd_buf[0];
    /* ignore some non-printing characters */
    if (c == '\000' || c > 'z') {
      continue;
    }
    /* convert to upper case */
    if ('a' <= c && c <= 'z') {
      c-= 'a';
      c+= 'A';
    }
    int quit = FALSE;
    switch (c) {
    case 'Q':
    case '\x03':
    case '\x04':
      printf("\nQ\n");
      quit = TRUE;
      break;
    case 'J': 
    case '\x10': // ctrl-P
      if (c != prev_c) {
        printf("\n");
      }
      if (pos > 0) {
        new_pos = pos - 1;
      } else {
        new_pos = 0;
      }
      printf(" %s", strings.strings[new_pos]);
      fflush(stdout);
      pos = new_pos;
      break;
    case 'K': 
    case '\x0e':
      if (c != prev_c) {
        printf("\n");
      }
      if (pos < strings.len) {
        new_pos = pos;
      } else {
        new_pos = strings.len - 1;
      }
      printf(" %s", strings.strings[new_pos]);
      fflush(stdout);
      pos = new_pos + 1;
      break;
    case 'G':
      if (c != prev_c) {
        printf("\n");
      }
      /* go to end */
      new_pos = strings.len - 1;
      printf(" %s <<<", strings.strings[new_pos]);
      fflush(stdout);
      pos = new_pos + 1;
      break;
    case 'F':
      if (c != prev_c) {
        printf("\n");
      }
      /* go to end */
      new_pos = 0;
      printf(" >>> %s", strings.strings[new_pos]);
      fflush(stdout);
      pos = new_pos + 1;
      break;
    case '-':
      if (c != prev_c) {
        printf("\n------------------------------------------------------------\n");
      }
      break;
    case '\n':
    case '\r':
      printf("\n");
      break;
    default:
      printf("\nunknown code: %x / %c\n", c, c);
      break;
    }
    if (quit) {
      break;
    }
    prev_c = c;
  }
  printf("\nQUIT\n");
  tcsetattr(tty_fd, TCSANOW, &orig_termios_struct);
  exit(0);
}

