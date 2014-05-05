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
  BOOL result;
  const char *msg1 = "Hello, World!\n(HANDLE)\n";
  const char *fileName = "../temp/test-redirect-stdout.txt";

  HANDLE fh1, fh2;

  DWORD errorCode;
  DWORD nWritten;
  HANDLE stdout_handle;

  stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
  HandleError("GetStdHandle()\n", stdout_handle == INVALID_HANDLE_VALUE);

  result = CloseHandle(stdout_handle);
  HandleError("CloseHandle()\n", ! result);
  

  fh1 = CreateFile(fileName,         /* lpName */
                   GENERIC_WRITE, /* or GENERIC_READ: dwAccess */
                   0, /* share mode: not shared: dwShareMode */
                   NULL, /* lp security attributes (here alway null) */
                   CREATE_ALWAYS, /* or CREATE_NEW OPEN_EXISTING, OPEN_ALWAYS, TRUNCATE_EXISTING dwCreate */
                   FILE_ATTRIBUTE_NORMAL, /* or FILE_ATTRIBUTE_READONLY or FILE_FLAGE_DELETE_ON_CLOSE or FILE_FLAG_OVERLAPPED (assync I/O) .... dwAttrAndFlags */
                   NULL); /* hTemplateFile */
  HandleError("fh1=CreateFile(...)\n", (fh1 == INVALID_HANDLE_VALUE));

  result = SetStdHandle(STD_OUTPUT_HANDLE, fh1);
  HandleError("SetStdHandle()\n", ! result);

  printf("Hello world to stdout (printf)\n");

  stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
  HandleError("GetStdHandle()\n", stdout_handle == INVALID_HANDLE_VALUE);

  result = WriteFile(stdout_handle, /* HANDLE */
                     (void *) msg1, /* lpBuffer */
                     (DWORD) strlen(msg1), /* nNumberOfBytesToWrite */
                     &nWritten, /* lpNumberOfBytesWritten */
                     NULL); /* lpOverlapped */
  HandleError("WriteFile()\n", ! result);

  CloseHandle(stdout_handle);
  
  ExitProcess(0);
}

