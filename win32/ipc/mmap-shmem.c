/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2015-05-11
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 *
 * Windows IPC: named pipe
 */




#include <windows.h>
#include <string.h>

VOID HandleError(LPCTSTR msg, BOOL failure) {
  if (failure) {
    DWORD errorCode = GetLastError();
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
  HANDLE sh = CreateFile("../temp/mmapfile.dat",
                         GENERIC_READ | GENERIC_WRITE, /* or GENERIC_READ: dwAccess */
                         FILE_SHARE_READ | FILE_SHARE_WRITE, /* share mode: not shared: dwShareMode */
                         NULL, /* lp security attributes (here alway null) */
                         OPEN_ALWAYS, /* or CREATE_NEW OPEN_EXISTING, OPEN_ALWAYS, TRUNCATE_EXISTING dwCreate */
                         FILE_ATTRIBUTE_NORMAL, /* or FILE_ATTRIBUTE_READONLY or FILE_FLAGE_DELETE_ON_CLOSE or FILE_FLAG_OVERLAPPED (assync I/O) .... dwAttrAndFlags */
                         NULL); /* hTemplateFile */
  HandleError("CreateFile", (sh == INVALID_HANDLE_VALUE));
  printf("file opened\n");

  DWORD lsize = 1000;
  DWORD hsize = 0;

  HANDLE smap = CreateFileMapping(sh, /*  _In_     HANDLE                hFile, */
                                  NULL, /* _In_opt_ LPSECURITY_ATTRIBUTES lpAttributes, */
                                  PAGE_READWRITE,  /* _In_     DWORD                 flProtect, */
                                  hsize, /* _In_     DWORD                 dwMaximumSizeHigh, */
                                  lsize, /* _In_     DWORD                 dwMaximumSizeLow, */
                                  "Global\mymapping"); /* _In_opt_ LPCTSTR               lpName */

  
  PROCESS_INFORMATION processInformation1;
  PROCESS_INFORMATION processInformation2;
  STARTUPINFO startupInfo;

  GetStartupInfo(&startupInfo);

  printf("starting children\n");

  result = CreateProcess("mmap-shmem-1.exe", /* LPCTSTR lpApplicationName, */
                         "", /* LPTSTR lpCommandLine, */
                         NULL, /* LPSECURITY_ATTRIBUTES lpProcessAttributes, */
                         NULL, /* LPSECURITY_ATTRIBUTES lpThreadAttributes, */
                         FALSE, /* bInheritHandles, */
                         0, /* CREATE_NEW_CONSOLE,  DWORD dwCreationFlags, */
                         NULL, /* LPVOID lpEnvironment, */
                         NULL, /* LPCTSTR lpCurrentDirectory,*/
                         &startupInfo, /* LPSTARTUPINFO lpStartupInfo, */
                         &processInformation1); /* LPPROCESS_INFORMATION lpProcessInformation */
  HandleError("first child start", ! result);
  printf("in parent process, 1st child started\n");

  result = CreateProcess("mmap-shmem-2.exe", /* LPCTSTR lpApplicationName, */
                         "", /* LPTSTR lpCommandLine, */
                         NULL, /* LPSECURITY_ATTRIBUTES lpProcessAttributes, */
                         NULL, /* LPSECURITY_ATTRIBUTES lpThreadAttributes, */
                         FALSE, /* bInheritHandles, */
                         0, /* CREATE_NEW_CONSOLE,  DWORD dwCreationFlags, */
                         NULL, /* LPVOID lpEnvironment, */
                         NULL, /* LPCTSTR lpCurrentDirectory,*/
                         &startupInfo, /* LPSTARTUPINFO lpStartupInfo, */
                         &processInformation2); /* LPPROCESS_INFORMATION lpProcessInformation */
  HandleError("second child start", ! result);
  printf("in parent process, 2nd child started\n");
  
  WaitForSingleObject(processInformation1.hProcess, INFINITE);
  printf("process 1 terminated\n");
  WaitForSingleObject(processInformation2.hProcess, INFINITE);
  printf("process 2 terminated\n");

  ExitProcess(0);
}

