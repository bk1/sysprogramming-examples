/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 *
 * copy function using memory map
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>

#include <itskylib.h>

void usage(const char *argv0, const char *msg) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("Usage\n\n");
  printf("%s src destination\n", argv0);
  printf("%s -b blocksize src destination\n", argv0);
  printf("\n");
  printf("copies src to destination using memorymap\n");
  printf("\n");
  printf("optional block size is the size of file portion mapped at once.  Defaults to src file size\n");
  exit(1);
}

int main(int argc, char *argv[]) {

  int retcode;

  if (is_help_requested(argc, argv)) {
    usage(argv[0], "");
  }

  if (argc < 3 || argc > 5) {    /* Test for correct number of arguments */
    usage(argv[0], "wrong number of arguments");
  }

  char *src = argv[argc-2];
  char *dest   = argv[argc-1];

  int src_fd = open(src, O_RDONLY);
  handle_error(src_fd, "open src", PROCESS_EXIT);
  int dest_fd = open(dest, O_RDWR | O_CREAT | O_EXCL, 0600);
  handle_error(dest_fd, "open dest", PROCESS_EXIT);
  
  struct stat sbuf;
  retcode = fstat(src_fd, &sbuf);
  handle_error(retcode, "fstat src", PROCESS_EXIT);
  
  off_t size = sbuf.st_size;
  retcode = ftruncate(dest_fd, size);
  handle_error(retcode, "resize destination", PROCESS_EXIT);
  retcode = fsync(dest_fd);
  handle_error(retcode, "fsync destination", PROCESS_EXIT);

  void *src_addr = mmap(NULL, size, PROT_READ, MAP_SHARED, src_fd, 0);
  if (src_addr == MAP_FAILED) {
    handle_error(-1, "mmap src", PROCESS_EXIT);
  }
  handle_ptr_error(src_addr, "mmap src", PROCESS_EXIT);
  void *dest_addr   = mmap(NULL, size, PROT_WRITE, MAP_SHARED, dest_fd, 0);
  if (dest_addr == MAP_FAILED) {
    handle_error(-1, "mmap dest", PROCESS_EXIT);
  }
  handle_ptr_error(dest_addr, "mmap dest", PROCESS_EXIT);
  memcpy(dest_addr, src_addr, size);
  retcode = munmap(src_addr, size);
  handle_error(retcode, "munmap src", PROCESS_EXIT);
  retcode = munmap(dest_addr, size);
  handle_error(retcode, "munmap dest", PROCESS_EXIT);
  close(src_fd);
  close(dest_fd);
  exit(0);
}
