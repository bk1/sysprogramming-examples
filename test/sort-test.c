/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/* implements tests for hsort */

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


/* Pointer to the file used by the tests. */
static FILE* temp_file = NULL;

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

/* Simple test of sort with empty array
 */
void test_sort_empty() {
  int arr[0];
  ternary_hsort_r(arr, 0, sizeof(int), compare_int_full, NULL);
  hsort_r(arr, 0, sizeof(int), compare_int_full, NULL);
  fsort_r(arr, 0, sizeof(int), compare_int_full, NULL, metric_int_full, NULL);
  qsort_r(arr, 0, sizeof(int), compare_int_full, NULL);
  isort_r(arr, 0, sizeof(int), compare_int_full, NULL);
  msort_r(arr, 0, sizeof(int), compare_int_full, NULL);
}

/* Simple test of sort with one-element-array
 */
void test_sort_one() {
  int arr[] = { 77 };
  hsort_r(arr, 1, sizeof(int), compare_int_full, NULL);
  CU_ASSERT_EQUAL(arr[0], 77);
  ternary_hsort_r(arr, 1, sizeof(int), compare_int_full, NULL);
  CU_ASSERT_EQUAL(arr[0], 77);
  fsort_r(arr, 1, sizeof(int), compare_int_full, NULL, metric_int_full, NULL);
  CU_ASSERT_EQUAL(arr[0], 77);
  qsort_r(arr, 1, sizeof(int), compare_int_full, NULL);
  CU_ASSERT_EQUAL(arr[0], 77);
  isort_r(arr, 1, sizeof(int), compare_int_full, NULL);
  CU_ASSERT_EQUAL(arr[0], 77);
  msort_r(arr, 1, sizeof(int), compare_int_full, NULL);
  CU_ASSERT_EQUAL(arr[0], 77);
}

/* Simple test of sort with two-element-array ascending
 */
void test_sort_two_asc() {
  int arr[] = { 77, 98 };
  hsort_r(arr, 2, sizeof(int), compare_int_full, NULL);
  CU_ASSERT_EQUAL(arr[0], 77);
  CU_ASSERT_EQUAL(arr[1], 98);
  ternary_hsort_r(arr, 2, sizeof(int), compare_int_full, NULL);
  CU_ASSERT_EQUAL(arr[0], 77);
  CU_ASSERT_EQUAL(arr[1], 98);
  fsort_r(arr, 2, sizeof(int), compare_int_full, NULL, metric_int_full, NULL);
  CU_ASSERT_EQUAL(arr[0], 77);
  CU_ASSERT_EQUAL(arr[1], 98);
  qsort_r(arr, 2, sizeof(int), compare_int_full, NULL);
  CU_ASSERT_EQUAL(arr[0], 77);
  CU_ASSERT_EQUAL(arr[1], 98);
  isort_r(arr, 2, sizeof(int), compare_int_full, NULL);
  CU_ASSERT_EQUAL(arr[0], 77);
  CU_ASSERT_EQUAL(arr[1], 98);
  msort_r(arr, 2, sizeof(int), compare_int_full, NULL);
  CU_ASSERT_EQUAL(arr[0], 77);
  CU_ASSERT_EQUAL(arr[1], 98);
}

/* Simple test of sort with two-element-array descending
 */
void test_sort_two_desc() {
  int arr[] = { 98, 77 };
  hsort_r(arr, 2, sizeof(int), compare_int_full, NULL);
  CU_ASSERT_EQUAL(arr[0], 77);
  CU_ASSERT_EQUAL(arr[1], 98);
  arr[0] = 98;
  arr[1] = 77;
  ternary_hsort_r(arr, 2, sizeof(int), compare_int_full, NULL);
  CU_ASSERT_EQUAL(arr[0], 77);
  CU_ASSERT_EQUAL(arr[1], 98);
  arr[0] = 98;
  arr[1] = 77;
  fsort_r(arr, 2, sizeof(int), compare_int_full, NULL, metric_int_full, NULL);
  CU_ASSERT_EQUAL(arr[0], 77);
  CU_ASSERT_EQUAL(arr[1], 98);
  arr[0] = 98;
  arr[1] = 77;
  qsort_r(arr, 2, sizeof(int), compare_int_full, NULL);
  CU_ASSERT_EQUAL(arr[0], 77);
  CU_ASSERT_EQUAL(arr[1], 98);
  arr[0] = 98;
  arr[1] = 77;
  isort_r(arr, 2, sizeof(int), compare_int_full, NULL);
  CU_ASSERT_EQUAL(arr[0], 77);
  CU_ASSERT_EQUAL(arr[1], 98);
  arr[0] = 98;
  arr[1] = 77;
  msort_r(arr, 2, sizeof(int), compare_int_full, NULL);
  CU_ASSERT_EQUAL(arr[0], 77);
  CU_ASSERT_EQUAL(arr[1], 98);
}

/* Simple test of sort with two-element-array ascending
 */
void test_sort_two_same() {
  int arr[] = { 88, 88 };
  hsort_r(arr, 2, sizeof(int), compare_int_full, NULL);
  CU_ASSERT_EQUAL(arr[0], 88);
  CU_ASSERT_EQUAL(arr[1], 88);
  ternary_hsort_r(arr, 2, sizeof(int), compare_int_full, NULL);
  CU_ASSERT_EQUAL(arr[0], 88);
  CU_ASSERT_EQUAL(arr[1], 88);
  fsort_r(arr, 2, sizeof(int), compare_int_full, NULL, metric_int_full, NULL);
  CU_ASSERT_EQUAL(arr[0], 88);
  CU_ASSERT_EQUAL(arr[1], 88);
  qsort_r(arr, 2, sizeof(int), compare_int_full, NULL);
  CU_ASSERT_EQUAL(arr[0], 88);
  CU_ASSERT_EQUAL(arr[1], 88);
  isort_r(arr, 2, sizeof(int), compare_int_full, NULL);
  CU_ASSERT_EQUAL(arr[0], 88);
  CU_ASSERT_EQUAL(arr[1], 88);
  msort_r(arr, 2, sizeof(int), compare_int_full, NULL);
  CU_ASSERT_EQUAL(arr[0], 88);
  CU_ASSERT_EQUAL(arr[1], 88);
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
  int harr[3];
  int tarr[3];
  int farr[3];
  int iarr[3];
  int marr[3];
  int master_arr[3];
  master_arr[0] = i;
  master_arr[1] = j;
  master_arr[2] = k;
  /* we assume that qsort is quite correct */
  qsort_r(master_arr, 3, sizeof(int), compare_int_full, NULL);
  /* make some incomplete check anyway */
  CU_ASSERT(master_arr[0] <= master_arr[1]);
  CU_ASSERT(master_arr[1] <= master_arr[2]);

  harr[0] = i;
  harr[1] = j;
  harr[2] = k;
  hsort_r(harr, 3, sizeof(int), compare_int_full, NULL);
  CU_ASSERT_EQUAL(harr[0], master_arr[0]);
  CU_ASSERT_EQUAL(harr[1], master_arr[1]);
  CU_ASSERT_EQUAL(harr[2], master_arr[2]);

  tarr[0] = i;
  tarr[1] = j;
  tarr[2] = k;
  ternary_hsort_r(tarr, 3, sizeof(int), compare_int_full, NULL);
  CU_ASSERT_EQUAL(tarr[0], master_arr[0]);
  CU_ASSERT_EQUAL(tarr[1], master_arr[1]);
  CU_ASSERT_EQUAL(tarr[2], master_arr[2]);

  farr[0] = i;
  farr[1] = j;
  farr[2] = k;
  fsort_r(farr, 3, sizeof(int), compare_int_full, NULL, metric_int_full, NULL);
  if (farr[0] != master_arr[0]) {
    print_arr(farr, 3);
    printf(": fsort: i=%d j=%d k=%d: farr[0]=%d (%d)\n", i, j, k, farr[0], master_arr[0]);
  }
  CU_ASSERT_EQUAL(farr[0], master_arr[0]);
  if (farr[1] != master_arr[1]) {
    print_arr(farr, 3);
    printf(": fsort: i=%d j=%d k=%d: farr[1]=%d (%d)\n", i, j, k, farr[1], master_arr[1]);
  }
  CU_ASSERT_EQUAL(farr[1], master_arr[1]);
  if (farr[2] != master_arr[2]) {
    print_arr(farr, 3);
    printf(": fsort: i=%d j=%d k=%d: farr[2]=%d (%d)\n", i, j, k, farr[2], master_arr[2]);
  }
  CU_ASSERT_EQUAL(farr[2], master_arr[2]);

  iarr[0] = i;
  iarr[1] = j;
  iarr[2] = k;
  isort_r(iarr, 3, sizeof(int), compare_int_full, NULL);
  if (iarr[0] != master_arr[0]) {
    printf("isort: i=%d j=%d k=%d: iarr[0]=%d (%d)\n", i, j, k, iarr[0], master_arr[0]);
  }
  CU_ASSERT_EQUAL(iarr[0], master_arr[0]);
  if (iarr[1] != master_arr[1]) {
    printf("isort: i=%d j=%d k=%d: iarr[1]=%d (%d)\n", i, j, k, iarr[1], master_arr[1]);
  }
  CU_ASSERT_EQUAL(iarr[1], master_arr[1]);
  if (iarr[2] != master_arr[2]) {
    printf("isort: i=%d j=%d k=%d: iarr[2]=%d (%d)\n", i, j, k, iarr[2], master_arr[2]);
  }
  CU_ASSERT_EQUAL(iarr[2], master_arr[2]);

  marr[0] = i;
  marr[1] = j;
  marr[2] = k;
  msort_r(marr, 3, sizeof(int), compare_int_full, NULL);
  if (marr[0] != master_arr[0]) {
    printf("msort: i=%d j=%d k=%d: marr[0]=%d (%d)\n", i, j, k, marr[0], master_arr[0]);
  }
  CU_ASSERT_EQUAL(marr[0], master_arr[0]);
  if (marr[1] != master_arr[1]) {
    printf("msort: i=%d j=%d k=%d: marr[1]=%d (%d)\n", i, j, k, marr[1], master_arr[1]);
  }
  CU_ASSERT_EQUAL(marr[1], master_arr[1]);
  if (marr[2] != master_arr[2]) {
    printf("msort: i=%d j=%d k=%d: marr[2]=%d (%d)\n", i, j, k, marr[2], master_arr[2]);
  }
  CU_ASSERT_EQUAL(marr[2], master_arr[2]);
}

void test_ternary_hsort_seven_asc() {
  int arr[] = {  0, 1, 2, 3, 4, 5, 6 };
  ternary_hsort_r(arr, 7, sizeof(int), compare_int_full, NULL);
  printf("test_ternary_hsort_seven_asc: ");
  print_arr(arr, 7);
  for (int i = 0; i < 7; i++) {
    if (i != arr[i]) {
      printf("arr[%d]=%d\n", i, arr[i]);
    }
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
  int qarr[100];
  int iarr[100];
  int marr[100];
  int harr[100];
  int tarr[100];
  int farr[100];
  for (int i = 0; i <= 100; i++) {
    for (int j = 0; j < i; j++) {
      qarr[j] = master_arr[j];
    }
    qsort_r(qarr, i, sizeof(int), compare_int_full, NULL);
    for (int j = 0; j < i; j++) {
      iarr[j] = master_arr[j];
    }
    isort_r(iarr, i, sizeof(int), compare_int_full, NULL);
    for (int j = 0; j < i; j++) {
      if (iarr[j] != qarr[j]) {
        printf("check_n: isort: i=%d j=%d: iarr[j]=%d (%d)\n", i, j, iarr[j], qarr[j]);
      }
      CU_ASSERT_EQUAL(iarr[j], qarr[j]);
    }
    for (int j = 0; j < i; j++) {
      marr[j] = master_arr[j];
    }
    msort_r(marr, i, sizeof(int), compare_int_full, NULL);
    for (int j = 0; j < i; j++) {
      if (marr[j] != qarr[j]) {
        printf("check_n: msort: i=%d j=%d: marr[j]=%d (%d)\n", i, j, marr[j], qarr[j]);
      }
      CU_ASSERT_EQUAL(marr[j], qarr[j]);
    }
    for (int j = 0; j < i; j++) {
      harr[j] = master_arr[j];
    }
    hsort_r(harr, i, sizeof(int), compare_int_full, NULL);
    for (int j = 0; j < i; j++) {
      if (harr[j] != qarr[j]) {
        printf("check_n: ternary_hsort: i=%d j=%d: harr[j]=%d (%d)\n", i, j, harr[j], qarr[j]);
      }
      CU_ASSERT_EQUAL(harr[j], qarr[j]);
    }
    for (int j = 0; j < i; j++) {
      tarr[j] = master_arr[j];
    }
    ternary_hsort_r(tarr, i, sizeof(int), compare_int_full, NULL);
    for (int j = 0; j < i; j++) {
      if (tarr[j] != qarr[j]) {
        printf("check_n: ternary_hsort: i=%d j=%d: tarr[j]=%d (%d)\n", i, j, tarr[j], qarr[j]);
      }
      CU_ASSERT_EQUAL(tarr[j], qarr[j]);
    }
    for (int j = 0; j < i; j++) {
      farr[j] = master_arr[j];
    }
    fsort_r(farr, i, sizeof(int), compare_int_full, NULL, metric_int_full, NULL);
    for (int j = 0; j < i; j++) {
      if (farr[j] != qarr[j]) {
        printf("check_n: fsort: i=%d j=%d: farr[j]=%d (%d)\n", i, j, farr[j], qarr[j]);
      }
      CU_ASSERT_EQUAL(farr[j], qarr[j]);
    }
  }
  for (int j = 1; j < 100; j++) {
    int a = qarr[j-1];
    int b = qarr[j];
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

