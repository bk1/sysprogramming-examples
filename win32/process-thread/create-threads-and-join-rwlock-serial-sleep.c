/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 *
 * create a process
 */

#include <windows.h>
#include <string.h>

#include <stdio.h>

#include <stdlib.h>

LONG count = 0;

SRWLOCK srwLock;

struct tdata {
  DWORD idx;
  LONG  tnum;
  DWORD duration;
  DWORD tid;
  HANDLE handle;
};

VOID HandleError(LPCTSTR msg, BOOL failure) {
  if (failure) {
    DWORD errorCode =GetLastError();
    LPTSTR errMsg;
    DWORD msgLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,  
                                 NULL, 
                                 errorCode, 
                                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                 (LPTSTR) &errMsg, 
                                 0, 
                                 NULL);
    if (msgLen > 0) {
      printf("%s: errorCode=%ld %s\n", msg, (long) errorCode, errMsg);
    } else {
      printf("%s: errorCode=%ld\n", msg, (long) errorCode);
    }
    if (errMsg != NULL) {
      LocalFree(errMsg);
    }
    ExitProcess(1);
  }
}


VOID MySleep(DWORD duration) {
  printf("wanting to sleep %d\n", duration);
  AcquireSRWLockExclusive(&srwLock);
  printf("starting to sleep %d\n", duration);
  Sleep(duration);
  printf("slept %d\n", duration);
  ReleaseSRWLockExclusive(&srwLock);
  printf("released mutex after having slept %d\n", duration);
}

DWORD WINAPI MyThreadFunction(LPVOID param);

DWORD threadId[5];

int main(int argc, char *argv[]) {
  BOOL result;

  DWORD errorCode;

  InitializeSRWLock(&srwLock);

  struct tdata threadData[5];

  DWORD MAX_TIME = 10000;

  int i;
  for (i = 0; i < 5; i++) {
    DWORD duration = rand() % MAX_TIME;
    threadData[i].duration = duration;
    threadData[i].idx = i;
    printf("creating thread i=%d d=%d\n", i, duration);
    threadData[i].handle = CreateThread(NULL, /* LPSECURITY_ATTRIBUTES lpThreadAttributes, */
                                   0, /* SIZE_T dwStackSize, */
                                   MyThreadFunction, /* LPTHREAD_START_ROUTINE lpStartAddress, */
                                   &threadData[i], /* LPVOID lpParameter, */
                                   0, /* DWORD dwCreationFlags, */
                                   &threadData[i].tid); /* LPDWORD lpThreadId */
    printf("thread created i=%d threadId=%d i=%d\n", i, threadData[i].tid);
  }
  MySleep(1000);
  HANDLE handles[5];
  for (i = 0; i < 5; i++) {
    handles[i] = threadData[i].handle;
  }
  result = WaitForMultipleObjects(5, handles, TRUE, 10*MAX_TIME);
  printf("threads waited result=%ud ", result);
  switch (result) {
  case WAIT_ABANDONED:
    printf("WAIT_ABANDONED");
    break;
  case WAIT_OBJECT_0:
    printf("WAIT_OBJECT_0");
    break;
  case WAIT_TIMEOUT:
    printf("WAIT_TIMEOUT");
    break;
  case WAIT_FAILED:
    printf("WAIT_FAILED");
    break;
  default:
    printf("OTHER");
  }
  printf("\n");
  MySleep(3000);
  ExitProcess(0);
}


DWORD WINAPI MyThreadFunction(LPVOID param) {
  struct tdata *tdata = (struct tdata *) param;
  LONG num = InterlockedIncrement(&count);
  tdata->tnum = num;
  DWORD duration = tdata->duration;
  printf("in thread i=%d n=%d t=%d\n", (int) tdata->idx, tdata->tnum, tdata->tid);
  MySleep(duration);
  printf("done with thread i=%d n=%d t=%d\n", (int) tdata->idx, tdata->tnum, tdata->tid);
  return 0;
}
