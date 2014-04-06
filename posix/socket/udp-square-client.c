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
  struct sockaddr_in server_address; /* Square server address */
  struct sockaddr_in from_addr;     /* Source address of square */
  unsigned short server_port;     /* Square server port */
  unsigned int from_size;           /* In-out of address size for recvfrom() */
  char *server_ip;                    /* IP address of server */
  char square_string[SQUAREMAX+1];                /* String to send to square server */
  char square_buffer[SQUAREMAX+1];      /* Buffer for receiving squared string */
  int square_string_len;               /* Length of string to square */
  int resp_string_len = 0;               /* Length of received response */

  if (argc < 3 || argc > 4) {
    /* Test for correct number of arguments */
    usage(argv[0], "wrong number of arguments");
  }

  server_ip = argv[1];           /* First arg: server IP address (dotted quad) */

  square_string_len = strlen(argv[2]);
  if (square_string_len > SQUAREMAX) {
    /* Check input length */
    usage(argv[0], "input word too long");
  }
  int x = atoi(argv[2]);       /* Second arg: string to square */
  sprintf(square_string, "%12d", x);
  square_string_len = strlen(square_string);
  if (square_string_len > SQUAREMAX) {
    /* Check input length */
    die_with_error("input word too long");
  }

  if (argc == 4) {
    server_port = atoi(argv[3]);  /* Use given port, if any */
  } else {
    server_port = 7;  /* 7 is the well-known port for the square service */
  }

  /* Create a datagram/UDP socket */
  sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  handle_error((long) socket, "socket() failed", PROCESS_EXIT);

  /* Construct the server address structure */
  memset(&server_address, 0, sizeof(server_address));    /* Zero out structure */
  server_address.sin_family = AF_INET;                 /* Internet addr family */
  server_address.sin_addr.s_addr = inet_addr(server_ip);  /* Server IP address */
  server_address.sin_port   = htons(server_port);     /* Server port */

  /* Send the string to the server */
  int send_string_len = square_string_len + 1;
  ssize_t sent_len = sendto(sock, square_string, send_string_len, 0, (struct sockaddr *) &server_address, sizeof(server_address));
  if (sent_len != send_string_len) {
    printf("sent_len=%ld send_string_len=%ld\n", (long) resp_string_len, (long) send_string_len);
    die_with_error("sendto() sent a different number of bytes than expected");
  }

  /* Recv a response */
  from_size = sizeof(from_addr);
  resp_string_len = recvfrom(sock, square_buffer, SQUAREMAX, 0, (struct sockaddr *) &from_addr, &from_size);
  if (resp_string_len != send_string_len) {
    printf("resp_string_len=%ld send_string_len=%ld\n", (long) resp_string_len, (long) send_string_len);
    die_with_error("recvfrom() failed");
  }
  if (server_address.sin_addr.s_addr != from_addr.sin_addr.s_addr) {
    die_with_error("Error: received a packet from unknown source.");
  }

  /* null-terminate the received data */
  square_buffer[resp_string_len] = '\0';
  int y = atoi(square_buffer);
  printf("x=%d y=x*x=%d\n", x, y);    /* Print the result and a final linefeed */
    
  close(sock);
  exit(0);
}
