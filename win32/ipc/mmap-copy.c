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
  HANDLE sh = CreateFile(argv[1],
                         GENERIC_READ | GENERIC_WRITE, /* or GENERIC_READ: dwAccess */
                         0, /* share mode: not shared: dwShareMode */
                         NULL, /* lp security attributes (here alway null) */
                         OPEN_ALWAYS, /* or CREATE_NEW OPEN_EXISTING, OPEN_ALWAYS, TRUNCATE_EXISTING dwCreate */
                         FILE_ATTRIBUTE_NORMAL, /* or FILE_ATTRIBUTE_READONLY or FILE_FLAGE_DELETE_ON_CLOSE or FILE_FLAG_OVERLAPPED (assync I/O) .... dwAttrAndFlags */
                         NULL); /* hTemplateFile */
  HandleError("CreateFile", (sh == INVALID_HANDLE_VALUE));
  HANDLE th = CreateFile(argv[2],
                         GENERIC_READ | GENERIC_WRITE, /* or GENERIC_READ: dwAccess */
                         0, /* share mode: not shared: dwShareMode */
                         NULL, /* lp security attributes (here alway null) */
                         OPEN_ALWAYS, /* or CREATE_NEW OPEN_EXISTING, OPEN_ALWAYS, TRUNCATE_EXISTING dwCreate */
                         FILE_ATTRIBUTE_NORMAL, /* or FILE_ATTRIBUTE_READONLY or FILE_FLAGE_DELETE_ON_CLOSE or FILE_FLAG_OVERLAPPED (assync I/O) .... dwAttrAndFlags */
                         NULL); /* hTemplateFile */
  HandleError("CreateFile", (th == INVALID_HANDLE_VALUE));
  printf("files opened");

  DWORD lsize = 0;
  DWORD hsize = 0;

  lsize = GetFileSize(sh, &hsize);
  printf("filesize=%d (%d)\n", (int) lsize, (int) hsize);

  HANDLE smap = CreateFileMapping(sh, /*  _In_     HANDLE                hFile, */
                                  NULL, /* _In_opt_ LPSECURITY_ATTRIBUTES lpAttributes, */
                                  PAGE_READWRITE,  /* _In_     DWORD                 flProtect, */
                                  hsize, /* _In_     DWORD                 dwMaximumSizeHigh, */
                                  lsize, /* _In_     DWORD                 dwMaximumSizeLow, */
                                  NULL); /* _In_opt_ LPCTSTR               lpName */

  HANDLE tmap = CreateFileMapping(th, /*  _In_     HANDLE                hFile, */
                                  NULL, /* _In_opt_ LPSECURITY_ATTRIBUTES lpAttributes, */
                                  PAGE_READWRITE,  /* _In_     DWORD                 flProtect, */
                                  hsize, /* _In_     DWORD                 dwMaximumSizeHigh, */
                                  lsize, /* _In_     DWORD                 dwMaximumSizeLow, */
                                  NULL); /* _In_opt_ LPCTSTR               lpName */

  char * sptr = (char *) MapViewOfFile(smap, /*   _In_ HANDLE hFileMappingObject, */
                                     FILE_MAP_ALL_ACCESS, /* _In_ DWORD  dwDesiredAccess, */
                                     0, /* _In_ DWORD  dwFileOffsetHigh, */
                                     0, /* _In_ DWORD  dwFileOffsetLow, */
                                     lsize); /* _In_ SIZE_T dwNumberOfBytesToMap */

  char * tptr = (char *) MapViewOfFile(tmap, /*   _In_ HANDLE hFileMappingObject, */
                                     FILE_MAP_ALL_ACCESS, /* _In_ DWORD  dwDesiredAccess, */
                                     0, /* _In_ DWORD  dwFileOffsetHigh, */
                                     0, /* _In_ DWORD  dwFileOffsetLow, */
                                     lsize); /* _In_ SIZE_T dwNumberOfBytesToMap */

  int i;
  for (i = 0; i < lsize; i++) {
    tptr[i] = sptr[i];
  }
  
  UnmapViewOfFile(sptr);
  UnmapViewOfFile(tptr);

  CloseHandle(smap);
  CloseHandle(tmap);

  CloseHandle(sh);
  CloseHandle(th);
}

