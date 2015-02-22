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

/* Simple test of sort with empty array
 */
void test_sort_empty() {
  int arr[0];
  for (int thread_count = 1; thread_count < 5; thread_count++) {
    parallel_ternary_hsort_r(arr, 0, sizeof(int), compare_int_full, NULL, thread_count);
    parallel_hsort_r(arr, 0, sizeof(int), compare_int_full, NULL, thread_count);
    parallel_fsort_r(arr, 0, sizeof(int), compare_int_full, NULL, metric_int_full, NULL, thread_count);
    parallel_qsort_r(arr, 0, sizeof(int), compare_int_full, NULL, thread_count);
    parallel_isort_r(arr, 0, sizeof(int), compare_int_full, NULL, thread_count);
    parallel_msort_r(arr, 0, sizeof(int), compare_int_full, NULL, thread_count);
  }
}

/* Simple test of sort with one-element-array
 */
void test_sort_one() {
  int arr[] = { 77 };
  for (int thread_count = 1; thread_count < 5; thread_count++) {
    parallel_hsort_r(arr, 1, sizeof(int), compare_int_full, NULL, thread_count);
    CU_ASSERT_EQUAL(arr[0], 77);
    parallel_ternary_hsort_r(arr, 1, sizeof(int), compare_int_full, NULL, thread_count);
    CU_ASSERT_EQUAL(arr[0], 77);
    parallel_fsort_r(arr, 1, sizeof(int), compare_int_full, NULL, metric_int_full, NULL, thread_count);
    CU_ASSERT_EQUAL(arr[0], 77);
    parallel_qsort_r(arr, 1, sizeof(int), compare_int_full, NULL, thread_count);
    CU_ASSERT_EQUAL(arr[0], 77);
    parallel_isort_r(arr, 1, sizeof(int), compare_int_full, NULL, thread_count);
    CU_ASSERT_EQUAL(arr[0], 77);
    parallel_msort_r(arr, 1, sizeof(int), compare_int_full, NULL, thread_count);
    CU_ASSERT_EQUAL(arr[0], 77);
  }
}

/* Simple test of sort with two-element-array ascending
 */
void test_sort_two_asc() {
  int arr[] = { 77, 98 };
  for (int thread_count = 1; thread_count < 5; thread_count++) {
    // printf("thread_count=%d hsort:\n", thread_count);
    parallel_hsort_r(arr, 2, sizeof(int), compare_int_full, NULL, thread_count);
    CU_ASSERT_EQUAL(arr[0], 77);
    CU_ASSERT_EQUAL(arr[1], 98);
    // printf("thread_count=%d ternary_hsort:\n", thread_count);
    parallel_ternary_hsort_r(arr, 2, sizeof(int), compare_int_full, NULL, thread_count);
    CU_ASSERT_EQUAL(arr[0], 77);
    CU_ASSERT_EQUAL(arr[1], 98);
    // printf("thread_count=%d fsort:\n", thread_count);
    parallel_fsort_r(arr, 2, sizeof(int), compare_int_full, NULL, metric_int_full, NULL, thread_count);
    CU_ASSERT_EQUAL(arr[0], 77);
    CU_ASSERT_EQUAL(arr[1], 98);
    // printf("thread_count=%d qsort:\n", thread_count);
    parallel_qsort_r(arr, 2, sizeof(int), compare_int_full, NULL, thread_count);
    CU_ASSERT_EQUAL(arr[0], 77);
    CU_ASSERT_EQUAL(arr[1], 98);
    // printf("thread_count=%d isort:\n", thread_count);
    parallel_isort_r(arr, 2, sizeof(int), compare_int_full, NULL, thread_count);
    CU_ASSERT_EQUAL(arr[0], 77);
    CU_ASSERT_EQUAL(arr[1], 98);
    // printf("thread_count=%d msort:\n", thread_count);
    parallel_msort_r(arr, 2, sizeof(int), compare_int_full, NULL, thread_count);
    CU_ASSERT_EQUAL(arr[0], 77);
    CU_ASSERT_EQUAL(arr[1], 98);
  }
}

/* Simple test of sort with two-element-array descending
 */
void test_sort_two_desc() {
  int arr[] = { 98, 77 };
  for (int thread_count = 1; thread_count < 5; thread_count++) {
    // printf("thread_count=%d hsort:\n", thread_count);
    parallel_hsort_r(arr, 2, sizeof(int), compare_int_full, NULL, thread_count);
    CU_ASSERT_EQUAL(arr[0], 77);
    CU_ASSERT_EQUAL(arr[1], 98);
    arr[0] = 98;
    arr[1] = 77;
    // printf("thread_count=%d hsort:\n", thread_count);
    parallel_ternary_hsort_r(arr, 2, sizeof(int), compare_int_full, NULL, thread_count);
    CU_ASSERT_EQUAL(arr[0], 77);
    CU_ASSERT_EQUAL(arr[1], 98);
    arr[0] = 98;
    arr[1] = 77;
    // printf("thread_count=%d hsort:\n", thread_count);
    parallel_fsort_r(arr, 2, sizeof(int), compare_int_full, NULL, metric_int_full, NULL, thread_count);
    CU_ASSERT_EQUAL(arr[0], 77);
    CU_ASSERT_EQUAL(arr[1], 98);
    arr[0] = 98;
    arr[1] = 77;
    // printf("thread_count=%d hsort:\n", thread_count);
    parallel_qsort_r(arr, 2, sizeof(int), compare_int_full, NULL, thread_count);
    CU_ASSERT_EQUAL(arr[0], 77);
    CU_ASSERT_EQUAL(arr[1], 98);
    arr[0] = 98;
    arr[1] = 77;
    // printf("thread_count=%d hsort:\n", thread_count);
    parallel_isort_r(arr, 2, sizeof(int), compare_int_full, NULL, thread_count);
    CU_ASSERT_EQUAL(arr[0], 77);
    CU_ASSERT_EQUAL(arr[1], 98);
    arr[0] = 98;
    arr[1] = 77;
    // printf("thread_count=%d hsort:\n", thread_count);
    parallel_msort_r(arr, 2, sizeof(int), compare_int_full, NULL, thread_count);
    CU_ASSERT_EQUAL(arr[0], 77);
    CU_ASSERT_EQUAL(arr[1], 98);
  }
}

/* Simple test of sort with two-element-array ascending
 */
void test_sort_two_same() {
  int arr[] = { 88, 88 };
  for (int thread_count = 1; thread_count < 5; thread_count++) {
    parallel_hsort_r(arr, 2, sizeof(int), compare_int_full, NULL, thread_count);
    CU_ASSERT_EQUAL(arr[0], 88);
    CU_ASSERT_EQUAL(arr[1], 88);
    parallel_ternary_hsort_r(arr, 2, sizeof(int), compare_int_full, NULL, thread_count);
    CU_ASSERT_EQUAL(arr[0], 88);
    CU_ASSERT_EQUAL(arr[1], 88);
    parallel_fsort_r(arr, 2, sizeof(int), compare_int_full, NULL, metric_int_full, NULL, thread_count);
    CU_ASSERT_EQUAL(arr[0], 88);
    CU_ASSERT_EQUAL(arr[1], 88);
    parallel_qsort_r(arr, 2, sizeof(int), compare_int_full, NULL, thread_count);
    CU_ASSERT_EQUAL(arr[0], 88);
    CU_ASSERT_EQUAL(arr[1], 88);
    parallel_isort_r(arr, 2, sizeof(int), compare_int_full, NULL, thread_count);
    CU_ASSERT_EQUAL(arr[0], 88);
    CU_ASSERT_EQUAL(arr[1], 88);
    parallel_msort_r(arr, 2, sizeof(int), compare_int_full, NULL, thread_count);
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

void print_arr(int *arr, int n) {
  printf("[");
  for (int i = 0; i < n; i++) {
    printf(" %d ", arr[i]);
  }
  printf("]");
}

void check_three(int i, int j, int k) {
  int harr[3];
  int qarr[3];
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

  for (int thread_count = 1; thread_count < 5; thread_count++) {
    qarr[0] = i;
    qarr[1] = j;
    qarr[2] = k;
    parallel_qsort_r(qarr, 3, sizeof(int), compare_int_full, NULL, thread_count);
    /* make some incomplete check anyway */
    CU_ASSERT(master_arr[0] <= master_arr[1]);
    CU_ASSERT(master_arr[1] <= master_arr[2]);

    harr[0] = i;
    harr[1] = j;
    harr[2] = k;
    parallel_hsort_r(harr, 3, sizeof(int), compare_int_full, NULL, thread_count);
    CU_ASSERT_EQUAL(harr[0], master_arr[0]);
    CU_ASSERT_EQUAL(harr[1], master_arr[1]);
    CU_ASSERT_EQUAL(harr[2], master_arr[2]);

    tarr[0] = i;
    tarr[1] = j;
    tarr[2] = k;
    parallel_ternary_hsort_r(tarr, 3, sizeof(int), compare_int_full, NULL, thread_count);
    CU_ASSERT_EQUAL(tarr[0], master_arr[0]);
    CU_ASSERT_EQUAL(tarr[1], master_arr[1]);
    CU_ASSERT_EQUAL(tarr[2], master_arr[2]);

    farr[0] = i;
    farr[1] = j;
    farr[2] = k;
    parallel_fsort_r(farr, 3, sizeof(int), compare_int_full, NULL, metric_int_full, NULL, thread_count);
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
    parallel_isort_r(iarr, 3, sizeof(int), compare_int_full, NULL, thread_count);
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
    parallel_msort_r(marr, 3, sizeof(int), compare_int_full, NULL, thread_count);
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
  int qarr[imax];
  int iarr[imax];
  int marr[imax];
  int harr[imax];
  int tarr[imax];
  int farr[imax];

  for (int i = 0; i <= imax; i++) {
    for (int j = 0; j < imax; j++) {
      sorted_arr[j] = master_arr[j];
    }
    qsort_r(sorted_arr, i, sizeof(int), compare_int_full, NULL);
    for (int thread_count = 1; thread_count < 5; thread_count++) {
      for (int j = 0; j < i; j++) {
        qarr[j] = master_arr[j];
      }
      parallel_qsort_r(qarr, i, sizeof(int), compare_int_full, NULL, thread_count);
      for (int j = 0; j < i; j++) {
        if (qarr[j] != sorted_arr[j]) {
          printf("check_parallel_n: qsort: i=%d j=%d: qarr[j]=%d (%d)\n", i, j, qarr[j], sorted_arr[j]);
        }
        CU_ASSERT_EQUAL(qarr[j], qarr[j]);
      }
      for (int j = 0; j < i; j++) {
        iarr[j] = master_arr[j];
      }
      parallel_isort_r(iarr, i, sizeof(int), compare_int_full, NULL, thread_count);
      for (int j = 0; j < i; j++) {
        if (iarr[j] != sorted_arr[j]) {
          printf("check_parallel_n: isort: i=%d j=%d: iarr[j]=%d (%d)\n", i, j, iarr[j], sorted_arr[j]);
        }
        CU_ASSERT_EQUAL(iarr[j], sorted_arr[j]);
      }
      for (int j = 0; j < i; j++) {
        marr[j] = master_arr[j];
      }
      parallel_msort_r(marr, i, sizeof(int), compare_int_full, NULL, thread_count);
      for (int j = 0; j < i; j++) {
        if (marr[j] != sorted_arr[j]) {
          printf("check_parallel_n: msort: i=%d j=%d: marr[j]=%d (%d)\n", i, j, marr[j], sorted_arr[j]);
        }
        CU_ASSERT_EQUAL(marr[j], sorted_arr[j]);
      }
      for (int j = 0; j < i; j++) {
        harr[j] = master_arr[j];
      }
      parallel_hsort_r(harr, i, sizeof(int), compare_int_full, NULL, thread_count);
      for (int j = 0; j < i; j++) {
        if (harr[j] != sorted_arr[j]) {
          printf("check_parallel_n: ternary_hsort: i=%d j=%d: harr[j]=%d (%d)\n", i, j, harr[j], sorted_arr[j]);
        }
        CU_ASSERT_EQUAL(harr[j], sorted_arr[j]);
      }
      for (int j = 0; j < i; j++) {
        tarr[j] = master_arr[j];
      }
      parallel_ternary_hsort_r(tarr, i, sizeof(int), compare_int_full, NULL, thread_count);
      for (int j = 0; j < i; j++) {
        if (tarr[j] != sorted_arr[j]) {
          printf("check_parallel_n: ternary_hsort: i=%d j=%d: tarr[j]=%d (%d)\n", i, j, tarr[j], sorted_arr[j]);
        }
        CU_ASSERT_EQUAL(tarr[j], sorted_arr[j]);
      }
      for (int j = 0; j < i; j++) {
        farr[j] = master_arr[j];
      }
      parallel_fsort_r(farr, i, sizeof(int), compare_int_full, NULL, metric_int_full, NULL, thread_count);
      for (int j = 0; j < i; j++) {
        if (farr[j] != sorted_arr[j]) {
          printf("check_parallel_n: fsort: i=%d j=%d: farr[j]=%d (%d)\n", i, j, farr[j], sorted_arr[j]);
        }
        CU_ASSERT_EQUAL(farr[j], sorted_arr[j]);
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
