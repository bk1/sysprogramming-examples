/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/* implements tests for psort */

/* enable qsort_r */
#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <CUnit/Basic.h>
#include <stdlib.h>

#include <psort.h>
#include <fsort-metrics.h>

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int init_suite1() {
  return 0;
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite1(void) {
  return 0;
}

void parallel_sort_any_r(enum sort_type selected_sort_type,
                         void *base,
                         size_t nmemb,
                         size_t size,
                         compare_fun3 compare,
                         void *argc,
                         metric_fun2 metric,
                         void *argm,
                         unsigned int thread_count) {
  switch (selected_sort_type) {
  case HEAP_SORT:
    parallel_hsort_r(base,
                     nmemb,
                     size,
                     compare,
                     argc,
                     thread_count);
    break; 
  case TERNARY_HEAP_SORT:
    parallel_ternary_hsort_r(base,
                             nmemb,
                             size,
                             compare,
                             argc,
                             thread_count);
    break; 
  case QUICK_SORT:
    parallel_qsort_r(base,         
                     nmemb,        
                     size,         
                     compare,      
                     argc,         
                     thread_count);
    break; 
    case FLASH_SORT:
      parallel_fsort_r(base,
                       nmemb,
                       size,
                       compare,
                       argc,
                       metric,
                       argm,
                       thread_count);
      break; 
  case INSERTION_SORT:
    parallel_isort_r(base,         
                     nmemb,        
                     size,         
                     compare,      
                     argc,         
                     thread_count);
    break; 
  case MERGE_SORT:
    parallel_msort_r(base,         
                     nmemb,        
                     size,         
                     compare,      
                     argc,         
                     thread_count);
    break;
  default:
    fprintf(stderr, "invalid parameter selected_sort_type=%d\n", selected_sort_type);
    exit(1);
  }
}

static const enum sort_type SORT_TYPES[] =  { HEAP_SORT, TERNARY_HEAP_SORT, QUICK_SORT, FLASH_SORT, INSERTION_SORT, MERGE_SORT };
static const size_t ST_COUNT = 6;

/* Simple test of sort with empty array
 */
void test_sort_empty() {
  int arr[0];
  for (int thread_count = 1; thread_count < 5; thread_count++) {
    for (int st = 0; st < ST_COUNT; st++) {
      enum sort_type ste = SORT_TYPES[st];
      parallel_sort_any_r(ste, arr, 0, sizeof(int), compare_int_full, NULL, metric_int_full, NULL, thread_count);
    }
  }
}

/* Simple test of sort with one-element-array
 */
void test_sort_one() {
  int arr[] = { 77 };
  for (int thread_count = 1; thread_count < 5; thread_count++) {
    for (int st = 0; st < ST_COUNT; st++) {
      enum sort_type ste = SORT_TYPES[st];
      parallel_sort_any_r(ste, arr, 1, sizeof(int), compare_int_full, NULL, metric_int_full, NULL, thread_count);
      CU_ASSERT_EQUAL(arr[0], 77);
    }
  }
}

/* Simple test of sort with two-element-array ascending
 */
void test_sort_two_asc() {
  int arr[] = { 77, 98 };
  for (int thread_count = 1; thread_count < 5; thread_count++) {
    for (int st = 0; st < ST_COUNT; st++) {
      enum sort_type ste = SORT_TYPES[st];
      parallel_sort_any_r(ste, arr, 2, sizeof(int), compare_int_full, NULL, metric_int_full, NULL, thread_count);
      CU_ASSERT_EQUAL(arr[0], 77);
      CU_ASSERT_EQUAL(arr[1], 98);
      arr[0] = 77;
      arr[1] = 98;
    }
  }
}

/* Simple test of sort with two-element-array descending
 */
void test_sort_two_desc() {
  int arr[] = { 98, 77 };
  for (int thread_count = 1; thread_count < 5; thread_count++) {
    for (int st = 0; st < ST_COUNT; st++) {
      enum sort_type ste = SORT_TYPES[st];
      parallel_sort_any_r(ste, arr, 2, sizeof(int), compare_int_full, NULL, metric_int_full, NULL, thread_count);
      CU_ASSERT_EQUAL(arr[0], 77);
      CU_ASSERT_EQUAL(arr[1], 98);
      arr[0] = 98;
      arr[1] = 77;
    }
  }
}

/* Simple test of sort with two-element-array ascending
 */
void test_sort_two_same() {
  int arr[] = { 88, 88 };
  for (int thread_count = 1; thread_count < 5; thread_count++) {
    for (int st = 0; st < ST_COUNT; st++) {
      enum sort_type ste = SORT_TYPES[st];
      parallel_sort_any_r(ste, arr, 2, sizeof(int), compare_int_full, NULL, metric_int_full, NULL, thread_count);
      CU_ASSERT_EQUAL(arr[0], 88);
      CU_ASSERT_EQUAL(arr[1], 88);
    }
  }
}

void check_three(int i, int j, int k);

/* Simple test of sort with two-element-array ascending
 */
void test_sort_three() {
  for (int i = 1; i <= 3; i++) {
    for (int j = 1; j <= 3; j++) {
      for (int k = 1; k <= 3; k++) {
        check_three(i, j, k);
      }
    }
  }
}

/* Simple test of sort with three-element-array ascending
 */
void test_sort_neg_three() {
  for (int i = -1; i >= -3; i--) {
    for (int j = -1; j >= -3; j--) {
      for (int k = -1; k >= -3; k--) {
        check_three(i, j, k);
      }
    }
  }
}

void print_arr(int *arr, int n) {
  printf("[");
  for (int i = 0; i < n; i++) {
    printf(" %d ", arr[i]);
  }
  printf("]");
}

void check_three(int i, int j, int k) {
  int master_arr[3];
  int arr[3];
  master_arr[0] = i;
  master_arr[1] = j;
  master_arr[2] = k;
  /* we assume that qsort is quite correct */
  qsort_r(master_arr, 3, sizeof(int), compare_int_full, NULL);
  CU_ASSERT(master_arr[0] <= master_arr[1]);
  CU_ASSERT(master_arr[1] <= master_arr[2]);

  for (int thread_count = 1; thread_count < 5; thread_count++) {
    for (int st = 0; st < ST_COUNT; st++) {
      enum sort_type ste = SORT_TYPES[st];
      arr[0] = i;
      arr[1] = j;
      arr[2] = k;
      parallel_sort_any_r(ste, arr, 3, sizeof(int), compare_int_full, NULL, metric_int_full, NULL, thread_count);
      CU_ASSERT_EQUAL(arr[0], master_arr[0]);
      CU_ASSERT_EQUAL(arr[1], master_arr[1]);
      CU_ASSERT_EQUAL(arr[2], master_arr[2]);
    }
  }
}

void check_parallel_n(int master_arr[], int imax);

void test_ascending_n() {
  int master_arr[100];
  for (int i = 0; i < 100; i++) {
    master_arr[i] = 3*i*i+2*i+1;
  }
  check_parallel_n(master_arr, 100);
}

void test_descending_n() {
  int master_arr[100];
  for (int i = 0; i < 100; i++) {
    master_arr[i] = 1000000-3*i*i+2*i+1;
  }
  check_parallel_n(master_arr, 100);
}

void test_negative_n() {
  int master_arr[100];
  for (int i = 0; i < 100; i++) {
    master_arr[i] = -3*i*i+2*i+1;
  }
  check_parallel_n(master_arr, 100);
}

void test_oscillating_n() {
  int master_arr[100];
  for (int i = 0; i < 100; i++) {
    master_arr[i] = 5 - 4*i + 3*i*i - 2*i*i*i + i*i*i*i;
  }
  check_parallel_n(master_arr, 100);
}

void test_random_n() {
  int master_arr[100];
  for (int i = 0; i < 100; i++) {
    master_arr[i] = (int) random();
  }
  check_parallel_n(master_arr, 100);
}

void check_parallel_n(int master_arr[], int imax) {
  int sorted_arr[imax];
  int arr[imax];

  for (int i = 0; i <= imax; i++) {
    for (int j = 0; j < imax; j++) {
      sorted_arr[j] = master_arr[j];
    }
    qsort_r(sorted_arr, i, sizeof(int), compare_int_full, NULL);
    for (int thread_count = 1; thread_count < 5; thread_count++) {
      for (int st = 0; st < ST_COUNT; st++) {
        enum sort_type ste = SORT_TYPES[st];
        for (int j = 0; j < i; j++) {
          arr[j] = master_arr[j];
        }
        parallel_sort_any_r(ste, arr, i, sizeof(int), compare_int_full, NULL, metric_int_full, NULL, thread_count);

        for (int j = 0; j < i; j++) {
          CU_ASSERT_EQUAL(sorted_arr[j], arr[j]);
        }
      }
    }
    for (int j = 1; j < i; j++) {
      int a = sorted_arr[j-1];
      int b = sorted_arr[j];
      CU_ASSERT(a <= b);
      CU_ASSERT(metric_int_full(&a, NULL) <= metric_int_full(&b, NULL));
    }
  }
}

/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
int main() {
  CU_pSuite pSuite = NULL;

  /* initialize the CUnit test registry */
  if (CUE_SUCCESS != CU_initialize_registry()) {
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("Suite_1", init_suite1, clean_suite1);
  if (NULL == pSuite) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  /* add the tests to the suite */
  if ((NULL == CU_add_test(pSuite, "test of sorts on empty sets", test_sort_empty))
      || (NULL == CU_add_test(pSuite, "test of sorts on one-element sets", test_sort_one))
      || (NULL == CU_add_test(pSuite, "test of sorts on ascending two-element sets", test_sort_two_asc))
      || (NULL == CU_add_test(pSuite, "test of sorts on descending two-element sets", test_sort_two_desc))
      || (NULL == CU_add_test(pSuite, "test of sorts on two-element sets with both elements the same", test_sort_two_same))
      || (NULL == CU_add_test(pSuite, "test of sorts on all kinds of positive three-element sets", test_sort_three))
      || (NULL == CU_add_test(pSuite, "test of sorts on all kinds of negative three-element sets", test_sort_neg_three))
      || (NULL == CU_add_test(pSuite, "test of sorts on all lengths from 0 to 100 of an ascending set", test_ascending_n))
      || (NULL == CU_add_test(pSuite, "test of sorts on all lengths from 0 to 100 of an descending set", test_descending_n))
      || (NULL == CU_add_test(pSuite, "test of sorts on all lengths from 0 to 100 of an negative set", test_negative_n))
      || (NULL == CU_add_test(pSuite, "test of sorts on all lengths from 0 to 100 of an oscillating set", test_oscillating_n))
      || (NULL == CU_add_test(pSuite, "test of sorts on all lengths from 0 to 100 of an random set", test_random_n))
      ) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}
