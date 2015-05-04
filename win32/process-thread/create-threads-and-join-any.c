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


DWORD WINAPI MyThreadFunction(LPVOID param);

DWORD threadId[5];

int main(int argc, char *argv[]) {
  BOOL result;

  HANDLE fh1, fh2;

  DWORD errorCode;
  DWORD nWritten;
  
  HANDLE threadHandle[5];

  DWORD param[5];

  DWORD MAX_TIME = 100000;

  int i;
  for (i = 0; i < 5; i++) {
    param[i] = rand() % MAX_TIME;
    printf("creating thread i=%d p=%d\n", i, param[i]);
    threadHandle[i] = CreateThread(NULL, /* LPSECURITY_ATTRIBUTES lpThreadAttributes, */
                                   0, /* SIZE_T dwStackSize, */
                                   MyThreadFunction, /* LPTHREAD_START_ROUTINE lpStartAddress, */
                                   &param[i], /* LPVOID lpParameter, */
                                   0, /* DWORD dwCreationFlags, */
                                   &threadId[i]); /* LPDWORD lpThreadId */
    printf("thread created threadId=%d i=%d p=%d\n", threadId[i], i, param[i]);
  }
  Sleep(1000);
  for (i = 0; i  < 5; i++) {
    DWORD result = WaitForSingleObject(threadHandle[i], MAX_TIME);
    printf("thread %d result=%ud ", i, result);
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
  }
  Sleep(3000);
  ExitProcess(0);
}


DWORD WINAPI MyThreadFunction(LPVOID param) {
  LONG idx = InterlockedIncrement(&count);
  DWORD tid = GetCurrentThreadId();
  DWORD *iparam = (DWORD *) param;
  DWORD t = *iparam;
  printf("in thread %d\n", (int) idx);
  Sleep(t);
  printf("done with thread %d\n", (int) idx);
  return 0;
}
