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
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <stdio.h>      /* for printf() and fprintf() and ... */
#include <stdlib.h>     /* for atoi() and exit() and ... */
#include <string.h>     /* for memset() and ... */
#include <sys/socket.h> /* for socket(), bind(), recv, send(), and connect() */
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>     /* for close() */

#include <itskylib.h>
#include <transmission-protocols.h>

#define RCVBUFSIZE 32   /* Size of receive buffer */
#define MAXPENDING 5    /* Maximum outstanding connection requests */


struct thread_arg {
  int socket;
  int family;
};

void signal_handler(int signo) {
  printf("closed\n");
}

void usage(const char *argv0, const char *msg) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("Usage\n\n");
  printf("%s <Server (hostname)> [<Port>] [<IPVersion>]\n", argv0);
  printf("1st argument is the name or IP of the server.  Needs to be \"owned\" by the server running this program\n");
  printf("2nd argument is the port.  Optional, defaults to 7000\n");
  printf("3rd argument is the Version of IP (4 or 6).  Optional, defaults to trying both.\n");
  printf("\n");
  printf("uses a protocol for communication\n");
  exit(1);
}

void handle_connection(void *args);

void handle_tcp_client(void *client_socket_ptr);   /* TCP client handling function */

int main(int argc, char *argv[]) {

  int retcode;

  if (is_help_requested(argc, argv)) {
    usage(argv[0], "");
  }

  if (argc < 2 || argc > 4) {    /* Test for correct number of arguments */
    usage(argv[0], "wrong number of arguments");
  }

  /* set pgid to pid */
  pid_t pid = getpid();
  retcode = setpgid(pid, pid);
  handle_error(retcode, "setpgid", PROCESS_EXIT);

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

  char *server_name = argv[1];             /* First arg: server IP address (dotted quad) */

  char service_or_server_port[1024];
  if (argc >= 3) {
    sprintf(service_or_server_port, "%s", argv[2]);
  } else {
    sprintf(service_or_server_port, "7000");
  }

  int ip_version = -1;
  if (argc >= 4) {
    ip_version = atoi(argv[3]);
  }

  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  if (ip_version == 4) {
    hints.ai_family = AF_INET;
  } else if (ip_version == 6) {
    hints.ai_family = AF_INET6;
  }
  hints.ai_socktype = SOCK_STREAM;
  struct addrinfo *result;

  retcode = getaddrinfo(server_name, service_or_server_port, &hints, &result);
  handle_error(retcode, "getaddrinfo() failed", PROCESS_EXIT);

  /* needs to wait for all children in the end */
  int count_successes = 0;

  /* Create a reliable, stream socket using TCP */
  struct addrinfo *rp;
  int server_socket = -1;                    /* Socket descriptor for server */
  for (rp = result; rp != NULL; rp = rp->ai_next) {
    server_socket = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (server_socket == -1) {
      // look errno
      //   handle_error(server_socket, "socket() failed", PROCESS_EXIT);

      continue;
    }
    /* Establish the connection to the square server */
    retcode = bind(server_socket, rp->ai_addr, rp->ai_addrlen);
    if (retcode == 0) {
      /* Success */
      struct thread_arg *arg_ptr = (struct thread_arg *) malloc(sizeof(struct thread_arg));
      arg_ptr->socket = server_socket;
      arg_ptr->family = rp->ai_family;
      pid_t child_pid = fork();
      handle_error(child_pid, "fork() for connect", PROCESS_EXIT);
      if (child_pid == 0) {
        handle_connection(arg_ptr);
        exit(0);
      } else {
        count_successes++;
      }
    }
    // look errno
    // retcode = connect(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    // handle_error(retcode, "connect() failed", PROCESS_EXIT);
    close(server_socket);
    server_socket = -1;
  }
  printf("%d connections are being handled\n", count_successes);
  if (count_successes == 0) {
    handle_error(server_socket, "no connection could be established", PROCESS_EXIT);
  }

  /* wait for child processes to finish */
  for (int i = 0; i < count_successes; i++) {
    int status;
    wait(&status);
    printf("process terminated with status=%d\n", status);
  }
  exit(0);
}

void handle_connection(void *args) {

  int retcode;

  struct thread_arg *targ = (struct thread_arg *) args;
  int server_socket = targ->socket;
  int family        = targ->family;
  char *family_s = family == AF_INET ? "IPv4" : "IPv6";
  unsigned int client_address_len;            /* Length of client address data structure */
  int client_socket;                    /* Socket descriptor for client */
  struct sockaddr_storage client_address; /* Client address */
           
  /* Mark the socket so it will listen for incoming connections */
  retcode = listen(server_socket, MAXPENDING);
  handle_error(retcode, "listen() failed", PROCESS_EXIT);
  int count_connections = 0;

  while (TRUE) { /* Run forever */
    /* Set the size of the in-out parameter */
    client_address_len = sizeof(client_address);

    /* Wait for a client to connect */
    client_socket = accept(server_socket, (struct sockaddr *) &client_address, &client_address_len);
    handle_error(client_socket, "accept() failed", PROCESS_EXIT);

    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    retcode = getnameinfo((struct sockaddr *) &client_address,
                          client_address_len, 
                          host, 
                          NI_MAXHOST,
                          service, 
                          NI_MAXSERV, 
                          NI_NUMERICSERV);
    handle_error(retcode, "getnameinfo", THREAD_EXIT);

    printf("Handling client %s for service=%s (%s)\n", host, service, family_s);
    /* client_socket is connected to a client! */

    int *client_socket_ptr = (int *) malloc(sizeof(int));
    *client_socket_ptr = client_socket;
    pid_t child_pid = fork();
    handle_error(child_pid, "fork() for client", PROCESS_EXIT);
    if (child_pid == 0) {
      handle_tcp_client(client_socket_ptr);
      exit(0);
    } else {
      count_connections++;
    }
  }
  /* NOT REACHED: */

  /* wait for child processes to finish */
  for (int i = 0; i < count_connections; i++) {
    int status;
    wait(&status);
    printf("process terminated with status=%d\n", status);
  }
  exit(0);
}

void handle_tcp_client(void *client_socket_ptr) {
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
}
