
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

#include <itskylib.h>

/*Start of main C language program*/
int main(int argc, char **argv) {
  if (argc < 2 || argc > 4) {
    printf("USAGE:\n\n%s buffersize infile outfile\n", argv[0]);
    exit(1);
  }
  
  int buffer_size = atoi(argv[1]);
  char *buffer = malloc(buffer_size + 1);
  
  int fdin, fdout;
  if (argc >= 3) {
    fdin = open(argv[2], O_RDONLY);
  } else {
    fdin = STDIN_FILENO;
  }
  if (argc >= 4) {
    fdout = open(argv[3], O_WRONLY);
  } else {
    fdout = STDOUT_FILENO;
  }
  while (1) {
    int n = read(fdin, buffer, buffer_size);
    if (n == 0) {
      break;
    }
    fprintf(stderr, "read %d of %d\n", n, buffer_size);
    char *ptr = buffer;
    while (n > 0) {
      int m = write(fdout, ptr, n);
      fprintf(stderr, "wrote %d of %d\n", m, n);
      n -= m;
      ptr += m;
    }
  }
  exit(0);
} 
