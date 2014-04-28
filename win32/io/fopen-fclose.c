/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 *
 * lowlevel IO compiles, but crashes
 */

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
  FILE *out = fopen("../temp/test-h.txt", "w");
  const char *msg = "Hello, World!\n(fopen/fwrite/fclose)";
  fwrite(msg, 1, strlen(msg), out);
  fclose(out);
}
