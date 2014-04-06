/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#ifndef _LIB_ITSKY_SORTCOMMON
#define _LIB_ITSKY_SORTCOMMON

#include <sys/types.h>

typedef int (*compare_fun2)(const void *left, const void *right);
typedef int (*compare_fun3)(const void *left, const void *right, void *mem);

/* functions needed for fsort (Flashsort):
 * Warning: this interface is not final, 
 * possibly it will change from double to unsigned long or unsigned long long, a 64-bit unsigned integer.
 */
typedef double (*metric_fun1)(const void *element);
typedef double (*metric_fun2)(const void *element, void *mem);

/* exchange two elements of size */
void swap_elements(void *left_ptr, void *right_ptr, size_t size);

/* map 3-param compare-function to 2-param-compare-function: The 2-param-function must be provided as the third parameter */
int compare_extend(const void *left, const void *right, void *mem);

/* map 2-param metric-function to 1-param-metric-function: The 1-param-function must be provided as the second parameter */
double metric_extend(const void *element, void *mem);

/* compare for sort for strings */
int compare_str_full(const void *left, const void *right, void *ignored);

/* compare for sort for strings */
int compare_int_full(const void *left, const void *right, void *ignored);

#endif

/* end of file sortcommon.h */
