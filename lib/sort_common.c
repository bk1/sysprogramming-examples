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

void swap_elements(void *left_ptr, void *right_ptr, size_t size) {
  void *buff = alloca(size);
  memcpy(buff, left_ptr, size);
  memcpy(left_ptr, right_ptr, size);
  memcpy(right_ptr, buff, size);
}
