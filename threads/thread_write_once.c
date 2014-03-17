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
#include <pthread.h>

static char *TEXT = "Beispieltext... ";
static char *TEXTO = "Beispieltext (stdout)... ";
static char *TEXTE = "Beispieltext (stderr)... ";
static char *NEWLINE = "\n";
static char *NEWLINEO = " (nl stdout)\n";
static char *NEWLINEE = " (nl stderr)\n";
static FILE *out;

/* thread function */
void *run(void *raw) {
  char *str = (char *) raw;
  fwrite(NEWLINE, 1, 1, out);
  fwrite(str, strlen(str), 1, out);
  fwrite(NEWLINEO, 1, strlen(NEWLINEO), stdout);
  fwrite(str, strlen(str), 1, stdout);
  fwrite(NEWLINEE, 1, strlen(NEWLINEE), stderr);
  fwrite(str, strlen(str), 1, stderr);
  sleep(1);
  fwrite(NEWLINEO, 1, strlen(NEWLINEO), stdout);
  fwrite(str, strlen(str), 1, stdout);
  return NULL;
}

int main(int argc, char *argv[]) {
  out = fopen("myfile", "w");
  fwrite(TEXT, 1, strlen(TEXT), out);
  fwrite(TEXTO, 1, strlen(TEXTO), stdout);
  fwrite(TEXTE, 1, strlen(TEXTE), stderr);
  pthread_t thread1, thread2;
  pthread_create(&thread1, NULL, run, "thread1\n");
  pthread_create(&thread2, NULL, run, "thread2\n");
  run("main\n");
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  printf("joined all\n");
  exit(0);
}
