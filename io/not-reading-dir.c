#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main() {
  int k,m,n,r;
  int fdout;
  int fdin;
  const char *DIRNAME = "/tmp/test-dir-9876";
  char *buffer = (char *) malloc(1000);
  r = mkdir(DIRNAME, 0777);
  if (r < 0) {
    int myerrno = errno;
    if (myerrno != EEXIST) {
      const char *error_str = strerror(myerrno);
      printf("errno=%d\nmessage=%s\n", myerrno, error_str);
      exit(1);
    }
  }
  fdin = open(DIRNAME, O_RDONLY);
  printf("fdin=%d\n", fdin);
  if (fdin < 0) {
    int myerrno = errno;
    const char *error_str = strerror(myerrno);
    printf("errno=%d\nmessage=%s\n", myerrno, error_str);
    exit(1);
  }
  k = read(fdin, (void *) buffer, 999);
  printf("k=%d\n", k);
  if (k < 0) {
    int myerrno = errno;
    const char *error_str = strerror(myerrno);
    printf("errno=%d\nmessage=%s\n", myerrno, error_str);
    exit(1);
  }
  r = close(fdin);
  printf("r=%d\n", r);
  if (r < 0) {
    int myerrno = errno;
    const char *error_str = strerror(myerrno);
    printf("errno=%d\nmessage=%s\n", myerrno, error_str);
    exit(1);
  }
  buffer[999] = (char) 0;
  printf("%d bytes read:\n%s\n", k, buffer);
}
