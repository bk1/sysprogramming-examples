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
  struct sockaddr_in server_address; /* Square server address */
  unsigned short server_port;     /* Square server port */
  char *server_ip;                    /* Server IP address (dotted quad) */
  char input_string[13];              /* String to send to square server */
  char square_buffer[RCVBUFSIZE];     /* Buffer for square string */
  unsigned int input_string_len;      /* Length of string to square */
  int bytes_received, total_bytes_received;   /* Bytes read in single recv() 
                                      and total bytes read */

  if (argc < 3 || argc > 4) {    /* Test for correct number of arguments */
    usage(argv[0], "wrong number of arguments");
  }

  server_ip = argv[1];             /* First arg: server IP address (dotted quad) */
  int x = atoi(argv[2]);         /* Second arg: string to square */
  sprintf(input_string, "%12d", x); /* make sure no non-numeric characters are around */

  if (argc == 4) {
    server_port = atoi(argv[3]); /* Use given port, if any */
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

  input_string_len = strlen(input_string);          /* Determine input length */

  /* Send the string to the server */
  int count = send(sock, input_string, input_string_len, 0);
  if (count != input_string_len) {
    die_with_error("send() sent a different number of bytes than expected");
  }

  /* Receive the same string containing the square back from the server */
  total_bytes_received = 0;
  printf("Received: ");                /* Setup to print the squared string */
  char *ptr = square_buffer;
  while (total_bytes_received < input_string_len) {
      /* Receive up to the buffer size (minus 1 to leave space for
         a null terminator) bytes from the sender */
    bytes_received = recv(sock, ptr, RCVBUFSIZE - 1 - total_bytes_received, 0);
    if (bytes_received <= 0) {
      die_with_error("recv() failed or connection closed prematurely");
    }
    total_bytes_received += bytes_received;   /* Keep tally of total bytes */
    ptr += bytes_received;
    square_buffer[bytes_received] = '\0';  /* Terminate the string! */
  }
  int y = atoi(square_buffer);
  printf("x=%d y=x*x=%d\n", x, y);    /* Print the result and a final linefeed */

  close(sock);
  exit(0);
}
