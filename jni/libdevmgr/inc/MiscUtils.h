#ifndef _MISC_UTILS_H
#define _MISC_UTILS_H


#ifdef _LINUX_
#include <libmtp.h>
#endif


#define  D_ERROR                1
#define  D_WARN                 1
#define  D_LOG                  1
#define  D_INFO                 1
#define  D_VERB                 0
#define  D_FUNC                 0
#define  D_PERF                 1

#if defined(_WINCE_)
#define  DBG(cond,printf_exp)   ((void)((cond)?(NKDbgPrintfW printf_exp),1:0))
#endif

#if defined(_ANDROID_)
#define  TAG                    "Native"
void LOGI(const wchar_t *fmt, ...);
#define  DBG(cond,printf_exp)   ((void)((cond)?(LOGI printf_exp),1:0))
#endif

#if defined(_LINUX_)
#define  DBG(cond,printf_exp)   ((void)((cond)?(wprintf printf_exp),fflush(stdout),1:0))
#endif

#if defined(_WIN32_)
#ifndef _USRDLL
#define  DBG(cond,printf_exp)   ((void)((cond)?(wprintf printf_exp),1:0))
#else
void TraceW(const wchar_t *fmt, ...);
#define  DBG(cond,printf_exp)   ((void)((cond)?(TraceW printf_exp),1:0))
#endif
#endif

#if defined(_ANDROID_) || defined(_LINUX_)
#define  SET_PTH_INVALID(pth)   (pth = 0)
#define  IS_PTH_VALID(pth)      (pth)
#else
#define  SET_PTH_INVALID(pth)   (pth.p = NULL)
#define  IS_PTH_VALID(pth)      (pth.p)
#endif

#define  ROUND_1K(n)            ((n + 1023) & (~1023))
#define  ROUND_4K(n)            ((n + 4095) & (~4095))

#define  CreateUniId()          (GetTickCount() + rand())

DWORD Round2Power(DWORD dwVal);

BOOL SkipCRLFs(const LPCSTR szString, LPCSTR& rszPointer, const BOOL fSkipChars);
BOOL MatchStringBeginning(const LPCSTR szString, const LPCSTR szMatch, LPCSTR& rszPointer);
BOOL MatchStringEnd(const LPCSTR szString, const LPCSTR szMatch, LPCSTR& rszPointer);
BOOL MatchStringAnywhere(const LPCSTR szString, const LPCSTR szMatch, LPCSTR& rszPointer);
BOOL ParseDWord(const LPCSTR szData, const BOOL fLeadingZerosAllowed, DWORD& rdw, LPCSTR& rszPointer);
BOOL ParseUInt(const LPCSTR szData, const BOOL fLeadingZerosAllowed, UINT& rnInt, LPCSTR& rszPointer);
BOOL ParseUnquotedString(const LPCSTR szData, const char chDelimiter, const LPSTR szOut, const UINT cbOut, LPCSTR& rszPointer);

#ifdef _LINUX_
DWORD MtpCreateFolder(LIBMTP_mtpdevice_t *pDevice, const char *szFolder, uint32_t &dwFolderId);
DWORD MtpDeleteFile(LIBMTP_mtpdevice_t *pDevice, uint32_t dwFolderId, const char *szFile);
#endif


#endif
