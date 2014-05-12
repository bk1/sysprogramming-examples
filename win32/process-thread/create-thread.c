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


int main(int argc, char *argv[]) {
  BOOL result;
  const char *msg1 = "Hello, World!\n(CreateFile/WriteFile/CloseHandle)\n++++++++++++++(1)\n\n";
  const char *msg2 = "Hello, World!\n(CreateFile/WriteFile/CloseHandle)\n(2)\n\n";
  const char *fileName = "../temp/test-w-twice.txt";

  HANDLE fh1, fh2;

  DWORD errorCode;
  DWORD nWritten;
  
  HANDLE threadHandle;
  DWORD threadId;


  threadHandle = CreateThread(NULL, /* LPSECURITY_ATTRIBUTES lpThreadAttributes, */
                               0, /* SIZE_T dwStackSize, */
                               MyThreadFunction, /* LPTHREAD_START_ROUTINE lpStartAddress, */
                               NULL, /* LPVOID lpParameter, */
                               0, /* DWORD dwCreationFlags, */
                               &threadId); /* LPDWORD lpThreadId */
  printf("thread created threadId=%d\n", threadId);
  Sleep(3000);
  printf("ending thread from parent\n");
  TerminateThread(threadHandle, 0);
  printf("terminated thread\n");
  Sleep(3000);

  ExitProcess(0);
}


DWORD WINAPI MyThreadFunction(LPVOID param) {
  printf("in thread\n");
  Sleep(5000);
  printf("done with thread\n");
  return 0;
}
