/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 *
 * Windows IO compiles, but crashes
 */




#include <windows.h>
#include <string.h>

int main(int argc, char *argv[]) {
  BOOL result;
  const char *msg = "Hello, World!\n(CreateFile/WriteFile/CloseHandle)\n";
  const char *fileName = "../temp/test-w.txt";
  HANDLE fh = CreateFile(fileName,         /* lpName */
                         GENERIC_WRITE, /* or GENERIC_READ: dwAccess */
                         0, /* share mode: not shared: dwShareMode */
                         NULL, /* lp security attributes (here alway null) */
                         CREATE_ALWAYS, /* or CREATE_NEW OPEN_EXISTING, OPEN_ALWAYS, TRUNCATE_EXISTING dwCreate */
                         FILE_ATTRIBUTE_NORMAL, /* or FILE_ATTRIBUTE_READONLY or FILE_FLAGE_DELETE_ON_CLOSE or FILE_FLAG_OVERLAPPED (assync I/O) .... dwAttrAndFlags */
                         NULL); /* hTemplateFile */
  DWORD nWritten = 0;
  result = WriteFile(fh, /* HANDLE */
                     (void *) msg, /* lpBuffer */
                     (DWORD) strlen(msg), /* nNumberOfBytesToWrite */
                     &nWritten, /* lpNumberOfBytesWritten */
                     NULL); /* lpOverlapped */
  result = CloseHandle(fh);
  if (! result) {
    /* todo */
  }
}

