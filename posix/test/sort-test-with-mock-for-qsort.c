/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/* This tests replaces the qsort from the libc by another an incorrect version of qsort which should cause most tests to fail */

/* enable qsort_r */
#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <CUnit/Basic.h>
#include <stdlib.h>

#include <hsort.h>
#include <ternary-hsort.h>
#include <isort.h>
#include <msort.h>
#include <fsort.h>
#include <fsort-metrics.h>
#include <psort.h>


/* Pointer to the file used by the tests. */
static FILE* temp_file = NULL;


void qsort_r(void *base,
             size_t nmemb,
             size_t size,
             compare_fun3 compare,
             void *arg) {

  if (nmemb > 0) {
    ((int *)base)[0] = 0;
  }
}

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int init_suite1() {
  if (NULL == (temp_file = fopen("temp.txt", "w+"))) {
    return -1;
  } else {
    return 0;
  }
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite1(void) {
  if (0 != fclose(temp_file)) {
    return -1;
  } else {
    temp_file = NULL;
    return 0;
  }
}


void sort_any_r(enum sort_type selected_sort_type,
                void *base,
                size_t nmemb,
                size_t size,
                compare_fun3 compare,
                void *argc,
                metric_fun2 metric,
                void *argm) {
  switch (selected_sort_type) {
  case HEAP_SORT:
    hsort_r(base,
            nmemb,
            size,
            compare,
            argc);
    break;
  case TERNARY_HEAP_SORT:
    ternary_hsort_r(base,
                    nmemb,
                    size,
                    compare,
                    argc);
    break;
  case QUICK_SORT:
    qsort_r(base,
            nmemb,
            size,
            compare,
            argc);
    break;
    case FLASH_SORT:
      fsort_r(base,
              nmemb,
              size,
              compare,
              argc,
              metric,
              argm);
      break;
  case INSERTION_SORT:
    isort_r(base,
            nmemb,
            size,
            compare,
            argc);
    break;
  case MERGE_SORT:
    msort_r(base,
            nmemb,
            size,
            compare,
            argc);
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
  for (int st = 0; st < ST_COUNT; st++) {
    enum sort_type ste = SORT_TYPES[st];
    sort_any_r(ste, arr, 0, sizeof(int), compare_int_full, NULL, metric_int_full, NULL);
  }
}

/* Simple test of sort with one-element-array
 */
void test_sort_one() {
  int arr[] = { 77 };
  for (int st = 0; st < ST_COUNT; st++) {
    enum sort_type ste = SORT_TYPES[st];
    sort_any_r(ste, arr, 1, sizeof(int), compare_int_full, NULL, metric_int_full, NULL);
    CU_ASSERT_EQUAL(arr[0], 77);
  }
}

/* Simple test of sort with two-element-array ascending
 */
void test_sort_two_asc() {
  for (int st = 0; st < ST_COUNT; st++) {
    enum sort_type ste = SORT_TYPES[st];
    int arr[] = { 77, 98 };
    sort_any_r(ste, arr, 2, sizeof(int), compare_int_full, NULL, metric_int_full, NULL);
    CU_ASSERT_EQUAL(arr[0], 77);
    CU_ASSERT_EQUAL(arr[1], 98);
  }
}

/* Simple test of sort with two-element-array descending
 */
void test_sort_two_desc() {
  for (int st = 0; st < ST_COUNT; st++) {
    enum sort_type ste = SORT_TYPES[st];
    int arr[] = { 98, 77 };
    sort_any_r(ste, arr, 2, sizeof(int), compare_int_full, NULL, metric_int_full, NULL);
    CU_ASSERT_EQUAL(arr[0], 77);
    CU_ASSERT_EQUAL(arr[1], 98);
  }
}

/* Simple test of sort with two-element-array ascending
 */
void test_sort_two_same() {
  for (int st = 0; st < ST_COUNT; st++) {
    enum sort_type ste = SORT_TYPES[st];
    int arr[] = { 88, 88 };
    sort_any_r(ste, arr, 2, sizeof(int), compare_int_full, NULL, metric_int_full, NULL);
    CU_ASSERT_EQUAL(arr[0], 88);
    CU_ASSERT_EQUAL(arr[1], 88);
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

/* Simple test of sort with two-element-array ascending
 */
void test_sort_three_123() {
  check_three(1, 2, 3);
}

/* Simple test of sort with two-element-array ascending
 */
void test_sort_three_321() {
  check_three(3, 2, 1);
}

/* Simple test of sort with two-element-array ascending
 */
void test_sort_three_312() {
  check_three(3, 1, 2);
}

void print_arr(int *arr, int n) {
  printf("[");
  for (int i = 0; i < n; i++) {
    printf(" %d ", arr[i]);
  }
  printf("]");
}

void check_three(int i, int j, int k) {
  int arr[3];
  int master_arr[3];
  master_arr[0] = i;
  master_arr[1] = j;
  master_arr[2] = k;
  /* we assume that qsort is quite correct */
  qsort_r(master_arr, 3, sizeof(int), compare_int_full, NULL);
  /* make some incomplete check anyway */
  CU_ASSERT(master_arr[0] <= master_arr[1]);
  CU_ASSERT(master_arr[1] <= master_arr[2]);
  for (int st = 0; st < ST_COUNT; st++) {
    enum sort_type ste = SORT_TYPES[st];
    arr[0] = i;
    arr[1] = j;
    arr[2] = k;
    sort_any_r(ste, arr, 3, sizeof(int), compare_int_full, NULL, metric_int_full, NULL);
    CU_ASSERT_EQUAL(arr[0], master_arr[0]);
    CU_ASSERT_EQUAL(arr[1], master_arr[1]);
    CU_ASSERT_EQUAL(arr[2], master_arr[2]);
  }
}

void test_ternary_hsort_seven_asc() {
  int arr[] = {  0, 1, 2, 3, 4, 5, 6 };
  ternary_hsort_r(arr, 7, sizeof(int), compare_int_full, NULL);
  for (int i = 0; i < 7; i++) {
    CU_ASSERT_EQUAL(i, arr[i]);
  }
}

void check_n(int master_arr[]);

void test_ascending_n() {
  int master_arr[100];
  for (int i = 0; i < 100; i++) {
    master_arr[i] = 3*i*i+2*i+1;
  }
  check_n(master_arr);
}

void test_descending_n() {
  int master_arr[100];
  for (int i = 0; i < 100; i++) {
    master_arr[i] = 1000000-3*i*i+2*i+1;
  }
  check_n(master_arr);
}

void test_negative_n() {
  int master_arr[100];
  for (int i = 0; i < 100; i++) {
    master_arr[i] = -3*i*i+2*i+1;
  }
  check_n(master_arr);
}

void test_oscillating_n() {
  int master_arr[100];
  for (int i = 0; i < 100; i++) {
    master_arr[i] = 5 - 4*i + 3*i*i - 2*i*i*i + i*i*i*i;
  }
  check_n(master_arr);
}

void test_random_n() {
  int master_arr[100];
  for (int i = 0; i < 100; i++) {
    master_arr[i] = (int) random();
  }
  check_n(master_arr);
}

void check_n(int master_arr[]) {
  int sorted_arr[100];
  int arr[100];
  for (int i = 0; i <= 100; i++) {
    for (int j = 0; j < i; j++) {
      sorted_arr[j] = master_arr[j];
    }
    qsort_r(sorted_arr, i, sizeof(int), compare_int_full, NULL);
    for (int st = 0; st < ST_COUNT; st++) {
      enum sort_type ste = SORT_TYPES[st];
      for (int j = 0; j < i; j++) {
        arr[j] = master_arr[j];
      }
      sort_any_r(ste, arr, i, sizeof(int), compare_int_full, NULL, metric_int_full, NULL);
      for (int j = 0; j < i; j++) {
        CU_ASSERT_EQUAL(arr[j], sorted_arr[j]);
      }
    }
  }
  for (int j = 1; j < 100; j++) {
    int a = sorted_arr[j-1];
    int b = sorted_arr[j];
    CU_ASSERT(a <= b);
    CU_ASSERT(metric_int_full(&a, NULL) <= metric_int_full(&b, NULL));
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
      || (NULL == CU_add_test(pSuite, "test of sorts on all kinds of three-element sets 123", test_sort_three_123))
      || (NULL == CU_add_test(pSuite, "test of sorts on all kinds of three-element sets 321", test_sort_three_321))
      || (NULL == CU_add_test(pSuite, "test of sorts on all kinds of three-element sets 312", test_sort_three_312))
      || (NULL == CU_add_test(pSuite, "test of sorts on all kinds of seven-element sets 0123456", test_ternary_hsort_seven_asc))
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

