/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-06-02
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/*Include necessary header files.*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <aio.h>

#include <pthread.h>

#include <itskylib.h>

int buffer_size;
int nfds;
char **pipe_names;

void usage(char *msg, char *argv0) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("USAGE:\n\n%s buffersize inpipe1 inpipe2 ... inpipen\n", argv0);
  if (msg != NULL && strlen(msg) > 0) {
    exit(1);
  } else {
    exit(0);
  }
}

void *handle_pipe_data(void *arg) {
  printf("thread started\n");
  struct aiocb *aiocbp = (struct aiocb *) arg;
  int retcode = aio_error(aiocbp);
  handle_error(retcode, "aio_error", THREAD_EXIT);
  ssize_t size_read = aio_return(aiocbp);
  handle_error(retcode, "aio_return", THREAD_EXIT);

  int fd = aiocbp->aio_fildes;
  char *name = pipe_names[fd];
  if (size_read == 0) {
    printf("EOF for %s (fd=%d)\n", name, fd);
    return NULL;
  }
  ((char *)aiocbp->aio_buf)[size_read] = '\000';
  printf("read %ld bytes from %s (fd=%d):\n%s\n", (long) size_read, name, fd, (char *) aiocbp->aio_buf);
  sleep(10);
  exit(0);
}

int main(int argc, char **argv) {
  if (argc == 2 && strcmp(argv[1], "--help") == 0) {
    usage(NULL, argv[0]);
  }
  if (argc < 3) {
    usage("not enough arguments", argv[0]);
    exit(1);
  }

  int max_fd = -1;
  int retcode;

  buffer_size = atoi(argv[1]);
  int npipes = argc-2;

  struct aiocb *aiocbs = (struct aiocb *) malloc(npipes * sizeof(struct aiocb));

  struct sigevent sigevent;
  memset(&sigevent, 0, sizeof(sigevent));
  sigevent.sigev_notify = SIGEV_THREAD;
  sigevent.sigev_notify_function = handle_pipe_data;

  int *fds = (int *) malloc(npipes * sizeof(int));
  for (int i = 0; i < npipes; i++) {
    char *name = argv[2+i];

    // O_NONBLOCK makes the open on a pipe succeed, even if there is no writer open on the other side:
    int fd = open(name, O_RDONLY | O_NONBLOCK);
    handle_error(fd, "open", PROCESS_EXIT);
    if (fd > max_fd) {
      max_fd = fd;
    }
    fds[i] = fd;
  }
  nfds = max_fd + 1;
  pipe_names = (char **) malloc(nfds * sizeof(char *));
  for (int i = 0; i < nfds; i++) {
    pipe_names[i] = "";
  }
  for (int i = 0; i < npipes; i++) {
    pipe_names[fds[i]] = argv[2+i];
  }

  for (int i = 0; i < npipes; i++) {
    int fd = fds[i];

    // set it to BLOCKING:
    int flags = fcntl(fd, F_GETFL, 0);
    handle_error(flags, "fcntl F_GETFL", PROCESS_EXIT);
    flags &= ~O_NONBLOCK;
    retcode = fcntl(fd, F_SETFL, flags);
    handle_error(retcode, "fcntl F_SETFL", PROCESS_EXIT);

    memset(&(aiocbs[i]), 0, sizeof(struct aiocb));
    sigevent.sigev_value.sival_ptr = &aiocbs[i];
    aiocbs[i].aio_fildes = fd;
    aiocbs[i].aio_offset = 0;
    aiocbs[i].aio_buf    = malloc(buffer_size + 1);
    aiocbs[i].aio_nbytes = buffer_size;
    aiocbs[i].aio_sigevent = sigevent;
    aiocbs[i].aio_lio_opcode = LIO_READ;
    retcode = aio_read(&aiocbs[i]);
    handle_error(retcode, "aio_read", PROCESS_EXIT);
  }
  /* let the child threads do the work */
  printf("all assync reads started\n");
  pthread_exit(NULL);
  exit(0);
}
