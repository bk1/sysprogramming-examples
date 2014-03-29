/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/* implements ternary heapsort, which is slightly faster than regular <a href="https://de.wikipedia.org/wiki/Heapsort">Heapsort</a> */

#include <stdio.h>
#include <string.h>
#include <alloca.h>

#include <itskylib.h>
#include <ternary-hsort.h>
#include <sortimpl.h>

#define PARENT(idx) (((idx) - 1) / 3)
#define LEFT(idx) (3*(idx) + 1)
#define MIDDLE(idx) (3*(idx) + 2)
#define RIGHT(idx) (3*(idx) + 3)


void ternary_sift_down(void *base,
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
    size_t middle_child = MIDDLE(root);
    size_t right_child = RIGHT(root);
    if (middle_child <= max_index) {
      void *middle_child_ptr = POINTER(base, middle_child, size);
      if (compare(swap_ptr, middle_child_ptr, arg) < 0) {
        swap = middle_child;
        swap_ptr = middle_child_ptr;
      }
      if (right_child <= max_index) {
        void *right_child_ptr = POINTER(base, right_child, size);
        if (compare(swap_ptr, right_child_ptr, arg) < 0) {
          swap = right_child;
          swap_ptr = right_child_ptr;
        }
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

void ternary_heapify(void *base,
                     size_t nmemb,
                     size_t size,
                     compare_fun3 compare,
                     void *arg) {
  size_t max_index = nmemb-1;
  size_t start = PARENT(max_index);
  for (long i = start; i >= 0; i--) {
    size_t parent = (size_t) i;
    ternary_sift_down(base, parent, size, max_index, compare, arg);
  }
}

void ternary_hsort(void *base,
                   size_t nmemb,
                   size_t size,
                   compare_fun2 compare) {
  ternary_hsort_r(base, nmemb, size, compare_extend, (void *) compare);
}



void ternary_hsort_r(void *base,
                     size_t nmemb,
                     size_t size,
                     compare_fun3 compare,
                     void *arg) {
  if (nmemb <= 1) {
    /* nothing to sort!! */
    return;
  }
  ternary_heapify(base, nmemb, size, compare, arg);
  size_t end = nmemb - 1;
  for (end = nmemb - 1; end > 0; end--) {
    void *end_ptr = POINTER(base, end, size);
    swap_elements(base, end_ptr, size);
    ternary_sift_down(base, 0, size, end-1, compare, arg);
  }
}
