/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 *
 * This file is inspired by http://cs.baylor.edu/~donahoo/practical/CSockets/code/UDPEchoClient.c
 */

#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdio.h>      /* for printf() and fprintf() and ... */
#include <stdlib.h>     /* for atoi() and exit() and ... */
#include <string.h>     /* for memset() and ... */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <sys/types.h>
#include <unistd.h>     /* for close() */

#include <itskylib.h>

#define RCVBUFSIZE 32   /* Size of receive buffer */
#define SQUAREMAX 31     /* Longest string to square */

void usage(const char *argv0, const char *msg) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("Usage\n\n");
  printf("%s <Server IP> <number> [<Port>]\n", argv0);
  exit(1);
}

int main(int argc, char *argv[]) {

  // int retcode;

  if (is_help_requested(argc, argv)) {
    usage(argv[0], "");
  }

  int sock;                        /* Socket descriptor */
  struct sockaddr_in squareServAddr; /* Square server address */
  struct sockaddr_in fromAddr;     /* Source address of square */
  unsigned short squareServPort;     /* Square server port */
  unsigned int fromSize;           /* In-out of address size for recvfrom() */
  char *servIP;                    /* IP address of server */
  char squareString[SQUAREMAX+1];                /* String to send to square server */
  char squareBuffer[SQUAREMAX+1];      /* Buffer for receiving squared string */
  int squareStringLen;               /* Length of string to square */
  int respStringLen = 0;               /* Length of received response */

  if (argc < 3 || argc > 4) {
    /* Test for correct number of arguments */
    usage(argv[0], "wrong number of arguments");
  }

  servIP = argv[1];           /* First arg: server IP address (dotted quad) */

  squareStringLen = strlen(argv[2]);
  if (squareStringLen > SQUAREMAX) {
    /* Check input length */
    usage(argv[0], "input word too long");
  }
  int x = atoi(argv[2]);       /* Second arg: string to square */
  sprintf(squareString, "%12d", x);
  squareStringLen = strlen(squareString);
  if (squareStringLen > SQUAREMAX) {
    /* Check input length */
    die_with_error("input word too long");
  }

  if (argc == 4) {
    squareServPort = atoi(argv[3]);  /* Use given port, if any */
  } else {
    squareServPort = 7;  /* 7 is the well-known port for the square service */
  }

  /* Create a datagram/UDP socket */
  sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  handle_error((long) socket, "socket() failed", PROCESS_EXIT);

  /* Construct the server address structure */
  memset(&squareServAddr, 0, sizeof(squareServAddr));    /* Zero out structure */
  squareServAddr.sin_family = AF_INET;                 /* Internet addr family */
  squareServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
  squareServAddr.sin_port   = htons(squareServPort);     /* Server port */

  /* Send the string to the server */
  int sendStringLen = squareStringLen + 1;
  ssize_t sentLen = sendto(sock, squareString, sendStringLen, 0, (struct sockaddr *) &squareServAddr, sizeof(squareServAddr));
  if (sentLen != sendStringLen) {
    printf("sentLen=%ld sendStringLen=%ld\n", (long) respStringLen, (long) sendStringLen);
    die_with_error("sendto() sent a different number of bytes than expected");
  }

  /* Recv a response */
  fromSize = sizeof(fromAddr);
  respStringLen = recvfrom(sock, squareBuffer, SQUAREMAX, 0, (struct sockaddr *) &fromAddr, &fromSize);
  if (respStringLen != sendStringLen) {
    printf("respStringLen=%ld sendStringLen=%ld\n", (long) respStringLen, (long) sendStringLen);
    die_with_error("recvfrom() failed");
  }
  if (squareServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr) {
    die_with_error("Error: received a packet from unknown source.");
  }

  /* null-terminate the received data */
  squareBuffer[respStringLen] = '\0';
  int y = atoi(squareBuffer);
  printf("x=%d y=x*x=%d\n", x, y);    /* Print the result and a final linefeed */
    
  close(sock);
  exit(0);
}
