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
#include <netdb.h>
#include <stdio.h>      /* for printf() and fprintf() and ... */
#include <stdlib.h>     /* for atoi() and exit() and ... */
#include <string.h>     /* for memset() and ... */
#include <sys/socket.h> /* for socket(), bind(), recv, send(), and connect() */
#include <sys/types.h>
#include <unistd.h>     /* for close() */
#include <pthread.h>
#include <signal.h>
#include <syslog.h>
#include <sys/stat.h>

#include <itskylib.h>
#include <transmission-protocols.h>

#define RCVBUFSIZE 32   /* Size of receive buffer */
#define MAXPENDING 5    /* Maximum outstanding connection requests */

pthread_attr_t attr;

struct thread_arg {
  int socket;
  int family;
};

char lock_file[300];

void exit_handler() {
  syslog(LOG_NOTICE, "exit_handler: removing lock_file=%s", lock_file);
  unlink(lock_file);
  syslog(LOG_NOTICE, "exit_handler: exiting");
}

void signal_handler(int signo) {
  if (signo == SIGUSR1 || signo == SIGUSR2) {
    /* ignore */
    return;
  }
  syslog(LOG_NOTICE, "signal_handler: exiting");
  exit_handler();
  exit(0);
}

void usage(const char *argv0, const char *msg) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("Usage\n\n");
  printf("%s start [<Server (hostname)>] [<Port>] [<IPVersion>]\n", argv0);
  printf("2nd argument is the name or IP of the server.  Needs to be \"owned\" by the server running this program. Defaults to localhost.\n");
  printf("3rd argument is the port.  Optional, defaults to 7000\n");
  printf("4th argument is the Version of IP (4 or 6).  Optional, defaults to trying both.\n");
  printf("\n");
  printf("%s status\n", argv0);
  printf("\n");
  printf("%s stop\n", argv0);
  printf("\n");
  printf("uses a protocol for communication\n");
  printf("square calculation server for TCP/IP as example of a daemon\n");
  exit(1);
}

void *handle_connection(void *args);

void *handle_tcp_client(void *client_socket_ptr);   /* TCP client handling function */

int get_daemon_pid() {
  int fd = open(lock_file, O_RDONLY);
  if (fd < 0 && errno == ENOENT) {
    return -1;
  } else if (fd < 0) {
    handle_error(fd, "open", PROCESS_EXIT);
    return -1; // just to please the compiler
  } else {
    char buf[21];
    int n = read(fd, buf, 20);
    handle_error(n, "read", PROCESS_EXIT);
    buf[n] = 0;
    int pid = atoi(buf);
    int retcode = kill(pid, 0);
    printf("pid=%d retcode of kill(pid, 0)=%d\n", pid, retcode);
    if (retcode < 0 && errno == ESRCH) {
      unlink(lock_file);
      return -1;
    } else {
      return pid;
    }
  }
}
    
void show_status() {
  int pid = get_daemon_pid();
  if (pid < 0) {
    printf("not running\n");
  } else {
    printf("running (process %d)\n", pid);
  }
}

void stop_daemon() {
  int pid = get_daemon_pid();
  if (pid < 0) {
    printf("not running\n");
  } else {
    printf("stopping (process %d)\n", pid);
    int retcode = kill(pid, SIGTERM);
    handle_error(retcode, "stopping daemon with SIGTERM", PROCESS_EXIT);
    sleep(1);
    printf("status after stopping:\n");
    show_status();
  }
}

int main(int argc, char *argv[]) {

  int retcode;

  if (is_help_requested(argc, argv)) {
    usage(argv[0], "");
  }

  if (argc < 2 || argc > 5) {    /* Test for correct number of arguments */
    usage(argv[0], "wrong number of arguments");
  }

  char tmp_name[strlen(argv[0])+1];
  strcpy(tmp_name, argv[0]);
  char *argv0_without_path = tmp_name;
  for (int i = 0; i < strlen(tmp_name); i++) {
    if (tmp_name[i] == '/') {
      argv0_without_path = tmp_name + i + 1;
    }
  }
  if (getuid() == 0) {
    sprintf(lock_file, "/var/run/%s.pid", argv0_without_path);
  } else {
    sprintf(lock_file, "/var/tmp/%s.pid", argv0_without_path);
  }

  printf("lock_file=%s\n", lock_file);

  if (strcmp(argv[1], "stop") == 0) {
    stop_daemon();
    exit(0);
  } else if (strcmp(argv[1], "status") == 0) {
    show_status();
    exit(0);
  } else if (strcmp(argv[1], "start") != 0) {
    usage(argv[0], "unsupported value for first argument: only start|stop|status");
  }

  int old_pid = get_daemon_pid();
  if (old_pid > 0) {
    printf("already running (process %d)\n", old_pid);
    exit(0);
  }

  int pid = fork();
  if (pid < 0) {
    handle_error(pid, "could not fork child from parent\n", PROCESS_EXIT);
  } else if (pid > 0) {
    int ppid = getpid();
    printf("in parent: pid=%ld pgid=%ld sid=%ld, child started with pid=%ld\nexiting...\n", (long) ppid, (long) getpgid(ppid), (long) getsid(ppid), (long) pid);
    exit(0);
  }

  retcode = setsid();
  handle_error(retcode, "setsid", PROCESS_EXIT);
  retcode = umask(000);
  handle_error(retcode, "umask", PROCESS_EXIT);
  retcode = chdir("/");
  handle_error(retcode, "chdir", PROCESS_EXIT);
  
  /* In child */
  pid = fork();
  if (pid < 0) {
    handle_error(pid, "could not fork child from parent\n", PROCESS_EXIT);
  } else if (pid > 0) {
    int cpid = getpid();
    printf("in child: pid=%d pgid=%d sid=%d, daemon started with pid=%d\nexiting...\n", cpid, getpgid(cpid), getsid(cpid), pid);
    exit(0);
  }
  
  int lock_fd = open(lock_file, O_CREAT | O_EXCL | O_WRONLY, 0600);
  handle_error(lock_fd, "open", PROCESS_EXIT);
  printf("lock_file=%s lock_fd=%d\n", lock_file, lock_fd);
  retcode = atexit(exit_handler);
  handle_error(retcode, "atexit", PROCESS_EXIT);
  char buf[21];
  sprintf(buf, "%d\n", getpid());
  retcode = write(lock_fd, buf, strlen(buf));
  handle_error(retcode, "write", PROCESS_EXIT);
  retcode = close(lock_fd);
  handle_error(retcode, "close", PROCESS_EXIT);

  openlog(argv[0], LOG_PID, LOG_USER);
  syslog(LOG_NOTICE, "%s started", argv[0]);

  retcode = close(STDIN_FILENO);
  handle_error_syslog(retcode, "close(STDIN_FILENO)", PROCESS_EXIT);
  retcode = close(STDOUT_FILENO);
  handle_error_syslog(retcode, "close(STDOUT_FILENO)", PROCESS_EXIT);
  retcode = close(STDERR_FILENO);
  handle_error_syslog(retcode, "close(STDERR_FILENO)", PROCESS_EXIT);

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  sigset_t sig_mask;
  retcode = sigemptyset(&sig_mask);
  handle_error_syslog(retcode, "sigemptyset", PROCESS_EXIT);
  retcode = sigaddset(&sig_mask, SIGPIPE);
  handle_error_syslog(retcode, "sigaddset", PROCESS_EXIT);
  retcode = sigaddset(&sig_mask, SIGTERM);
  handle_error_syslog(retcode, "sigaddset", PROCESS_EXIT);
  retcode = sigaddset(&sig_mask, SIGINT);
  handle_error_syslog(retcode, "sigaddset", PROCESS_EXIT);

  struct sigaction new_sigaction_pipe;
  struct sigaction old_sigaction_pipe;
  /* assign unused fields to null *first*, so if there is a union the real values will supersede */
  new_sigaction_pipe.sa_sigaction = NULL;
  new_sigaction_pipe.sa_restorer = NULL;
  new_sigaction_pipe.sa_handler = signal_handler;
  new_sigaction_pipe.sa_mask = sig_mask;
  new_sigaction_pipe.sa_flags = SA_NOCLDSTOP;
  retcode = sigaction(SIGPIPE, &new_sigaction_pipe, &old_sigaction_pipe);
  handle_error_syslog(retcode, "sigaction SIGPIPE", PROCESS_EXIT);

  struct sigaction new_sigaction_term = new_sigaction_pipe;
  struct sigaction old_sigaction_term;
  retcode = sigaction(SIGTERM, &new_sigaction_term, &old_sigaction_term);
  handle_error_syslog(retcode, "sigaction SIGTERM", PROCESS_EXIT);

  struct sigaction new_sigaction_int = new_sigaction_pipe;
  struct sigaction old_sigaction_int;
  retcode = sigaction(SIGINT, &new_sigaction_int, &old_sigaction_int);
  handle_error_syslog(retcode, "sigaction SIGINT", PROCESS_EXIT);
  
  char *server_name;
  if (argc >= 3) {
    server_name = argv[2];             /* First arg: server IP address (dotted quad) */
  } else {
    server_name = "localhost";
  }

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
  handle_error_syslog(retcode, "getaddrinfo() failed", PROCESS_EXIT);

  int count_successes = 0;

  /* Create a reliable, stream socket using TCP */
  struct addrinfo *rp;
  int server_socket = -1;                    /* Socket descriptor for server */
  for (rp = result; rp != NULL; rp = rp->ai_next) {
    server_socket = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (server_socket == -1) {
      // look errno
      //   handle_error_syslog(server_socket, "socket() failed", PROCESS_EXIT);

      continue;
    }
    /* Establish the connection to the square server */
    retcode = bind(server_socket, rp->ai_addr, rp->ai_addrlen);
    if (retcode == 0) {
      /* Success */
      count_successes++;
      struct thread_arg *arg_ptr = (struct thread_arg *) malloc(sizeof(struct thread_arg));
      arg_ptr->socket = server_socket;
      arg_ptr->family = rp->ai_family;
      pthread_t thread;
      retcode = pthread_create(&thread, &attr, handle_connection, arg_ptr);
      handle_thread_error_syslog(retcode, "pthread_create() for handle_connection()", PROCESS_EXIT);
    } else {
      // look errno
      // retcode = connect(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));
      // handle_error_syslog(retcode, "connect() failed", PROCESS_EXIT);
      close(server_socket);
    }
    server_socket = -1;
  }
  syslog(LOG_NOTICE, "%d connections are being handled\n", count_successes);
  if (count_successes == 0) {
    handle_error_syslog(server_socket, "no connection could be established", PROCESS_EXIT);
  }

  /* finish master thread */
  pthread_exit(NULL);
}

void *handle_connection(void *args) {

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
  handle_error_syslog(retcode, "listen() failed", PROCESS_EXIT);

  while (TRUE) { /* Run forever */
    /* Set the size of the in-out parameter */
    client_address_len = sizeof(client_address);

    /* Wait for a client to connect */
    client_socket = accept(server_socket, (struct sockaddr *) &client_address, &client_address_len);
    handle_error_syslog(client_socket, "accept() failed", PROCESS_EXIT);

    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    retcode = getnameinfo((struct sockaddr *) &client_address,
                          client_address_len, 
                          host, 
                          NI_MAXHOST,
                          service, 
                          NI_MAXSERV, 
                          NI_NUMERICSERV);
    handle_error_syslog(retcode, "getnameinfo", THREAD_EXIT);

    syslog(LOG_INFO, "Handling client %s for service=%s (%s)\n", host, service, family_s);
    /* client_socket is connected to a client! */

    pthread_t thread;
    int *client_socket_ptr = (int *) malloc(sizeof(int));
    *client_socket_ptr = client_socket;
    retcode = pthread_create(&thread, &attr, handle_tcp_client, client_socket_ptr);
    handle_thread_error_syslog(retcode, "pthread_create() for handle_tcp_client()", PROCESS_EXIT);
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
  syslog(LOG_INFO, "connection terminated by client\n");
  sleep(1);
  close(client_socket);    /* Close client socket */
  return NULL;
}
