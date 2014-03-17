#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

void *run(void *arg) {
  char *carg = (char *) arg;
  char *result = (char *) malloc(strlen(carg) * 2 + 1);
  sprintf(result, "%s%s", carg, carg);
  return result;
}


int main(int argc, char *argv[]) {
    pthread_t thread1;
    pthread_create(&thread1, NULL, run, "Hello, world\n");
    void *result;
    pthread_join(thread1, &result);
    printf("result:\n%s", (char *) result);
    exit(0);
}
