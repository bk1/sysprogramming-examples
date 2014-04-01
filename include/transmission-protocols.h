/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#ifndef _LIB_ITSKY_TRANSMISSION_PROTOCOLS
#define _LIB_ITSKY_TRANSMISSION_PROTOCOLS

typedef void (*consumer_function)(const char *buff, size_t count);


/* transmit a string over a socket or pipe connnection
 * if len is given it is assumed to be the lenght of the string
 * if it is -1, the length is found out with strlen()
 * The length of string is transmitted first as 4 byte unsigned integer,
 * followed by the string itself.
 * @param client_socket  a socket or pipe.  Could be a file also.
 * @param str string to be transmitted
 * @param len length of string to be transmitted
 */
void write_string(int client_socket, char *str, size_t len);

void write_eot(int client_socket);

int read_string_fragmentable(int client_socket, char *buffer, size_t buffer_size, consumer_function consume);

int read_string(int client_socket, consumer_function consume);

void write_4byte_string(int client_socket, const char *str);

void read_4byte_string(int client_socket, char *str);


#endif
