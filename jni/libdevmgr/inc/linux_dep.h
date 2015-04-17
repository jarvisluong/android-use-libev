#ifndef _LINUX_DEP_H
#define _LINUX_DEP_H


typedef  unsigned int               UINT, *PUINT;
typedef  unsigned char              BYTE, *PBYTE;
typedef  unsigned short             WORD, *PWORD;
typedef  unsigned long              DWORD, *PDWORD;
typedef  unsigned long long         QWORD, *PQWORD;

typedef  long                       LONG;

typedef  int                        BOOL;
#define  FALSE                      0
#define  TRUE                       1

typedef  char                      *LPSTR, *PSTR;
typedef  const char                *LPCSTR, *PCSTR;

typedef  void                       *HANDLE;

typedef  int                        SOCKET;


#define  MAX_PATH                   256

#define  INVALID_SOCKET             (SOCKET)(~0)
#define  SOCKET_ERROR               (-1)

#define  ERROR_SUCCESS              0L
#define  ERROR_ACCESS_DENIED        5L
#define  ERROR_INVALID_HANDLE       6L
#define  ERROR_NOT_ENOUGH_MEMORY    8L
#define  ERROR_INVALID_DATA         13L
#define  ERROR_OUTOFMEMORY          14L
#define  ERROR_NOT_READY            21L
#define  ERROR_NOT_SUPPORTED        50L
#define  ERROR_INVALID_PARAMETER    87L
#define  ERROR_ALREADY_EXISTS       183L
#define  ERROR_OPERATION_ABORTED    995L
#define  ERROR_NOT_FOUND            1168L
#define  ERROR_CANCELLED            1223L
#define  ERROR_INVALID_INDEX        1413L
#define  ERROR_TIMEOUT              1460L
#define  ERROR_NOT_CONNECTED        2250L
#define  ERROR_INVALID_STATE        5023L

#define  WAIT_TIMEOUT               258L
#define  WAIT_FAILED                0xffffffffL

#define  FULL_FUNC_NAME             __PRETTY_FUNCTION__

#define  GetLastError()             errno
#define  WSAGetLastError()          errno

#define  _countof(_Array)           (sizeof(_Array)/sizeof(_Array[0]))

#define  Sleep(ms)                  usleep(ms*1000)

#define  _snprintf                  snprintf
#define  _snwprintf                 swprintf

#define  closesocket(s)             close(s)
#define  _access(f, m)              access(f, m)

#define  MAKEWORD(a, b)             ((WORD)(((BYTE)(((DWORD)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD)(b)) & 0xff))) << 8))
#define  MAKELONG(a, b)             ((DWORD)(((WORD)(((DWORD)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD)(b)) & 0xffff))) << 16))
#define  LOWORD(l)                  ((WORD)(((DWORD)(l)) & 0xffff))
#define  HIWORD(l)                  ((WORD)((((DWORD)(l)) >> 16) & 0xffff))
#define  LOBYTE(w)                  ((BYTE)(((DWORD)(w)) & 0xff))
#define  HIBYTE(w)                  ((BYTE)((((DWORD)(w)) >> 8) & 0xff))

int sem_timedwait2(sem_t *sem, DWORD dwMilliseconds);

DWORD GetTickCount();

#endif

