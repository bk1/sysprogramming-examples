/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  FILE *out = fopen("myfile", "w");
  char *TEXT = "Beispieltext... ";
  char *TEXTO = "Beispieltext (stdout)... ";
  char *TEXTE = "Beispieltext (stderr)... ";
  char *NEWLINE = "\n";
  char *NEWLINEO = " (nl stdout)\n";
  char *NEWLINEE = " (nl stderr)\n";
  fwrite(TEXT, 1, strlen(TEXT), out);
  fwrite(TEXTO, 1, strlen(TEXTO), stdout);
  fwrite(TEXTE, 1, strlen(TEXTE), stderr);
  fork();
  fwrite(NEWLINE, 1, 1, out);
  fwrite(NEWLINEO, 1, strlen(NEWLINEO), stdout);
  fwrite(NEWLINEE, 1, strlen(NEWLINEE), stderr);
  exit(0);
}
