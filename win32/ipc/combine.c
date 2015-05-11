/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2015-05-11
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 *
 * Windows IPC: anonymous pipe
 */




#include <windows.h>
#include <string.h>

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

int main(int argc, char *argv[]) {
  BOOL result;
  HANDLE readPipe;
  HANDLE writePipe;
  result = CreatePipe(&readPipe, &writePipe, NULL, 0);
  HandleError("create pipe", ! result);
  
  PROCESS_INFORMATION processInformation1;
  PROCESS_INFORMATION processInformation2;
  STARTUPINFO startupInfo1;
  STARTUPINFO startupInfo2;


  printf("starting children\n");

  GetStartupInfo(&startupInfo1);
  startupInfo1.hStdOutput = writePipe;
  result = CreateProcess(argv[1],
                         "", /* LPTSTR lpCommandLine, */
                         NULL, /* LPSECURITY_ATTRIBUTES lpProcessAttributes, */
                         NULL, /* LPSECURITY_ATTRIBUTES lpThreadAttributes, */
                         FALSE, /* bInheritHandles, */
                         0, /* CREATE_NEW_CONSOLE,  DWORD dwCreationFlags, */
                         NULL, /* LPVOID lpEnvironment, */
                         NULL, /* LPCTSTR lpCurrentDirectory,*/
                         &startupInfo1, /* LPSTARTUPINFO lpStartupInfo, */
                         &processInformation1); /* LPPROCESS_INFORMATION lpProcessInformation */
  HandleError("first child start", ! result);
  printf("in parent process, 1st child started\n");

  GetStartupInfo(&startupInfo2);
  startupInfo2.hStdInput = readPipe;
  result = CreateProcess(argv[2],
                         "", /* LPTSTR lpCommandLine, */
                         NULL, /* LPSECURITY_ATTRIBUTES lpProcessAttributes, */
                         NULL, /* LPSECURITY_ATTRIBUTES lpThreadAttributes, */
                         FALSE, /* bInheritHandles, */
                         0, /* CREATE_NEW_CONSOLE,  DWORD dwCreationFlags, */
                         NULL, /* LPVOID lpEnvironment, */
                         NULL, /* LPCTSTR lpCurrentDirectory,*/
                         &startupInfo2, /* LPSTARTUPINFO lpStartupInfo, */
                         &processInformation2); /* LPPROCESS_INFORMATION lpProcessInformation */
  HandleError("second child start", ! result);
  printf("in parent process, 2nd child started\n");

  Sleep(1000);
  WaitForSingleObject(processInformation1.hProcess, INFINITE);
  printf("process 1 terminated\n");
  WaitForSingleObject(processInformation2.hProcess, INFINITE);
  printf("process 2 terminated\n");

  ExitProcess(0);
}

