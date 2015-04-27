#include <stdio.h>

int main(int argc, char **argv) {
  char buf[1000];
  sprintf(buf, "%s %d %d\n", "hello world", 3, 9);
  puts(buf);
}
