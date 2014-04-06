/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#ifndef _LIB_ITSKY_PSORT
#define _LIB_ITSKY_PSORT

#include <sys/types.h>
#include <sortcommon.h>

enum sort_type { HEAP_SORT, TERNARY_HEAP_SORT, QUICK_SORT, FLASH_SORT, INSERTION_SORT, MERGE_SORT };

void psort_r(void *base,
             size_t nmemb,
             size_t size,
             compare_fun3 compare,
             void *argc,
             metric_fun2 metric,
             void *argm,
             unsigned int thread_count,
             enum sort_type selected_sort_type);

void parallel_hsort_r(void *base,
                      size_t nmemb,
                      size_t size,
                      compare_fun3 compare,
                      void *argc,
                      unsigned int thread_count);

void parallel_ternary_hsort_r(void *base,
                              size_t nmemb,
                              size_t size,
                              compare_fun3 compare,
                              void *argc,
                              unsigned int thread_count);

void parallel_qsort_r(void *base,
                      size_t nmemb,
                      size_t size,
                      compare_fun3 compare,
                      void *argc,
                      unsigned int thread_count);

void parallel_isort_r(void *base,
                      size_t nmemb,
                      size_t size,
                      compare_fun3 compare,
                      void *argc,
                      unsigned int thread_count);

void parallel_msort_r(void *base,
                      size_t nmemb,
                      size_t size,
                      compare_fun3 compare,
                      void *argc,
                      unsigned int thread_count);

void parallel_fsort_r(void *base,
                      size_t nmemb,
                      size_t size,
                      compare_fun3 compare,
                      void *argc,
                      metric_fun2 metric,
                      void *argm,
                      unsigned int thread_count);

#endif
