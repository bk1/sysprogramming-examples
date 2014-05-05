/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 *
 * create a directory
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
  BOOL result;
  const char *msg1 = "Hello, World!\n(HANDLE)\n";
  const char *goodDirName = "../temp/testdirectory";
  const char *badDirName = "../tempx/testdirectory";

  DWORD errorCode;
  HANDLE stdout_handle;

  result = CreateDirectory(goodDirName, NULL);
  HandleError("CreateDirectory()", ! result);
  printf("CreateDirectory() successful\n");

  result = CreateDirectory(goodDirName, NULL);
  errorCode = GetLastError();
  printf("CreateDirectory() again failed: result=%d errorCode=%d (errorCode==ERROR_ALREADY_EXISTS)=%d\n", result, errorCode, (errorCode == ERROR_ALREADY_EXISTS));
  
  result = CreateDirectory(badDirName, NULL);
  errorCode = GetLastError();
  printf("CreateDirectory() again failed: result=%d errorCode=%d (errorCode==ERROR_PATH_NOT_FOUND)=%d\n", result, errorCode, (errorCode == ERROR_PATH_NOT_FOUND));

  result = RemoveDirectory(goodDirName);
  HandleError("RemoveDirectory()", ! result);
  printf("RemoveDirectory() successful\n");

  result = RemoveDirectory(goodDirName);
  errorCode = GetLastError();
  printf("RemoveDirectory() again failed: result=%d errorCode=%d (errorCode==ERROR_FILE_NOT_FOUND)=%d\n", result, errorCode, (errorCode == ERROR_FILE_NOT_FOUND));
  
  result = RemoveDirectory(badDirName);
  errorCode = GetLastError();
  printf("RemoveDirectory() again failed: result=%d errorCode=%d (errorCode==ERROR_PATH_NOT_FOUND)=%d\n", result, errorCode, (errorCode == ERROR_PATH_NOT_FOUND));
  
  ExitProcess(0);
}

