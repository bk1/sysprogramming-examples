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

  HANDLE fh1;

  DWORD errorCode;
  DWORD nWritten;

  fh1 = CreateFile("CONOUT$",          /* lpName */
                   GENERIC_WRITE, /* or GENERIC_READ: dwAccess */
                   0, /* share mode: not shared: dwShareMode */
                   NULL, /* lp security attributes (here alway null) */
                   CREATE_ALWAYS, /* or CREATE_NEW OPEN_EXISTING, OPEN_ALWAYS, TRUNCATE_EXISTING dwCreate */
                   FILE_ATTRIBUTE_NORMAL, /* or FILE_ATTRIBUTE_READONLY or FILE_FLAGE_DELETE_ON_CLOSE or FILE_FLAG_OVERLAPPED (assync I/O) .... dwAttrAndFlags */
                   NULL); /* hTemplateFile */
  HandleError("fh1=CreateFile(...) -> errorCode=%ld\n", (fh1 == INVALID_HANDLE_VALUE));

  nWritten = 0;
  result = WriteFile(fh1, /* HANDLE */
                     (void *) msg, /* lpBuffer */
                     (DWORD) strlen(msg), /* nNumberOfBytesToRead */
                     &nWritten, /* lpNumberOfBytesWritten */
                     NULL); /* lpOverlapped */
  HandleError("WriteFile(...)", ! result);
  result = CloseHandle(fh1);
  HandleError("CloseHandle(fh1)", ! result);

  printf("n=%d\n", nWritten);

  ExitProcess(0);
}

