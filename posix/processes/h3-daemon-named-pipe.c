/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2015-03-03
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */


#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

#include <itskylib.h>

void usage(char *argv[], char *msg) {
  if (msg != NULL) {
    printf("%s\n\n", msg);
  }
  printf("Usage:\n\n%s input_fifo output_file\n\nreads from input_fifo and writes squares to output_file\n\n", argv[0]);
  exit(1);
}

void find_start(char *argv[]) {
  char buf[1024];

  while (TRUE) {
    // printf("outer loop fs\n");
    int fd = open(argv[1], O_RDONLY);
    handle_error(fd, "open in daemon_process", PROCESS_EXIT);
    while (TRUE) {
      // printf("inner loop fs\n");
      int s = read(fd, buf, 1);
      buf[1] = 0;
      // printf("s=%d buf=%s\n", s, buf);
      if (s == 0) {
        break;
      }
      // printf("s=%d buf=%s\n", s, buf);
      handle_error(s, "read in find_start", PROCESS_EXIT);
      // printf("s=%d buf=%s\n", s, buf);
      if (strcmp(buf, "S") == 0) {
        close(fd);
        return;
      }
    }
    close(fd);
  }
}

void daemon_process(char *argv[]) {
  // printf("start daemon\n");
  char buf[1025];

  int out = open(argv[2], O_WRONLY | O_CREAT, 0666);
  handle_error(out, "open write", PROCESS_EXIT);
  // printf("%s open\n", argv[2]);
  find_start(argv);
  // printf("read start\n");

  while (TRUE) {
    // printf("outer loop\n");
    int fd = open(argv[1], O_RDONLY);
    handle_error(fd, "open in daemon_process", PROCESS_EXIT);
    // printf("open\n");
    while (TRUE) {
      // printf("inner loop\n");
      int s = read(fd, buf, 1024);
      // printf("read s=%d\n", s);
      handle_error(s, "read", PROCESS_EXIT);
      if (s == 0) {
        int retcode = close(fd);
        handle_error(retcode, "close in daemon", PROCESS_EXIT);
        break;
      }
      buf[s] = 0;
      // printf("read s=%d -> %s\n", s, buf);
      for  (int i = 0; i < s; i++) {
        if (buf[i] == 'Q' || buf[i] == 'q') {
          // printf("exit daemon\n");
          exit(0);
        }
      }
      int x = atoi(buf);
      int y = x*x;
      // printf("x=%d y=%d\n", x, y);
      char outbuf[1024];
      sprintf(outbuf, "%d\n", y);
      int retcode = write(out, outbuf, strlen(outbuf));
      handle_error(retcode, "write in daemon", PROCESS_EXIT);
    }
  }
  exit(0);
}

void child(char *argv[]) {
  printf("start child\n");
  pid_t daemon_process_pid = fork();
  handle_error(daemon_process_pid, "fork() for daemon_process", PROCESS_EXIT);
  if (daemon_process_pid != 0) {
    printf("exit child\n");
    exit(0);
  } else {
    daemon_process(argv);
  }
}


void parent(char *argv[], pid_t child_pid) {
  printf("start parent\n");
  int status;
  // printf("parent waiting for child_pid=%d\n", child_pid);
  pid_t pid = waitpid(child_pid, &status, 0);
  printf("parent child-pid=%d child=%d status=%d\n", (int) pid, (int) child_pid, (int) status);
  handle_error(pid, "waitpid", PROCESS_EXIT);
  int fd = open(argv[1], O_WRONLY, 0);
  handle_error(fd, "open in parent", PROCESS_EXIT);
  printf("parent opened\n");
  int retcode = write(fd, "S", 1);
  handle_error(retcode, "write in parent", PROCESS_EXIT);
  printf("parent written\n");
  retcode  = close(fd);
  handle_error(retcode, "close in parent", PROCESS_EXIT);
  printf("parent closed\n");
  printf("end parent\n");
  exit(0);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    usage(argv, "wrong number of arguments");
  }
  if (strcmp(argv[1], "-h") ==0) {
    usage(argv, NULL);
  }
  int retcode = unlink(argv[1]);
  if (retcode == -1 && errno == ENOENT) {
    printf("file %s does not exist nothing to delete\n", argv[0]);
  } else {
    handle_error(retcode, "unlink", PROCESS_EXIT);
  }
  retcode = mkfifo(argv[1], 0666 );
  handle_error(retcode, "mkfifo", PROCESS_EXIT);

  pid_t child_pid = fork();
  handle_error(child_pid, "fork() for child", PROCESS_EXIT);
  if (child_pid == 0) {
    child(argv);
  } else {
    parent(argv, child_pid);
  }
  exit(0);
}
