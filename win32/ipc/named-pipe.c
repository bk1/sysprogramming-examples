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
  const char *msg = "Hello, World!\n(CreateFile/WriteFile/CloseHandle)\n";
  const char *pipeName = "\\\\.\\pipe\\test-p";
  char buffer[1024];
  HANDLE sh = CreateNamedPipe(pipeName,
                              PIPE_ACCESS_DUPLEX,
                              PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS,
                              10,
                              0,
                              0,
                              NMPWAIT_USE_DEFAULT_WAIT,
                              NULL);
  HandleError("CreateNamedPipe", (sh == INVALID_HANDLE_VALUE));
  printf("pipe created\n");
  result = ConnectNamedPipe(sh, NULL);
  HandleError("ConnectNamedPipe",  !result);
  printf("pipe connected\n");
  
  HANDLE ch = CreateFile(pipeName,         /* lpName */
                         GENERIC_WRITE, /* or GENERIC_READ: dwAccess */
                         0, /* share mode: not shared: dwShareMode */
                         NULL, /* lp security attributes (here alway null) */
                         OPEN_ALWAYS, /* or CREATE_NEW OPEN_EXISTING, OPEN_ALWAYS, TRUNCATE_EXISTING dwCreate */
                         FILE_ATTRIBUTE_NORMAL, /* or FILE_ATTRIBUTE_READONLY or FILE_FLAGE_DELETE_ON_CLOSE or FILE_FLAG_OVERLAPPED (assync I/O) .... dwAttrAndFlags */
                         NULL); /* hTemplateFile */
  HandleError("CreateFile", (ch == INVALID_HANDLE_VALUE));
  printf("pipe client connected\n");

  DWORD nWritten = 0;
  result = WriteFile(sh, /* HANDLE */
                     (void *) msg, /* lpBuffer */
                     (DWORD) strlen(msg), /* nNumberOfBytesToWrite */
                     &nWritten, /* lpNumberOfBytesWritten */
                     NULL); /* lpOverlapped */
  HandleError("WriteFile", ! result);
  printf("written");

  DWORD nRead = 0;
  result = ReadFile(ch, /* HANDLE */
                    (void *) buffer, /* lpBuffer */
                    (DWORD) 1023, /* nNumberOfBytesToRead */
                     &nRead, /* lpNumberOfBytesWritten */
                     NULL); /* lpOverlapped */
  HandleError("ReadFile(...)", ! result);
  result = CloseHandle(sh);
  HandleError("CloseHandle(sh)", ! result);
  printf("%d bytes read\n", (int) nRead);

  buffer[nRead] = (char) 0;

  result = CloseHandle(ch);
  HandleError("CloseHandle(ch)", ! result);
  printf("%s\n", buffer);
}

