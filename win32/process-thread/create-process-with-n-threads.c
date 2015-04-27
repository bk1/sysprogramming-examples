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
  
  PROCESS_INFORMATION processInformation;
  STARTUPINFO startupInfo;

  GetStartupInfo(&startupInfo);
  result = CreateProcess("child-process-with-n-threads.exe", /* LPCTSTR lpApplicationName, */
                            "", /* LPTSTR lpCommandLine, */
                            NULL, /* LPSECURITY_ATTRIBUTES lpProcessAttributes, */
                            NULL, /* LPSECURITY_ATTRIBUTES lpThreadAttributes, */
                            FALSE, /* bInheritHandles, */
                         0, /* CREATE_NEW_CONSOLE,  DWORD dwCreationFlags, */
                            NULL, /* LPVOID lpEnvironment, */
                            NULL, /* LPCTSTR lpCurrentDirectory,*/
                            &startupInfo, /* LPSTARTUPINFO lpStartupInfo, */
                            &processInformation); /* LPPROCESS_INFORMATION lpProcessInformation */
  printf("in parent process\n");
  Sleep(5000);
  printf("terminating child\n");
  TerminateProcess(processInformation.hProcess, 0);
  printf("child terminated");

  ExitProcess(0);
}

