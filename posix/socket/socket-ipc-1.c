/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <itskylib.h>

void usage(const char *argv0, const char *msg) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("Usage\n\n");
  printf("%s\n\tExample of creating a thread and communicating from new thread to original thread via anonymous pipe\n\n", argv0);
  exit(1);
}

int main(int argc, char *argv[]) {

  if (is_help_requested(argc, argv)) {
    usage(argv[0], "");
  }

  int socket4 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  handle_error(socket4, "socket4", PROCESS_EXIT);

  struct sockaddr addr;
  //  addr.sa_family =

    //   int retcode = bind(socket4, &addr,...)

  //int socket6 = socket(AF_INET6, SOCK_STREAM, 0);
  //handle_error(socket6, "socket6", PROCESS_EXIT);
}
