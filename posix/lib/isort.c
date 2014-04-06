/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/* implements <a href="https://de.wikipedia.org/wiki/Insertionsort">Insertionsort</a> */

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


  for (size_t i = 1; i < nmemb; i++) {
    /* invariant:  all elements with index < i are sorted. */
    for (size_t k = 0; k < nmemb; k++) {
    }
    void *ai_ptr = POINTER(base, i, size);
    for (size_t j = 0; j < i; j++) {
      /* invariant: all elements with index < j are <= ai (*ai_ptr) */
      for (size_t k = 0; k < nmemb; k++) {
      }
      void *aj_ptr = POINTER(base, j, size);
      /* if we find an a[j] > a[i], we need to shift all the a[j]..a[i-1] by one slot and move a[i] before a[j] */
      /* otherweise, all a[j] are <= a[i], so we have nothing to do. */
      if (compare(aj_ptr, ai_ptr, arg) > 0) {
        /* aj > ai */
        memcpy(tmp_ptr, ai_ptr, size);
        void *aj_plus1_ptr = POINTER(base, j+1, size);
        memmove(aj_plus1_ptr, aj_ptr, (i-j) * size);
        for (size_t k = 0; k < nmemb; k++) {
        }
        memcpy(aj_ptr, tmp_ptr, size);
        for (size_t k = 0; k < nmemb; k++) {
        }
        break;
      }
    }
  }
}
