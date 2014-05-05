/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 *
 * Windows IO compiles and disallows using the same file with two different handles
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
  const char *msg1 = "Hello, World!\n(CreateFile/WriteFile/CloseHandle)\n++++++++++++++(1)\n\n";
  const char *msg2 = "Hello, World!\n(CreateFile/WriteFile/CloseHandle)\n(2)\n\n";
  const char *fileName = "../temp/test-w-twice.txt";

  HANDLE fh1, fh2;

  DWORD errorCode;
  DWORD nWritten;

  fh1 = CreateFile(fileName,         /* lpName */
                   GENERIC_WRITE, /* or GENERIC_READ: dwAccess */
                   FILE_SHARE_READ | FILE_SHARE_WRITE, /* share mode: not shared: dwShareMode */
                   NULL, /* lp security attributes (here alway null) */
                   CREATE_ALWAYS, /* or CREATE_NEW OPEN_EXISTING, OPEN_ALWAYS, TRUNCATE_EXISTING dwCreate */
                   FILE_ATTRIBUTE_NORMAL, /* or FILE_ATTRIBUTE_READONLY or FILE_FLAGE_DELETE_ON_CLOSE or FILE_FLAG_OVERLAPPED (assync I/O) .... dwAttrAndFlags */
                   NULL); /* hTemplateFile */
  HandleError("fh1=CreateFile(...) -> errorCode=%ld\n", (fh1 == INVALID_HANDLE_VALUE));

  fh2 = CreateFile(fileName,         /* lpName */
                   GENERIC_WRITE, /* or GENERIC_READ: dwAccess */
                   FILE_SHARE_READ | FILE_SHARE_WRITE, /* share mode: not shared: dwShareMode */
                   NULL, /* lp security attributes (here alway null) */
                   CREATE_ALWAYS, /* or CREATE_NEW OPEN_EXISTING, OPEN_ALWAYS, TRUNCATE_EXISTING dwCreate */
                   FILE_ATTRIBUTE_NORMAL, /* or FILE_ATTRIBUTE_READONLY or FILE_FLAGE_DELETE_ON_CLOSE or FILE_FLAG_OVERLAPPED (assync I/O) .... dwAttrAndFlags */
                   NULL); /* hTemplateFile */
  HandleError("fh2=CreateFile(...) -> errorCode=%ld\n", (fh2 == INVALID_HANDLE_VALUE));

  nWritten = 0;
  result = WriteFile(fh1, /* HANDLE */
                     (void *) msg1, /* lpBuffer */
                     (DWORD) strlen(msg1), /* nNumberOfBytesToWrite */
                     &nWritten, /* lpNumberOfBytesWritten */
                     NULL); /* lpOverlapped */
  HandleError("WriteFile(fh1...)", ! result);

  nWritten = 0;
  result = WriteFile(fh2, /* HANDLE */
                     (void *) msg2, /* lpBuffer */
                     (DWORD) strlen(msg2), /* nNumberOfBytesToWrite */
                     &nWritten, /* lpNumberOfBytesWritten */
                     NULL); /* lpOverlapped */
  HandleError("WriteFile(fh2...)", ! result);

  result = CloseHandle(fh2);
  HandleError("CloseHandle(fh2)", ! result);

  result = CloseHandle(fh1);
  HandleError("CloseHandle(fh1)", ! result);

  ExitProcess(0);
}

