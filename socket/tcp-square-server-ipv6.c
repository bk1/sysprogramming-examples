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

void handle_tcp_client(int client_socket);   /* TCP client handling function */

int main(int argc, char *argv[]) {

  int retcode;

  if (is_help_requested(argc, argv)) {
    usage(argv[0], "");
  }

  int server_socket;                    /* Socket descriptor for server */
  int client_socket;                    /* Socket descriptor for client */
  struct sockaddr_in6 server_address; /* Local address */
  struct sockaddr_in6 client_address; /* Client address */
  unsigned short server_port;     /* Server port */
  unsigned int clnt_addr_len;            /* Length of client address data structure */

  if (argc != 2) {
    /* Test for correct number of arguments */
    usage(argv[0], "wrong number of arguments");
  }
  server_port = atoi(argv[1]);  /* First arg:  local port */

  /* Create socket for incoming connections */
  server_socket = socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP);
  handle_error(server_socket, "socket() failed", PROCESS_EXIT);

  /* Construct the server address structure */
  memset(&server_address, 0, sizeof(server_address));     /* Zero out structure */
  server_address.sin6_family = AF_INET6;             /* Internet address family */
  server_address.sin6_port   = htons(server_port); /* Server port: htons host to network byte order */
  /* server_address.sin6_flowinfo; */
  server_address.sin6_addr   = in6addr_any;
  /* server_address.sin6_scope_id; */

  /* Bind to the local address */
  retcode = bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));
  handle_error(retcode, "bind() failed", PROCESS_EXIT);

  /* Mark the socket so it will listen for incoming connections */
  retcode = listen(server_socket, MAXPENDING);
  handle_error(retcode, "listen() failed", PROCESS_EXIT);

  while (TRUE) { /* Run forever */
    /* Set the size of the in-out parameter */
    clnt_addr_len = sizeof(client_address);

    /* Wait for a client to connect */
    client_socket = accept(server_socket, (struct sockaddr *) &client_address, &clnt_addr_len);
    handle_error(client_socket, "accept() failed", PROCESS_EXIT);

    /* client_socket is connected to a client! */
    char client_addr_ipv6[100];
    //Sockets Layer Call: inet_ntop()
    inet_ntop(AF_INET6, &(client_address.sin6_addr),client_addr_ipv6, 100);
    printf("Incoming connection from client having IPv6 address: %s\n",client_addr_ipv6);
    handle_tcp_client(client_socket);
  }
  /* NOT REACHED: */
  exit(0);
}

void handle_tcp_client(int client_socket) {
  char square_buffer[RCVBUFSIZE];      /* Buffer for square string */
  int received_message_size;                    /* Size of received message */

  while (TRUE) {
    /* Receive message from client */
    received_message_size = recv(client_socket, square_buffer, RCVBUFSIZE - 1, 0);
    handle_error(received_message_size, "recv() failed", PROCESS_EXIT);

    if (received_message_size == 0) {
      /* zero indicates end of transmission */
      break;
    }
    square_buffer[received_message_size] = '\000';
    /* Send received string and receive again until end of transmission */
    /* Square message and send it back to client */
    int x = atoi(square_buffer);
    int y = x*x;
    sprintf(square_buffer, "%12d", y);
    int send_message_size = strlen(square_buffer);
    ssize_t sent_size = send(client_socket, square_buffer, send_message_size, 0);
    if (sent_size != received_message_size) {
      die_with_error("send() failed");
    }
    /* See if there is more data to receive in the next round...*/
  }

  close(client_socket);    /* Close client socket */
}
