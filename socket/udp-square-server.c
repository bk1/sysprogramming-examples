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
  struct sockaddr_in squareServAddr; /* Local address */
  struct sockaddr_in squareClntAddr; /* Client address */
  unsigned int cliAddrLen;         /* Length of incoming message */
  char inputBuffer[SQUAREMAX+1];        /* Buffer for square string */
  char outputBuffer[SQUAREMAX+1];        /* Buffer for square string */
  unsigned short squareServPort;     /* Server port */
  int recvMsgSize;                 /* Size of received message */

  if (argc != 2) {
    /* Test for correct number of parameters */
    usage(argv[0], "wrong number of arguments");
  }

  squareServPort = atoi(argv[1]);  /* First arg:  local port */

  /* Create socket for sending/receiving datagrams */
  sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  handle_error(sock, "socket() failed", PROCESS_EXIT);

  /* Construct local address structure */
  memset(&squareServAddr, 0, sizeof(squareServAddr));   /* Zero out structure */
  squareServAddr.sin_family = AF_INET;                /* Internet address family */
  squareServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
  squareServAddr.sin_port = htons(squareServPort);      /* Local port */

  /* Bind to the local address */
  retcode = bind(sock, (struct sockaddr *) &squareServAddr, sizeof(squareServAddr));
  handle_error(retcode, "bind() failed", PROCESS_EXIT);

  while (TRUE) {
    /* Run forever */

    /* Set the size of the in-out parameter */
    cliAddrLen = sizeof(squareClntAddr);

    /* Block until receive message from a client */
    recvMsgSize = recvfrom(sock, inputBuffer, SQUAREMAX, 0, (struct sockaddr *) &squareClntAddr, &cliAddrLen);
    handle_error(recvMsgSize, "recvfrom() failed", PROCESS_EXIT);
    printf("Handling client %s\n", inet_ntoa(squareClntAddr.sin_addr));
    int x = atoi(inputBuffer);
    int y = x*x;
    sprintf(outputBuffer, "%12d", y);
    size_t sendMsgSize = strlen(outputBuffer) + 1;
    /* Send received datagram back to the client */
    ssize_t sentSize = sendto(sock, outputBuffer, sendMsgSize, 0, (struct sockaddr *) &squareClntAddr, sizeof(squareClntAddr));
    if (sentSize != sendMsgSize) {
      printf("sentSize=%ld sendMsgSize=%ld\n", sentSize, sendMsgSize);
      die_with_error("sendto() sent a different number of bytes than expected");
    }
  }
  /* NOT REACHED: */
  exit(0);
}
