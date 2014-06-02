
/*This is a program to illustrate
 *the semaphore operations, semop(),
 *system call capabilities.
 */
/*Include necessary header files.*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <itskylib.h>

/*Start of main C language program*/
int main(int argc, char **argv) {
  int retcode;
  if (argc < 2 || argc > 4) {
    printf("USAGE:\n\n%s buffersize infile outfile\n", argv[0]);
    exit(1);
  }
  
  int buffer_size = atoi(argv[1]);
  char *buffer = malloc(buffer_size + 1);
  
  int fdin, fdout;
  if (argc >= 3 && strcmp(argv[2], "-") != 0) {
    fdin = open(argv[2], O_RDONLY |O_NONBLOCK);
    handle_error(fdin, "open", PROCESS_EXIT);
  } else {
    fdin = STDIN_FILENO;
    retcode = fcntl(fdin, F_SETFL, fcntl(fdin, F_GETFL) |O_NONBLOCK);
    handle_error(retcode, "fcntl", PROCESS_EXIT);
  }
  if (argc >= 4 && strcmp(argv[3], "-") != 0) {
    fdout = open(argv[3], O_WRONLY|O_NONBLOCK);
    handle_error(fdout, "open", PROCESS_EXIT);
  } else {
    fdout = STDOUT_FILENO;
    retcode = fcntl(fdout, F_SETFL, fcntl(fdout, F_GETFL) |O_NONBLOCK);
    handle_error(retcode, "fcntl", PROCESS_EXIT);
  }
  while (1) {
    int n = read(fdin, buffer, buffer_size);
    if (n < 1) {
      if (errno == EAGAIN) {
        fprintf(stderr, "read nothing (EAGAIN)\n");
        sleep(1);
        continue;
      } else {
        handle_error(n, "read", PROCESS_EXIT);
      }
    }
    if (n == 0) {
      fprintf(stderr, "read EOF -> done\n");
      break;
    }
    fprintf(stderr, "read %d of %d\n", n, buffer_size);
    char *ptr = buffer;
    while (n > 0) {
      int m = write(fdout, ptr, n);
      if (m < 0) {
        if (errno == EAGAIN) {
          fprintf(stderr, "wrote nothing\n");
          sleep(1);
          continue;
        } else {
          handle_error(m, "write", PROCESS_EXIT);
        }
      }
      fprintf(stderr, "wrote %d of %d\n", m, n);
      n -= m;
      ptr += m;
    }
  }
  exit(0);
} 
