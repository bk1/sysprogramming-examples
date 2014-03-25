/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <itskylib.h>

void usage(const char *argv0, const char *msg) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("Usage\n\n");
  printf("%s [-o] [-f] [-e]\n\texample of buffered output that is duplicated due to a fork\n", argv0);
  printf("\t-o: write to stdout\n");
  printf("\t-f: write to file (myfile in current directory)\n");
  printf("\t-e: write to stderr\n\n");
  exit(1);
}

int main(int argc, char *argv[]) {

  if (is_help_requested(argc, argv)) {
    usage(argv[0], "");
  }
  if (argc > 4) {
    usage(argv[0], "too many arguments");
  }

  int to_stdout = FALSE;
  int to_stderr = FALSE;
  int to_file = FALSE;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-o") == 0) { to_stdout = TRUE; }
    if (strcmp(argv[i], "-e") == 0) { to_stderr = TRUE; }
    if (strcmp(argv[i], "-f") == 0) { to_file = TRUE; }
  }

  FILE *out = fopen("myfile", "w");
  char *TEXT = "Beispieltext... ";
  char *TEXTO = "Beispieltext (stdout)... ";
  char *TEXTE = "Beispieltext (stderr)... ";
  char *NEWLINE = "\n";
  char *NEWLINEO = " (nl stdout)\n";
  char *NEWLINEE = " (nl stderr)\n";

  /* these are done once, but unbuffered stuff will be duplicated an occur twice in output */
  if (to_file)   { fwrite(TEXT, 1, strlen(TEXT), out); }
  if (to_stdout) { fwrite(TEXTO, 1, strlen(TEXTO), stdout); }
  if (to_stderr) { fwrite(TEXTE, 1, strlen(TEXTE), stderr); }

  fork();

  /* these are done twice, once in child and once in parent */
  if (to_file)   { fwrite(NEWLINE, 1, 1, out); }
  if (to_stdout) { fwrite(NEWLINEO, 1, strlen(NEWLINEO), stdout); }
  if (to_stderr) { fwrite(NEWLINEE, 1, strlen(NEWLINEE), stderr); }

  exit(0);
}
