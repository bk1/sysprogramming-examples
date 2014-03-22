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
#include <hsort.h>
#include <sortimpl.h>

#define PARENT(idx) (((idx) - 1) / 2)
#define LEFT(idx) (2*(idx) + 1)
#define RIGHT(idx) (2*(idx) + 2)

int parent_idx(int idx) {
  return PARENT(idx);
}

int left_child_idx(int idx) {
  return LEFT(idx);
}

int right_child_idx(int idx) {
  return RIGHT(idx);
}


void sift_down(void *base,
               size_t start,
               size_t size,
               size_t max_index,
               compare_fun3 compare,
               void *arg) {
  size_t root = start;
  while (TRUE) {
    size_t left_child = LEFT(root);
    if (left_child > max_index) {
      break;
    }
    void *root_ptr = POINTER(base, root, size);
    size_t swap = root;
    void *swap_ptr = root_ptr;
    void *left_child_ptr = POINTER(base, left_child, size);
    if (compare(swap_ptr, left_child_ptr, arg) < 0) {
      swap = left_child;
      swap_ptr = left_child_ptr;
    }
    size_t right_child = left_child + 1;
    if (right_child <= max_index) {
      void *right_child_ptr = POINTER(base, right_child, size);
      if (compare(swap_ptr, right_child_ptr, arg) < 0) {
        swap = right_child;
        swap_ptr = right_child_ptr;
      }
    }
    if (swap != root) {
      swap_elements(root_ptr, swap_ptr, size);
      root = swap;
      root_ptr = swap_ptr;
    } else {
      return;
    }
  }
}

void heapify(void *base,
             size_t nmemb,
             size_t size,
             compare_fun3 compare,
             void *arg) {
  size_t max_index = nmemb-1;
  size_t start = PARENT(max_index);
  for (long i = start; i >= 0; i--) {
    size_t parent = (size_t) i;
    // printf("heapify(nmemb=%ld size=%ld start=%ld parent=%ld)\n", nmemb, size, start, parent);
    sift_down(base, parent, size, max_index, compare, arg);
  }
}

void hsort(void *base,
           size_t nmemb,
           size_t size,
           compare_fun2 compare) {
  hsort_r(base, nmemb, size, compare_extend, (void *) compare);
}



void hsort_r(void *base,
             size_t nmemb,
             size_t size,
             compare_fun3 compare,
             void *arg) {
  if (nmemb <= 1) {
    /* nothing to sort!! */
    return;
  }
  heapify(base, nmemb, size, compare, arg);
  size_t end = nmemb - 1;
  for (end = nmemb - 1; end > 0; end--) {
    void *end_ptr = POINTER(base, end, size);
    swap_elements(base, end_ptr, size);
    sift_down(base, 0, size, end-1, compare, arg);
  }
}

int test_macros(int argc, char *argv[]) {
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
   return 0;
}
