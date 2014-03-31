/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
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

#define RCVBUFSIZE 32   /* Size of receive buffer */

void usage(const char *argv0, const char *msg) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("Usage\n\n");
  printf("%s <Server IP> <number> [<Port>]\n", argv0);
  exit(1);
}

int main(int argc, char *argv[]) {

  int retcode;

  if (is_help_requested(argc, argv)) {
    usage(argv[0], "");
  }

  int sock;                        /* Socket descriptor */
  struct sockaddr_in squareServAddr; /* Square server address */
  unsigned short squareServPort;     /* Square server port */
  char *servIP;                    /* Server IP address (dotted quad) */
  char inputString[13];              /* String to send to square server */
  char squareBuffer[RCVBUFSIZE];     /* Buffer for square string */
  unsigned int inputStringLen;      /* Length of string to square */
  int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv() 
                                      and total bytes read */

  if (argc < 3 || argc > 4) {    /* Test for correct number of arguments */
    usage(argv[0], "wrong number of arguments");
  }

  servIP = argv[1];             /* First arg: server IP address (dotted quad) */
  int x = atoi(argv[2]);         /* Second arg: string to square */
  sprintf(inputString, "%12d", x); /* make sure no non-numeric characters are around */

  if (argc == 4) {
    squareServPort = atoi(argv[3]); /* Use given port, if any */
  } else {
    squareServPort = 7000;  /* 7000 is a free port */
  }

  /* Create a reliable, stream socket using TCP */
  sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  handle_error(sock, "socket() failed", PROCESS_EXIT);

  /* Construct the server address structure */
  memset(&squareServAddr, 0, sizeof(squareServAddr));     /* Zero out structure */
  squareServAddr.sin_family      = AF_INET;             /* Internet address family */
  squareServAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
  squareServAddr.sin_port        = htons(squareServPort); /* Server port: htons host to network byte order */

  /* Establish the connection to the square server */
  retcode = connect(sock, (struct sockaddr *) &squareServAddr, sizeof(squareServAddr));
  handle_error(retcode, "connect() failed", PROCESS_EXIT);

  inputStringLen = strlen(inputString);          /* Determine input length */

  /* Send the string to the server */
  int count = send(sock, inputString, inputStringLen, 0);
  if (count != inputStringLen) {
    die_with_error("send() sent a different number of bytes than expected");
  }

  /* Receive the same string containing the square back from the server */
  totalBytesRcvd = 0;
  printf("Received: ");                /* Setup to print the squared string */
  char *ptr = squareBuffer;
  while (totalBytesRcvd < inputStringLen) {
      /* Receive up to the buffer size (minus 1 to leave space for
         a null terminator) bytes from the sender */
    bytesRcvd = recv(sock, ptr, RCVBUFSIZE - 1 - totalBytesRcvd, 0);
    if (bytesRcvd <= 0) {
      die_with_error("recv() failed or connection closed prematurely");
    }
    totalBytesRcvd += bytesRcvd;   /* Keep tally of total bytes */
    ptr += bytesRcvd;
    squareBuffer[bytesRcvd] = '\0';  /* Terminate the string! */
  }
  int y = atoi(squareBuffer);
  printf("x=%d y=x*x=%d\n", x, y);    /* Print the result and a final linefeed */

  close(sock);
  exit(0);
}
