/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 *
 * This file is inspired by http://cs.baylor.edu/~donahoo/practical/CSockets/code/HandleTCPClient.c
 */

#include <errno.h>
#include <stdio.h>      /* for printf() and fprintf() and ... */
#include <stdlib.h>     /* for atoi() and exit() and ... */
#include <string.h>     /* for memset() and ... */
#include <sys/types.h>
#include <unistd.h>     /* for close() */

void usage(const char *argv0, const char *msg) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("Usage\n\n");
  printf("%s x y\n", argv0);
  printf("creates closure adder which adds x to given param\n");
  printf("prints i+x for i=0..y-1\n");
  exit(1);
}

struct closure;

typedef int (*fun_type)(const struct closure *context, const int param);

struct closure {
  int x;
  fun_type fun;
};

int f(const struct closure *context, const int param) {
  return (context->x) + param;
}

struct closure *adder(int x) {
  struct closure *result = malloc(sizeof(struct closure));
  result->x = x;
  result->fun = f;
  return  result;
}

int main(int argc, char *argv[]) {

  if (argc < 2) {
    usage(argv[0], "not enough parameters");
  }
  int x = atoi(argv[1]);
  int y = atoi(argv[2]);
  struct closure *cl = adder(x);
  int i;
  for (i = 0; i < y; i++) {
    printf("cl(%d)=%d\n", i, cl->fun(cl, i));
  }
}
