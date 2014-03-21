/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <stdio.h>
#include <string.h>
#include <alloca.h>

#include <sortcommon.h>
#include <itskylib.h>

/* exchange two elements of size */
void swap_elements(void *left_ptr, void *right_ptr, size_t size) {
  void *buff = alloca(size);
  memcpy(buff, left_ptr, size);
  memcpy(left_ptr, right_ptr, size);
  memcpy(right_ptr, buff, size);
}

/* map 3-param compare-function to 2-param-compare-function: The 2-param-function must be provided as the third parameter */
int compare_extend(const void *left, const void *right, void *mem) {
  compare_fun2 compare_basic = (compare_fun2) mem;
  return compare_basic(left, right);
}

/* map 2-param metric-function to 1-param-metric-function: The 1-param-function must be provided as the second parameter */
double metric_extend(const void *element, void *mem) {
  metric_fun1 metric_basic = (metric_fun1) mem;
  return metric_basic(element);
}


/* end of file sort_common.c */
