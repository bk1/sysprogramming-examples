/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <hsort.h>
#include <stdio.h>

#define PARENT(idx) (((idx) - 1) / 2)
#define LEFT(idx) (2*(idx) + 1)
#define RIGHT(idx) (2*(idx) + 2)



int compare_extend(const void *left, const void *right, void *mem) {
  compare_fun2 *compare_basic = (compare_fun2 *) mem;
  return (*compare_basic)(left, right);
}

void hsort(void *base, 
           size_t nmemb, 
           size_t size,
           int (*compare)(const void *, const void *)) {
  hsort_r(base, nmemb, size, compare_extend, (void *) compare);
}

  

void hsort_r(void *base, 
             size_t nmemb, 
             size_t size,
             int (*compare)(const void *, const void *, void *),
             void *arg) {
}

 int main(int argc, char *argv[]) {
   int i;
   for (i = 0; i < 10; i++) {
     int left = LEFT(i);
     int right = RIGHT(i);
     int lp = PARENT(left);
     int rp = PARENT(right);
     int l_ok = lp == i;
     int r_ok = rp == i;
     printf("i=%d l=%d r=%d l_ok=%d r_ok=%d\n", i, left, right, l_ok, r_ok);
   }
   printf("------------------------------------------------------------");
   printf("parent(0)=%d\n", PARENT(0));
   for (i = 1; i < 20; i++) {
     int parent = PARENT(i);
     int left = LEFT(parent);
     int right = RIGHT(parent);
     int ok = left == i || right == i;
     printf("i=%d p=%d l=%d r=%d ok=%d\n", i, parent, left, right, ok);
   }
     
 }   
