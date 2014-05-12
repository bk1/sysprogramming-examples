/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 *
 * access console even when 
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
  const char *msg = "Hello, World!\n(CreateFile/WriteFile/CloseHandle)\n";
  const char *fileName = "../temp/test-w-not2.txt";
  char buffer[1024];


  HANDLE stdout_handle;

  DWORD errorCode;
  DWORD nWritten;

  const char *msg1 = "this is a short message";
  Sleep(10000);
  result = FreeConsole();
  result = AllocConsole();
  stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);


  result = WriteFile(stdout_handle, /* HANDLE */
                     (void *) msg1, /* lpBuffer */
                     (DWORD) strlen(msg1), /* nNumberOfBytesToWrite */
                     &nWritten, /* lpNumberOfBytesWritten */
                     NULL); /* lpOverlapped */
  HandleError("WriteFile()\n", ! result);

  printf("n=%d\n", nWritten);

  Sleep(10000);
  ExitProcess(0);
}

