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
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdio.h>      /* for printf() and fprintf() and ... */
#include <stdlib.h>     /* for atoi() and exit() and ... */
#include <string.h>     /* for memset() and ... */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <sys/types.h>
#include <unistd.h>     /* for close() */

#include <itskylib.h>
#include <transmission-protocols.h>

#define RCVBUFSIZE 32   /* Size of receive buffer */

void usage(const char *argv0, const char *msg) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("Usage\n\n");
  printf("%s <Server IP> [<Port>]\n", argv0);
  printf("runs in an infinite loop reading input and sending it to the server and printing the result\n");
  printf("end with Ctrl-C or QUIT\n");
  exit(1);
}

int main(int argc, char *argv[]) {

  int retcode;

  if (is_help_requested(argc, argv)) {
    usage(argv[0], "");
  }

  int sock;                        /* Socket descriptor */
  struct sockaddr_in server_address; /* Square server address */
  unsigned short server_port;     /* Square server port */
  char *server_ip;                    /* Server IP address (dotted quad) */
  char input_string[13];              /* String to send to square server */

  if (argc < 2 || argc > 3) {    /* Test for correct number of arguments */
    usage(argv[0], "wrong number of arguments");
  }

  server_ip = argv[1];             /* First arg: server IP address (dotted quad) */
  int x = atoi(argv[2]);         /* Second arg: string to square */
  sprintf(input_string, "%12d", x); /* make sure no non-numeric characters are around */

  if (argc == 3) {
    server_port = atoi(argv[2]); /* Use given port, if any */
  } else {
    server_port = 7000;  /* 7000 is a free port */
  }

  /* Create a reliable, stream socket using TCP */
  sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  handle_error(sock, "socket() failed", PROCESS_EXIT);

  /* Construct the server address structure */
  memset(&server_address, 0, sizeof(server_address));     /* Zero out structure */
  server_address.sin_family      = AF_INET;             /* Internet address family */
  server_address.sin_addr.s_addr = inet_addr(server_ip);   /* Server IP address */
  server_address.sin_port        = htons(server_port); /* Server port: htons host to network byte order */

  /* Establish the connection to the square server */
  retcode = connect(sock, (struct sockaddr *) &server_address, sizeof(server_address));
  handle_error(retcode, "connect() failed", PROCESS_EXIT);

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
