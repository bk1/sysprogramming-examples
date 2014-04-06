/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 *
 * This file is inspired by
 * http://cs.baylor.edu/~donahoo/practical/CSockets/code/UDPEchoServer.c
 */

#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() and inet_ntoa() */
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>      /* for printf() and fprintf() and ... */
#include <stdlib.h>     /* for atoi() and exit() and ... */
#include <string.h>     /* for memset() and ... */
#include <sys/socket.h> /* for socket() and bind() */
#include <sys/types.h>
#include <unistd.h>     /* for close() */


#include <itskylib.h>

#define RCVBUFSIZE 32   /* Size of receive buffer */
#define SQUAREMAX 31     /* Longest string to square */
#define MAXPENDING 5    /* Maximum outstanding connection requests */

void usage(const char *argv0, const char *msg) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("Usage\n\n");
  printf("%s <Server Port>\n", argv0);
  exit(1);
}

int main(int argc, char *argv[]) {

  int retcode;

  if (is_help_requested(argc, argv)) {
    usage(argv[0], "");
  }

  int sock;                        /* Socket */
  struct sockaddr_in server_address; /* Local address */
  struct sockaddr_in client_address; /* Client address */
  unsigned int client_address_len;         /* Length of incoming message */
  char input_buffer[SQUAREMAX+1];        /* Buffer for square string */
  char output_buffer[SQUAREMAX+1];        /* Buffer for square string */
  unsigned short server_port;     /* Server port */
  int received_message_size;                 /* Size of received message */

  if (argc != 2) {
    /* Test for correct number of parameters */
    usage(argv[0], "wrong number of arguments");
  }

  server_port = atoi(argv[1]);  /* First arg:  local port */

  /* Create socket for sending/receiving datagrams */
  sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  handle_error(sock, "socket() failed", PROCESS_EXIT);

  /* Construct local address structure */
  memset(&server_address, 0, sizeof(server_address));   /* Zero out structure */
  server_address.sin_family = AF_INET;                /* Internet address family */
  server_address.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
  server_address.sin_port = htons(server_port);      /* Local port */

  /* Bind to the local address */
  retcode = bind(sock, (struct sockaddr *) &server_address, sizeof(server_address));
  handle_error(retcode, "bind() failed", PROCESS_EXIT);

  while (TRUE) {
    /* Run forever */

    /* Set the size of the in-out parameter */
    client_address_len = sizeof(client_address);

    /* Block until receive message from a client */
    received_message_size = recvfrom(sock, input_buffer, SQUAREMAX, 0, (struct sockaddr *) &client_address, &client_address_len);
    handle_error(received_message_size, "recvfrom() failed", PROCESS_EXIT);
    printf("Handling client %s\n", inet_ntoa(client_address.sin_addr));
    int x = atoi(input_buffer);
    int y = x*x;
    sprintf(output_buffer, "%12d", y);
    size_t send_message_size = strlen(output_buffer) + 1;
    /* Send received datagram back to the client */
    ssize_t sent_size = sendto(sock, output_buffer, send_message_size, 0, (struct sockaddr *) &client_address, sizeof(client_address));
    if (sent_size != send_message_size) {
      printf("sent_size=%ld send_message_size=%ld\n", sent_size, send_message_size);
      die_with_error("sendto() sent a different number of bytes than expected");
    }
  }
  /* NOT REACHED: */
  exit(0);
}
