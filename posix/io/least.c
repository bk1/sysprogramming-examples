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
// #include <sys/stat.h>
// #include <sys/types.h>
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

  int retcode;

  char *buff = malloc(FILE_SIZE+1);
  handle_ptr_error(buff, "malloc", PROCESS_EXIT);
  
  int size;
  if (argc == 1) {
    size = read(STDIN_FILENO, buff, FILE_SIZE);
    handle_error(size, "read stdin", PROCESS_EXIT);
  } else {
    int fd = open(argv[1], O_RDONLY);
    handle_error(fd, "open", PROCESS_EXIT);
    size = read(fd, buff, FILE_SIZE);
    handle_error(size, "read fd", PROCESS_EXIT);
    retcode = close(fd);
    handle_error(retcode, "close fd", PROCESS_EXIT);
  }
  buff[size] = '\000';

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
    char cc;
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
      new_pos = 0;
      for (int j = pos - 2; j >= 0; j--) {
        if (buff[j] == '\n') {
          new_pos = j+1;
          break;
        }
      }
      // printf("%d-%d: ", (int) pos, (int) new_pos);
      if (new_pos < pos) {
        char *line = malloc(pos-new_pos + 1);
        handle_ptr_error(line, "malloc line", PROCESS_EXIT);
        strncpy(line, buff+new_pos, pos-new_pos);
        line[pos-new_pos] = '\000';
        printf("%s", line);
        fflush(stdout);
      }
      pos = new_pos;
      break;
    case 'K': 
    case '\x0e':
      if (c != prev_c) {
        printf("\n");
      }
      new_pos = size;
      for (int j = pos; j < size-1; j++) {
        if (buff[j] == '\n') {
          new_pos = j+1;
          break;
        }
      }
      if (new_pos > pos) {
        char *line = malloc(new_pos-pos + 1);
        handle_ptr_error(line, "malloc line", PROCESS_EXIT);
        strncpy(line, buff+pos, new_pos-pos);
        line[new_pos-pos] = '\000';
        // printf("%d-%d: ", (int) pos, (int) new_pos);
        printf("%s", line);
        fflush(stdout);
      }
      pos = new_pos;
      break;
    case 'H':
    case '\x02':
      if (c != prev_c) {
        printf("\n");
      }
      if (pos > 0) {
        new_pos = pos - 1;
        printf("%c", buff[new_pos]);
        fflush(stdout);
        pos = new_pos;
      }
      break;
    case 'L':
    case '\x06':
      if (c != prev_c) {
        printf("\n");
      }
      if (pos < size) {
        new_pos = pos + 1;
        printf("%c", buff[pos]);
        fflush(stdout);
        pos = new_pos;
      }
      break;
    case 'G':
      if (c != prev_c) {
        printf("\n");
      }
      /* go to end */
      printf("%s", buff + pos);
      fflush(stdout);
      pos = size;
      break;
    case 'F':
      if (c != prev_c) {
        printf("\n");
      }
      /* go to beginning */
      cc = buff[pos];
      buff[pos] = '\000';
      printf("%s\n", buff);
      fflush(stdout);
      buff[pos] = cc;
      pos = 0;
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

