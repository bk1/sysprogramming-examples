/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 *
 * This file is inspired by http://cs.baylor.edu/~donahoo/practical/CSockets/code/HandleTCPClient.c
 */

#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdio.h>      /* for printf() and fprintf() and ... */
#include <stdlib.h>     /* for atoi() and exit() and ... */
#include <string.h>     /* for memset() and ... */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <sys/types.h>
#include <unistd.h>     /* for close() */

//       int getaddrinfo(const char *node, const char *service,
//                       const struct addrinfo *hints,
//                       struct addrinfo **res);

//       void freeaddrinfo(struct addrinfo *res);

//       const char *gai_strerror(int errcode);

#include <itskylib.h>
#include <transmission-protocols.h>

#define RCVBUFSIZE 32   /* Size of receive buffer */


void usage(const char *argv0, const char *msg) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("Usage\n\n");
  printf("%s <Server (hostname)> [<Port>] [<IPVersion>]\n", argv0);
  printf("1st argument is the name of the server\n");
  printf("2nd argument is the port.  Optional, defaults to 7000\n");
  printf("3rd argument is the Version of IP (4 or 6).  Optional, defaults to trying both.\n");
  printf("runs in an infinite loop reading input and sending it to the server and printing the result\n");
  printf("end with Ctrl-C or QUIT\n");
  exit(1);
}

int main(int argc, char *argv[]) {

  int retcode;

  if (is_help_requested(argc, argv)) {
    usage(argv[0], "");
  }

  if (argc < 2 || argc > 4) {    /* Test for correct number of arguments */
    usage(argv[0], "wrong number of arguments");
  }

  char *server_name = argv[1];             /* First arg: server IP address (dotted quad) */

  char service_or_server_port[1024];
  if (argc >= 3) {
    sprintf(service_or_server_port, "%s", argv[2]);
  } else {
    sprintf(service_or_server_port, "7000");
  }

  int ip_version = -1;
  if (argc >= 4) {
    ip_version = atoi(argv[3]);
  }

  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  if (ip_version == 4) {
    hints.ai_family = AF_INET;
  } else if (ip_version == 6) {
    hints.ai_family = AF_INET6;
  }
  hints.ai_socktype = SOCK_STREAM;
  struct addrinfo *result;

  retcode = getaddrinfo(server_name, service_or_server_port, &hints, &result);
  handle_error(retcode, "getaddrinfo() failed", PROCESS_EXIT);

  /* Create a reliable, stream socket using TCP */
  struct addrinfo *rp;
  int sock = -1;                        /* Socket descriptor */
  for (rp = result; rp != NULL; rp = rp->ai_next) {
    sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (sock == -1) {
      // look errno
      //   handle_error(sock, "socket() failed", PROCESS_EXIT);

      continue;
    }
    /* Establish the connection to the square server */
    retcode = connect(sock, rp->ai_addr, rp->ai_addrlen);
    if (retcode == 0) {
      break;                  /* Success */
    }
    // look errno
    // retcode = connect(sock, (struct sockaddr *) &server_address, sizeof(server_address));
    // handle_error(retcode, "connect() failed", PROCESS_EXIT);
    close(sock);
    sock = -1;
  }
  handle_error(sock, "no connection could be established", PROCESS_EXIT);

  char *buffer_ptr[1];

  while (TRUE) {
    char line[4096];
    printf("x=");
    fflush(stdout);
    fgets(line, 4096, stdin);
    int n = strlen(line);
    /* ignore empty line */
    if (n == 0) {
      continue;
    }
    if (n > 0 && line[n-1] == '\n') {
      line[n-1] = '\000';
    }
    if (line[0] == '\003' || line[0] == '\004' || strcmp(line, "QUIT") == 0) {
      printf("terminated by user\n");
      break;
    }
    int x = atoi(line);
    sprintf(line, "%12d", x);
    write_string(sock, line, -1);
    
    size_t ulen = read_and_store_string(sock, buffer_ptr);
    if (ulen == 0) {
      printf("terminated by server\n");
      break;
    }
    char *buffer = *buffer_ptr;
    int y = atoi(buffer);
    printf("x=%d y=x*x=%d\n", x, y);    /* Print the result and a final linefeed */
    free(buffer);
  }
  write_eot(sock);
  printf("sent EOT\n");
  read_and_store_string(sock, buffer_ptr);
  printf("received EOT\n");
  sleep(1);
  close(sock);
  exit(0);
}
