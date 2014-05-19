/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 *
 * redirect stdout
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
    if (errorCode != 0) {
      printf("exiting\n");
      ExitProcess(1);
    } else {
      printf("continue\n");
    }
  }
}

int main(int argc, char *argv[]) {
  BOOL result = TRUE;

  DWORD errorCode;
  
  WIN32_FIND_DATA findData;

  HANDLE search = FindFirstFile("*", &findData);
  
  if (search != INVALID_HANDLE_VALUE) {
    while (result) {
      printf("found file=%s (%s)\n", findData.cFileName, findData.cAlternateFileName);
      result = FindNextFile(search, &findData);
    }
  }
  printf("nothing found\n");

  ExitProcess(0);
}

