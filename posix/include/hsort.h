/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#ifndef _LIB_ITSKY_HSORT
#define _LIB_ITSKY_HSORT

#include <sys/types.h>
#include <sortcommon.h>

int parent_idx(int idx);

int left_child_idx(int idx);

int right_child_idx(int idx);

void hsort(void *base,
           size_t nmemb,
           size_t size,
           compare_fun2 compare);

void hsort_r(void *base,
             size_t nmemb,
             size_t size,
             compare_fun3 compare,
             void *arg);

#endif
