/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/* use mutex for IPC, but do preparation separately */

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

#include <itskylib.h>

#define RCVBUFSIZE 32   /* Size of receive buffer */
#define MAXPENDING 5    /* Maximum outstanding connection requests */

void usage(const char *argv0, const char *msg) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("Usage\n\n");
  printf("%s n\n", argv0);
  exit(1);
}

int sv[2];

void *server(void *arg) {
  int *iarg = (int *) arg;
  int socket = *iarg;
  char buff[RCVBUFSIZE];
  while (TRUE) {
    int size = read(socket, buff, RCVBUFSIZE - 1);
    handle_error(size, "read() failed", PROCESS_EXIT);
    if (size == 0) {
      /* zero indicates end of transmission */
      break;
    }
    buff[size] = (char) 0;
    int x = atoi(buff);
    int y = x*x;
    sprintf(buff, "%12d", y);
    ssize_t sent_size = write(socket, buff, RCVBUFSIZE - 1);
    handle_error(sent_size, "write", PROCESS_EXIT);
  }
  close(socket);
  return NULL;
}

void *client(void *arg) {
  int *iarg = (int *) arg;
  int socket = *iarg;
  char buff[RCVBUFSIZE];
  int i;
  for (i = 0; i < 100; i++) {
    sprintf(buff, "%d", i);
    int size = write(socket, buff, RCVBUFSIZE - 1);
    handle_error(size, "write() failed", PROCESS_EXIT);
    size = read(socket, buff, RCVBUFSIZE - 1);
    handle_error(size, "read() failed", PROCESS_EXIT);
    if (size == 0) {
      /* zero indicates end of transmission */
      break;
    }
    buff[size] = (char) 0;
    int y = atoi(buff);
    printf("x=%4d y=%8d\n", i, y);
  }
  close(socket);
  return NULL;
}

int main(int argc, char *argv[]) {

  int retcode;

  if (is_help_requested(argc, argv)) {
    usage(argv[0], "");
  }

  retcode = socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
  handle_error(retcode, "socketpair", PROCESS_EXIT);
    
  pthread_t server_thread;
  pthread_t client_thread;
  retcode = pthread_create(&server_thread, NULL, server, &sv[0]);
  handle_thread_error(retcode, "create server thread", PROCESS_EXIT);
  pthread_create(&client_thread, NULL, client, &sv[1]);
  handle_thread_error(retcode, "create client thread", PROCESS_EXIT);
  
  retcode = pthread_join(client_thread, NULL);
  handle_thread_error(retcode, "join client thread", PROCESS_EXIT);
  pthread_join(server_thread, NULL);
  handle_thread_error(retcode, "join server thread", PROCESS_EXIT);
}

