/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/*
 * library functions for transmission of strings through pipes, sockets and the like.
 * Just as an example how this could be achieved...
 */

#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() and inet_ntoa() */
#include <errno.h>
//#include <netinet/in.h>
//#include <netinet/tcp.h>
//#include <stdbool.h>
#include <stdio.h>      /* for printf() and fprintf() and ... */
#include <stdlib.h>     /* for atoi() and exit() and ... */
#include <string.h>     /* for memset() and ... */
//#include <sys/socket.h> /* for socket(), bind(), recv, send(), and connect() */
#include <sys/types.h>
#include <unistd.h>     /* for close() */

#include <itskylib.h>
#include <transmission-protocols.h>

const char *EMPTY_BUFFER = "";

/* transmit a string over a socket or pipe connnection
 * if len is given it is assumed to be the lenght of the string
 * if it is -1, the length is found out with strlen()
 * The length of string is transmitted first as 4 byte unsigned integer,
 * followed by the string itself.
 * @param client_socket  a socket or pipe.  Could be a file also.
 * @param str string to be transmitted
 * @param len length of string to be transmitted
 */
void write_string(int client_socket, char *str, size_t len) {
  if (len == -1) {
    len = strlen(str);
  }
  uint32_t ulen = (uint32_t) len;
  if (ulen != len) {
    handle_error_myerrno(-1, EFBIG, "string too long", THREAD_EXIT);
  }
  uint32_t ulen_net = htonl(ulen);
  write(client_socket, &ulen_net, sizeof(ulen_net));
  char *ptr = str;
  size_t rest_len = len;
  while (rest_len > 0) {
    size_t partial_len = write(client_socket, ptr, rest_len);
    // 
    rest_len -= partial_len;
    ptr += partial_len;
  }
}

void write_eot(int client_socket) {
  write_string(client_socket, "", 0);
}


size_t read_string_fragmentable(int client_socket, char *buffer, size_t buffer_size, consumer_function consume) {
    uint32_t ulen_net = 0;
    size_t bytes_received = read(client_socket, &ulen_net, sizeof(ulen_net));
    if (bytes_received != sizeof(ulen_net)) {
      die_with_error("recv() failed or connection closed prematurely");
    }

    uint32_t ulen = ntohl(ulen_net);
    if (ulen == 0) {
      return ulen;
    }

    size_t rest_len = ulen;
    while (rest_len > 0) {
      size_t read_len = rest_len;
      if (read_len > buffer_size) {
        read_len = buffer_size;
      }
      /* Receive up to the read_len bytes from the sender */
      bytes_received = read(client_socket, buffer, read_len);
      if (bytes_received <= 0) {
        die_with_error("recv() failed or connection closed prematurely");
      }
      consume(buffer, bytes_received);
      rest_len -= bytes_received;
    }
    return ulen;
}

size_t read_string(int client_socket, consumer_function consume) {
  char *buffer_ptr[1];
  uint32_t ulen = read_and_store_string(client_socket, buffer_ptr);
  if (ulen == 0) {
    return ulen;
  }
  char *buffer = *buffer_ptr;
  consume(buffer, ulen);
  free(buffer);
  return ulen;
}


/* the caller has to free the buffer, unless ulen == 0 */
size_t read_and_store_string(int client_socket, char **result) {
    uint32_t ulen_net = 0;
    size_t bytes_received = recv(client_socket, &ulen_net, sizeof(ulen_net), 0);
    if (bytes_received != sizeof(ulen_net)) {
      printf("recv() failed or connection closed prematurely");
      return 0;
    }

    uint32_t ulen = ntohl(ulen_net);
    // printf("ulen=%d\n");
    if (ulen == 0) {
      *result = (char *) EMPTY_BUFFER; /* actually the same as empty string */
      return ulen;
    }
    char *buffer = (char *) malloc(ulen + 1);
    buffer[ulen] = '\000';
    char *ptr = buffer;
    size_t rest_len = ulen;
    while (rest_len > 0) {
      /* Receive up to the read_len bytes from the sender */
      bytes_received = read(client_socket, ptr, rest_len);
      if (bytes_received <= 0) {
        die_with_error("recv() failed or connection closed prematurely");
      }
      rest_len -= bytes_received;
      ptr += bytes_received;
    }
    *result = buffer;
    return ulen;
}

void free_read_string(size_t ulen, char *buffer) {
  if (ulen != 0) {
    free(buffer);
  }
}

/*
 * use 4 byte long strings as a simple communication command language.
 * actually 5 bytes are transmitted, because the 0 as termination is included.
 */
void write_4byte_string(int client_socket, const char *str) {
  char buff[5] = { '\000' };
  sprintf(buff, "%s", str);
  size_t count = write(client_socket, buff, 5);
  if (count != 5) {
    die_with_error("write() sent a different number of bytes than expected");
  }
}
  
/*
 * use 4 byte long strings as a simple communication command language.
 * actually 5 bytes are transmitted, because the 0 as termination is included.
 */
void read_4byte_string(int client_socket, char *str) {
  char buff[5] = { '\000' };
  size_t count = read(client_socket, buff, 5);
  if (count != 5) {
    die_with_error("read() received a different number of bytes than expected");
  }

  memcpy(str, buff, 5);
}
