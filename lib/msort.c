/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/* implements <a href="https://de.wikipedia.org/wiki/Heapsort">Heapsort</a> */

#include <stdio.h>
#include <string.h>
#include <alloca.h>

#include <itskylib.h>
#include <msort.h>
#include <sortimpl.h>

void msort(void *base,
           size_t nmemb,
           size_t size,
           compare_fun2 compare) {
  msort_r(base, nmemb, size, compare_extend, (void *) compare);
}



void msort_r(void *base,
             size_t nmemb,
             size_t size,
             compare_fun3 compare,
             void *arg) {
  if (nmemb <= 1) {
    /* nothing to sort!! */
    return;
  }

  // printf("msort_r\n");

  void *scratch = malloc(nmemb * size);

  for (size_t step = 1; step < nmemb; step *= 2) {
    size_t dstep = 2*step;
    // printf("step=%ld dstep=%ld [", step, dstep);
    // for (size_t qq = 0; qq < nmemb; qq++) {
      // printf(" %d ", ((int *)base)[qq]);
    // }
    // printf("]\n");
    size_t start_left;
    size_t start_right;
    for (start_left = 0, start_right = step; start_right < nmemb; start_left += dstep, start_right += dstep) {
      size_t i = 0;
      size_t j = 0;
      size_t m = step;
      size_t n = step;
      if (start_right + n > nmemb) {
        n = nmemb - start_right;
      }
      size_t total_len = m + n;
      // printf("start_left=%ld start_right=%ld nmemb=%ld m=%ld n=%ld total_len=%ld [", start_left, start_right, nmemb, m, n, total_len);
      // for (size_t qq = 0; qq < nmemb; qq++) {
        // printf(" %d ", ((int *)base)[qq]);
      // }
      // printf("]\n");
      void *result = scratch;
      void *left  = POINTER(base, start_left,  size);
      void *right = POINTER(base, start_right, size);
      void *source = left;
      while (i+j < total_len) {
        // printf("i=%ld j=%ld m=%ld n=%ld src=[", i, j, m, n);
        // for (size_t qq = 0; qq < nmemb; qq++) {
          // printf(" %d ", ((int *)base)[qq]);
        // }
        // printf("] dest=[");
        // for (size_t qq = 0; qq < i+j; qq++) {
          // printf(" %d ", ((int *)scratch)[qq]);
        // }
        // printf("]\n");
        if (i >= m) {
          memcpy(result, right, (n-j)*size);
          j = n;
          // printf("was i>=m (copied rest of right) i=%ld j=%ld m=%ld n=%ld src=[", i, j, m, n);
          // for (size_t qq = 0; qq < nmemb; qq++) {
            // printf(" %d ", ((int *)base)[qq]);
          // }
          // printf("] dest=[");
          // for (size_t qq = 0; qq < i+j; qq++) {
            // printf(" %d ", ((int *)scratch)[qq]);
          // }
          // printf("]\n");
        } else if (j >= n) {
          memcpy(result, left, (m-i)*size);
          i = m;
          // printf("was j>=n (copied rest of left) i=%ld j=%ld m=%ld n=%ld src=[", i, j, m, n);
          // for (size_t qq = 0; qq < nmemb; qq++) {
            // printf(" %d ", ((int *)base)[qq]);
          // }
          // printf("] dest=[");
          // for (size_t qq = 0; qq < i+j; qq++) {
            // printf(" %d ", ((int *)scratch)[qq]);
          // }
          // printf("]\n");
        } else {
          if (compare(left, right, arg) <= 0) {
            memcpy(result, left, size);
            result += size;
            left += size;
            i++;
            // printf("was left<=right (copied one from left) i=%ld j=%ld m=%ld n=%ld src=[", i, j, m, n);
            // for (size_t qq = 0; qq < nmemb; qq++) {
              // printf(" %d ", ((int *)base)[qq]);
            // }
            // printf("] dest=[");
            // for (size_t qq = 0; qq < i+j; qq++) {
              // printf(" %d ", ((int *)scratch)[qq]);
            // }
            // printf("]\n");
          } else {
            memcpy(result, right, size);
            result += size;
            right += size;
            j++;
            // printf("was left > right (copied one from right) i=%ld j=%ld m=%ld n=%ld src=[", i, j, m, n);
            // for (size_t qq = 0; qq < nmemb; qq++) {
              // printf(" %d ", ((int *)base)[qq]);
            // }
            // printf("] dest=[");
            // for (size_t qq = 0; qq < i+j; qq++) {
              // printf(" %d ", ((int *)scratch)[qq]);
            // }
            // printf("]\n");
          }
        }
      }
      memcpy(source, scratch, total_len*size);
      // printf("i=%ld j=%ld m=%ld n=%ld total_len=%ld [", i, j, m, n, total_len);
      // for (size_t qq = 0; qq < nmemb; qq++) {
        // printf(" %d ", ((int *)base)[qq]);
      // }
      // printf("]\n");
    }
  }
}
