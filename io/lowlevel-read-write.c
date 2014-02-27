#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
  int fdreadwrite;
  int fdin;
  const char *IFILENAME = "lowlevel-read-write.c";
  const char *OFILENAME = "reversed.txt";
  struct stat stat_buf;
  int m = 20;
  int k;
  int n;
  int block_count;
  int i, j;
  char *buffer = malloc(m+3);
  buffer[m+2] = '\000';
  fdin  = open(IFILENAME, O_RDONLY);
  fdreadwrite = open(OFILENAME, O_RDWR | O_CREAT, 0666);
  printf("fdin=%d fdreadwrite=%d\n", fdin, fdreadwrite);
  fstat(fdin, &stat_buf);
  n = stat_buf.st_size;
  block_count = (n + m - 1) / m;
  printf("size=%d block_count=%d\n", n, block_count);
  for (i = 0; i < block_count; i++) {
    k = read(fdin, (void *) (buffer+1), m);
    if (k == 0) {
      break;
    }
    /** error handling skipped */
    /** fill up with spaces */
    for (j = k+1; j <= m; j++) {
      buffer[j] = ' ';
    }
    buffer[0] = '|';
    buffer[m+1] = '|';
    lseek(fdreadwrite, (block_count - i - 1) * (m + 2), SEEK_SET);
    write(fdreadwrite, (void *) buffer, m+2);
    /** error handling skipped */
  }
  close(fdin);
  lseek(fdreadwrite, 0, SEEK_SET);
  for (i = 0; i < block_count; i++) {
    k = read(fdreadwrite, (void *) buffer, m);
    buffer[k] = '\000';
    printf("%s", buffer);
  }
  close(fdreadwrite);
  printf("\n");
}
