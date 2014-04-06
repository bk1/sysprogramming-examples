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
#include <pthread.h>
#include <signal.h>

#include <itskylib.h>
#include <transmission-protocols.h>

#define RCVBUFSIZE 32   /* Size of receive buffer */
#define MAXPENDING 5    /* Maximum outstanding connection requests */


void signal_handler(int signo) {
  printf("closed\n");
}

void usage(const char *argv0, const char *msg) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("Usage\n\n");
  printf("%s <Server Port>\n", argv0);
  printf("\n");
  printf("uses a protocol for communication\n");
  exit(1);
}

void *handle_tcp_client(void *client_socket_ptr);   /* TCP client handling function */

int main(int argc, char *argv[]) {

  int retcode;

  if (is_help_requested(argc, argv)) {
    usage(argv[0], "");
  }

  sigset_t sig_mask;
  retcode = sigemptyset(&sig_mask);
  handle_error(retcode, "sigemptyset", PROCESS_EXIT);
  retcode = sigaddset(&sig_mask, SIGPIPE);
  handle_error(retcode, "sigaddset", PROCESS_EXIT);

  struct sigaction new_sigaction;
  struct sigaction old_sigaction;
  /* assign unused fields to null *first*, so if there is a union the real values will supersede */
  new_sigaction.sa_sigaction = NULL;
  new_sigaction.sa_restorer = NULL;
  new_sigaction.sa_handler = signal_handler;
  new_sigaction.sa_mask = sig_mask;
  new_sigaction.sa_flags = SA_NOCLDSTOP;
  retcode = sigaction(SIGPIPE, &new_sigaction, &old_sigaction);
  handle_error(retcode, "sigaction", PROCESS_EXIT);

  int server_socket;                    /* Socket descriptor for server */
  int client_socket;                    /* Socket descriptor for client */
  struct sockaddr_in server_address; /* Local address */
  struct sockaddr_in client_address; /* Client address */
  unsigned short server_port;     /* Server port */
  unsigned int client_address_len;            /* Length of client address data structure */

  if (argc != 2) {
    /* Test for correct number of arguments */
    usage(argv[0], "wrong number of arguments");
  }
  server_port = atoi(argv[1]);  /* First arg:  local port */

  /* Create socket for incoming connections */
  server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  handle_error(server_socket, "socket() failed", PROCESS_EXIT);

  /* Construct local address structure */
  memset(&server_address, 0, sizeof(server_address));   /* Zero out structure */
  server_address.sin_family = AF_INET;                /* Internet address family */
  server_address.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
  server_address.sin_port = htons(server_port);      /* Local port */

  /* Bind to the local address */
  retcode = bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));
  handle_error(retcode, "bind() failed", PROCESS_EXIT);

  /* Mark the socket so it will listen for incoming connections */
  retcode = listen(server_socket, MAXPENDING);
  handle_error(retcode, "listen() failed", PROCESS_EXIT);

  while (TRUE) { /* Run forever */
    /* Set the size of the in-out parameter */
    client_address_len = sizeof(client_address);

    /* Wait for a client to connect */
    client_socket = accept(server_socket, (struct sockaddr *) &client_address, &client_address_len);
    handle_error(client_socket, "accept() failed", PROCESS_EXIT);

    printf("Handling client %s\n", inet_ntoa(client_address.sin_addr));
    /* client_socket is connected to a client! */

    pthread_t thread;
    int *client_socket_ptr = (int *) malloc(sizeof(int));
    *client_socket_ptr = client_socket;
    pthread_create(&thread, NULL, handle_tcp_client, client_socket_ptr);
    pthread_detach(thread);
  }
  /* NOT REACHED: */
  exit(0);
}

void* handle_tcp_client(void *client_socket_ptr) {
  int client_socket = *((int *) client_socket_ptr);
  free(client_socket_ptr); /* malloc was made before starting this thread */
  char square_buffer[RCVBUFSIZE];      /* Buffer for square string */

  while (TRUE) {
    /* Receive message from client */
    char *buffer_ptr[1];
    // printf("reading\n");
    size_t ulen = read_and_store_string(client_socket, buffer_ptr);
    // printf("read ulen=%d\n", (int) ulen);
   if (ulen == 0) {
      break;
    }

    /* Send received string and receive again until end of transmission */
    /* Square message and send it back to client */
    char *buffer = *buffer_ptr;
    int x = atoi(buffer);
    int y = x*x;
    sprintf(square_buffer, "%12d", y);
    int send_msg_size = strlen(square_buffer);
    write_string(client_socket, square_buffer, send_msg_size);
    free_read_string(ulen, buffer);
    /* See if there is more data to receive in the next round...*/
  }

  write_eot(client_socket);
  printf("connection terminated by client\n");
  sleep(1);
  close(client_socket);    /* Close client socket */
  return NULL;
}
