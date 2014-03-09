/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int fork_result;
  int pipes[2];
  char buff[1024];
  int ret_code;
  char message[] = "This is the message to be transmitted";
  ret_code = pipe(pipes);
  if (ret_code != 0) exit(1);
  fork_result = fork();
  if (fork_result == 0) {
    close(pipes[0]);
    ret_code = write(pipes[1], message, strlen(message));
    if (ret_code < 0) exit(2);
    close(pipes[1]);
  } else {
    close(pipes[1]);
    ret_code = read(pipes[0], buff, strlen(message));
    if (ret_code < 0) exit(3);
    close(pipes[0]);
    printf("found message=\"%s\"\n", buff);
  }
  exit(0);
}
