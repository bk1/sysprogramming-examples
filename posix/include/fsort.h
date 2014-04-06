/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#ifndef _LIB_ITSKY_FSORT
#define _LIB_ITSKY_FSORT

#include <sys/types.h>
#include <sortcommon.h>

void fsort(void *base, 
           size_t nmemb, 
           size_t size,
           compare_fun2 compare,
           metric_fun1 metric);

void fsort_r(void *base, 
             size_t nmemb, 
             size_t size,
             compare_fun3 compare,
             void *argc,
             metric_fun2 metric,
             void *argm);

void fsort_f(void *base, 
             size_t nmemb, 
             size_t size,
             double factor,
             compare_fun3 compare,
             void *argc,
             metric_fun2 metric,
             void *argm);

#endif
