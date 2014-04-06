/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#ifndef _LIB_ITSKY_ISORT
#define _LIB_ITSKY_ISORT

#include <sys/types.h>
#include <sortcommon.h>

void isort(void *base, 
           size_t nmemb, 
           size_t size,
           compare_fun2 compare);

void isort_r(void *base, 
             size_t nmemb, 
             size_t size,
             compare_fun3 compare,
             void *arg);

#endif
