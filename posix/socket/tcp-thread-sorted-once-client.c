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

#define RCVBUFSIZE 4096   /* Size of receive buffer */

void consume(const char *buff, size_t count);

void usage(const char *argv0, const char *msg) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("Usage\n\n");
  printf("%s <Server IP> [<Port>]\n", argv0);
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
  char receive_buffer[RCVBUFSIZE];     /* Buffer for received string */

  if (argc < 2 || argc > 3) {    /* Test for correct number of arguments */
    usage(argv[0], "wrong number of arguments");
  }

  server_ip = argv[1];             /* First arg: server IP address (dotted quad) */

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

  const char *cmd_string = "GETX";
  /* Send the string to the server */
  write_4byte_string(sock, cmd_string);

  /* Receive the same string containing the square back from the server */
  printf("Received: \n");                /* Setup to print the squared string */
  while (TRUE) {

    size_t ulen = read_string_fragmentable(sock, receive_buffer, RCVBUFSIZE, consume);
    if (ulen == 0) {
      break;
    }
  }

  const char *done_string = "DONE";
  write_4byte_string(sock, done_string);
  close(sock);
  printf("\nDONE\n");
  exit(0);
}

void consume(const char *buff, size_t count) {
  write(STDOUT_FILENO, buff, count);
}
