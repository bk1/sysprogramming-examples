/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 *
 * Windows IO
 *
 * depending on option uses exclusive or non-exclusive handle for opening file
 */

#include <windows.h>
#include <string.h>



#define BUFFER_SIZE 1000000

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

  DWORD shareMode = 0;
  if (argc > 1 && strcmp(argv[1], "-s") == 0) {
    shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
  }
  BOOL result;
  const char *fileName = "../temp/test-w-twice.txt";

  HANDLE fh1;

  DWORD errorCode;
  DWORD nWritten;

  fh1 = CreateFile(fileName,         /* lpName */
                   GENERIC_READ, /* or GENERIC_READ: dwAccess */
                   shareMode, /* share mode: not shared: dwShareMode */
                   NULL, /* lp security attributes (here alway null) */
                   CREATE_ALWAYS, /* or CREATE_NEW OPEN_EXISTING, OPEN_ALWAYS, TRUNCATE_EXISTING dwCreate */
                   FILE_ATTRIBUTE_NORMAL, /* or FILE_ATTRIBUTE_READONLY or FILE_FLAGE_DELETE_ON_CLOSE or FILE_FLAG_OVERLAPPED (assync I/O) .... dwAttrAndFlags */
                   NULL); /* hTemplateFile */
  HandleError("fh1=CreateFile(...) -> errorCode=%ld\n", (fh1 == INVALID_HANDLE_VALUE));

  char buffer[BUFFER_SIZE];

  DWORD n;
  result = ReadFile(fh1, buffer, BUFFER_SIZE, &n, NULL);
  HandleError("ReadFile(fh1...)", ! result);

  Sleep(30000);

  result = CloseHandle(fh1);
  HandleError("CloseHandle(fh1)", ! result);

  ExitProcess(0);
}

