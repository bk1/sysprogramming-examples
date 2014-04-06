/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#ifndef _LIB_ITSKY_FSORT_METRICS
#define _LIB_ITSKY_FSORT_METRICS

/* metric assuming even distribution */
double metric_str_full(const void *element, void *ignored);


/* metric assuming distribution of almost only printable characters (according to what is_string_char() in lib.c understands) */
double metric_binary_printable_pref(const void *element, void *ignored);

double metric_int_full(const void *element, void *ignored);

#endif
