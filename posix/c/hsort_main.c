/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <hsort.h>
#include <itskylib.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef char *char_ptr;

int compare_simple(const void *left, const void *right) {
  return strcmp(*(const char_ptr *) left, *(const char_ptr *) right);
}

int compare_full(const void *left, const void *right, void *ignored) {
  return strcmp(*(const char_ptr *) left, *(const char_ptr *) right);
}

int main(int argc, char *argv[]) {
  int n = argc - 1;
  printf("n=%d\n", n);
  char_ptr *base = (char_ptr *) malloc(sizeof(char_ptr)*(n));
  printf("found base\n");
  memcpy(base, argv+1, sizeof(char_ptr) * n);
  for (int i = 0; i < n; i++) {
    printf("%4d: \"%s\"\n", i, base[i]);
  }
  printf("------------------------------------------------------------\n");
  hsort_r(base, n, sizeof(char *), compare_full, (void *) NULL);
  for (int i = 0; i < n; i++) {
    printf("%4d: \"%s\"", i, base[i]);
    int left = 2*i+1;
    if (left < n) {
      printf(" l=\"%s\"", base[left]);
      int right = left + 1;
      if (right < n) {
        printf(" r=\"%s\"", base[right]);
      }
    }
    printf("\n");
  }
  printf("------------------------------------------------------------\n");
  hsort(base, n, sizeof(char *), compare_simple);
  for (int i = 0; i < n; i++) {
    printf("%4d: \"%s\"", i, base[i]);
    int left = 2*i+1;
    if (left < n) {
      printf(" l=\"%s\"", base[left]);
      int right = left + 1;
      if (right < n) {
        printf(" r=\"%s\"", base[right]);
      }
    }
    printf("\n");
  }
}
