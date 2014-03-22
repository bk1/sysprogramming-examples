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
#include <isort.h>
#include <sortimpl.h>

void isort(void *base,
           size_t nmemb,
           size_t size,
           compare_fun2 compare) {
  isort_r(base, nmemb, size, compare_extend, (void *) compare);
}



void isort_r(void *base,
             size_t nmemb,
             size_t size,
             compare_fun3 compare,
             void *arg) {
  if (nmemb <= 1) {
    /* nothing to sort!! */
    return;
  }

  void *tmp_ptr = alloca(size);

  // printf("\nisort: ------------------------------------------------------------\n");

  for (size_t i = 1; i < nmemb; i++) {
    /* invariant:  all elements with index < i are sorted. */
    // printf("i=%ld [", i);
    for (size_t k = 0; k < nmemb; k++) {
      // printf(" %d ", *(int *)POINTER(base, k, size));
    }
    // printf("]\n");
    void *ai_ptr = POINTER(base, i, size);
    // printf("i=%ld ai=%d\n", i, *(int *)ai_ptr);
    for (size_t j = 0; j < i; j++) {
      /* invariant: all elements with index < j are <= ai (*ai_ptr) */
      // printf("i=%ld j=%ld [", i, j);
      for (size_t k = 0; k < nmemb; k++) {
        // printf(" %d ", *(int *)POINTER(base, k, size));
      }
      // printf("]\n");
      void *aj_ptr = POINTER(base, j, size);
      // printf("j=%ld aj=%d\n", j, *(int *)aj_ptr);
      /* if we find an a[j] > a[i], we need to shift all the a[j]..a[i-1] by one slot and move a[i] before a[j] */
      /* otherweise, all a[j] are <= a[i], so we have nothing to do. */
      if (compare(aj_ptr, ai_ptr, arg) > 0) {
        /* aj > ai */
        memcpy(tmp_ptr, ai_ptr, size);
        void *aj_plus1_ptr = POINTER(base, j+1, size);
        // printf("j=%ld aj_plus=%d\n", j, *(int *)aj_plus1_ptr);
        // printf("memmoving: i=%ld j=%ld t=%d aj=%d aj_plus=%d i-j=%d [", i, j, *(int *)tmp_ptr, *(int *)aj_ptr, *(int *)aj_plus1_ptr, i-j);
        memmove(aj_plus1_ptr, aj_ptr, (i-j) * size);
        // printf("memmoved: i=%ld j=%ld t=%d aj=%d aj_plus=%d i-j=%d [", i, j, *(int *)tmp_ptr, *(int *)aj_ptr, *(int *)aj_plus1_ptr, i-j);
        for (size_t k = 0; k < nmemb; k++) {
          // printf(" %d ", *(int *)POINTER(base, k, size));
        }
        // printf("]\n");
        memcpy(aj_ptr, tmp_ptr, size);
        // printf("moved: i=%ld j=%ld [", i, j);
        for (size_t k = 0; k < nmemb; k++) {
          // printf(" %d ", *(int *)POINTER(base, k, size));
        }
        // printf("]\n");
        break;
      }
    }
  }
  // printf("done isort: ------------------------------------------------------------\n");
}
