#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main() {
  int k,m,n;
  int fdout;
  int fdin;
  const char *FILENAME = "/tmpx/out-highlevel.txt";
  const char *CONTENT = "Dies ist ein Beispieltext 2";
  char *buffer;
  n = strlen(CONTENT) + 1; // include '\000'
  fdout = open(FILENAME, O_WRONLY | O_CREAT);
  printf("fdout=%d\n", fdout);
  if (fdout < 0) {
    int myerrno = errno;
    const char *error_str = strerror(myerrno);
    printf("errno=%d\nmessage=%s\n", myerrno, error_str);
    exit(1);
  }
  /* ... */
}
