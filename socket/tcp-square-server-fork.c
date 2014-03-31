/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 *
 * This file is inspired by
 * http://cs.baylor.edu/~donahoo/practical/CSockets/code/TCPEchoServer.c
 * and
 * http://cs.baylor.edu/~donahoo/practical/CSockets/code/HandleTCPClient.c
 */

#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() and inet_ntoa() */
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>      /* for printf() and fprintf() and ... */
#include <stdlib.h>     /* for atoi() and exit() and ... */
#include <string.h>     /* for memset() and ... */
#include <sys/socket.h> /* for socket(), bind(), recv, send(), and connect() */
#include <sys/types.h>
#include <unistd.h>     /* for close() */

#include <itskylib.h>

#define RCVBUFSIZE 32   /* Size of receive buffer */
#define MAXPENDING 5    /* Maximum outstanding connection requests */

void usage(const char *argv0, const char *msg) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("Usage\n\n");
  printf("%s <Server Port>\n", argv0);
  exit(1);
}

void handle_tcp_client(int clntSocket);   /* TCP client handling function */

int main(int argc, char *argv[]) {

  int retcode;

  if (is_help_requested(argc, argv)) {
    usage(argv[0], "");
  }

  int servSock;                    /* Socket descriptor for server */
  int clntSock;                    /* Socket descriptor for client */
  struct sockaddr_in squareServAddr; /* Local address */
  struct sockaddr_in squareClntAddr; /* Client address */
  unsigned short squareServPort;     /* Server port */
  unsigned int clntAddrLen;            /* Length of client address data structure */

  if (argc != 2) {
    /* Test for correct number of arguments */
    usage(argv[0], "wrong number of arguments");
  }
  squareServPort = atoi(argv[1]);  /* First arg:  local port */

  /* Create socket for incoming connections */
  servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  handle_error(servSock, "socket() failed", PROCESS_EXIT);

  /* Construct local address structure */
  memset(&squareServAddr, 0, sizeof(squareServAddr));   /* Zero out structure */
  squareServAddr.sin_family = AF_INET;                /* Internet address family */
  squareServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
  squareServAddr.sin_port = htons(squareServPort);      /* Local port */

  /* Bind to the local address */
  retcode = bind(servSock, (struct sockaddr *) &squareServAddr, sizeof(squareServAddr));
  handle_error(retcode, "bind() failed", PROCESS_EXIT);

  /* Mark the socket so it will listen for incoming connections */
  retcode = listen(servSock, MAXPENDING);
  handle_error(retcode, "listen() failed", PROCESS_EXIT);

  while (TRUE) { /* Run forever */
    /* Set the size of the in-out parameter */
    clntAddrLen = sizeof(squareClntAddr);

    /* Wait for a client to connect */
    clntSock = accept(servSock, (struct sockaddr *) &squareClntAddr, &clntAddrLen);
    handle_error(clntSock, "accept() failed", PROCESS_EXIT);

    /* clntSock is connected to a client! */

    pid_t pid = fork();
    handle_error(pid, "fork() after accept", PROCESS_EXIT);
    if (pid == 0) {
      printf("Handling client %s\n", inet_ntoa(squareClntAddr.sin_addr));
      handle_tcp_client(clntSock);
    }
  }
  /* NOT REACHED: */
  exit(0);
}

void handle_tcp_client(int clntSocket) {
  char squareBuffer[RCVBUFSIZE];      /* Buffer for square string */
  int recvMsgSize;                    /* Size of received message */

  while (TRUE) {
    /* Receive message from client */
    recvMsgSize = recv(clntSocket, squareBuffer, RCVBUFSIZE - 1, 0);
    handle_error(recvMsgSize, "recv() failed", PROCESS_EXIT);

    if (recvMsgSize == 0) {
      /* zero indicates end of transmission */
      break;
    }
    squareBuffer[recvMsgSize] = '\000';
    /* Send received string and receive again until end of transmission */
    /* Square message and send it back to client */
    int x = atoi(squareBuffer);
    int y = x*x;
    sprintf(squareBuffer, "%12d", y);
    int sendMsgSize = strlen(squareBuffer);
    ssize_t sentSize = send(clntSocket, squareBuffer, sendMsgSize, 0);
    if (sentSize != recvMsgSize) {
      die_with_error("send() failed");
    }
    /* See if there is more data to receive in the next round...*/
  }

  close(clntSocket);    /* Close client socket */
}
