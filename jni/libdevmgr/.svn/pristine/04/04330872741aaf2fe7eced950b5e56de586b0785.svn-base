#include <StdAfx.h>
#include "MiscUtils.h"


#if defined(_ANDROID_)
#include <android/log.h>
void LOGI(const wchar_t *fmt, ...)
{
    va_list  arg;
    wchar_t  wcs[200];
    char  mbs[200];

    va_start(arg, fmt);
    vswprintf(wcs, _countof(wcs), fmt, arg);
    va_end(arg);

    wcstombs(mbs, wcs, _countof(mbs));

    __android_log_print(ANDROID_LOG_INFO, TAG, mbs);
}
#endif

#if defined(_ANDROID_) || defined(_LINUX_)
int sem_timedwait2(sem_t *sem, DWORD dwMilliseconds)
{
    struct timespec  ts;
    struct timeval  now; 

    gettimeofday(&now, NULL); 

    ts.tv_nsec = now.tv_usec * 1000 + (dwMilliseconds % 1000) * 1000000; 
    ts.tv_sec = now.tv_sec + dwMilliseconds / 1000;
 
    return sem_timedwait(sem, &ts);
}
#endif

#ifdef _USRDLL
void TraceW(const wchar_t *fmt, ...)
{
    va_list  argptr;
    wchar_t  buf[1000];

    va_start(argptr, fmt);
    vswprintf_s(buf, _countof(buf), fmt, argptr);
    va_end(argptr);

    OutputDebugString(buf);
}
#endif

DWORD Round2Power(DWORD dwVal)
{
    dwVal--;
    dwVal |= (dwVal >> 1);
    dwVal |= (dwVal >> 2);
    dwVal |= (dwVal >> 4);
    dwVal |= (dwVal >> 8);
    dwVal |= (dwVal >> 16);
    dwVal++;

    return dwVal;
}

#if defined(_ANDROID_) || defined(_LINUX_)
DWORD GetTickCount()
{
    struct timeval  t;

    gettimeofday(&t, NULL);

    return  t.tv_sec * 1000 + t.tv_usec / 1000;
}
#endif

inline void SkipChars(const LPCSTR szData, LPCSTR& rszPointer, LPCSTR szChars)
{
    rszPointer = szData + strspn(szData,szChars);
}

BOOL SkipCRLFs(const LPCSTR szString, LPCSTR& rszPointer, const BOOL fSkipChars)
{
    rszPointer = szString;
    if ( fSkipChars )
    {
        while ( (*rszPointer) && ((*rszPointer) != '\r') && ((*rszPointer) != '\n') )
        {
            rszPointer++;
        }
    }
    while ( ((*rszPointer) == '\r') || ((*rszPointer) == '\n') )
    {
        rszPointer++;
    }
    return (rszPointer != szString);
}

BOOL MatchStringBeginning(const LPCSTR szString, const LPCSTR szMatch, LPCSTR& rszPointer)
{
    BOOL fRet = FALSE;

    LPCSTR szTemp = strstr(szString, szMatch);
    if (!szTemp)
    {
        goto End;
    }

    fRet = (szTemp == szString);
    if (fRet)
    {
        rszPointer = szString + strlen(szMatch);
    }

End:
    return fRet;
}

BOOL MatchStringEnd(const LPCSTR szString, const LPCSTR szMatch, LPCSTR& rszPointer)
{
    UINT cbString = strlen(szString);
    UINT cbMatch = strlen(szMatch);
    BOOL fRet = FALSE;

    if (cbString >= cbMatch)
    {
        fRet = !strcmp(szString + cbString - cbMatch, szMatch);
    }
    if (fRet)
    {
        rszPointer = szString + cbString;
    }

    return fRet;
}

BOOL MatchStringAnywhere(const LPCSTR szString, const LPCSTR szMatch, LPCSTR& rszPointer)
{
    BOOL fRet = FALSE;

    LPCSTR szTemp = strstr(szString, szMatch);
    if (!szTemp)
    {
        goto End;
    }

    rszPointer = szTemp + strlen(szMatch);
    fRet = TRUE;

End:
    return fRet;
}

BOOL ParseDWord(const LPCSTR szData, const BOOL fLeadingZerosAllowed, DWORD& rdw, LPCSTR& rszPointer)
{
    int nLeadingDigit = -1;
    BOOL Found=FALSE;

    // Skip any spaces
    SkipChars(szData, rszPointer, " ");

    rdw = 0;

    while (*rszPointer >= '0' && *rszPointer <= '9')
    {
        if (!fLeadingZerosAllowed)
        {
            // Test for leading 0s
            if (-1 == nLeadingDigit)
            {
                // Leading digit hasn't been set yet -- set it now
                nLeadingDigit = *rszPointer - '0';
            }
            else if (!nLeadingDigit)
            {
                // Leading digit is 0 and we got another digit
                // This means that we have leading 0s -- reset the pointer and punt
                Found=FALSE;
                goto Error;
            }
        }

        rdw = rdw * 10 + (*rszPointer - '0');
        rszPointer++;
        Found=TRUE;
    }

    // Skip past any spaces
    SkipChars(rszPointer, rszPointer, " ");

Error:
    if (!Found)
    {
        rszPointer = szData;
    }
    return Found;
}

BOOL ParseUInt(const LPCSTR szData, const BOOL fLeadingZerosAllowed, UINT& rnInt, LPCSTR& rszPointer)
{
    return ParseDWord(szData, fLeadingZerosAllowed, (DWORD&)rnInt, rszPointer);
}

BOOL ParseUnquotedString(const LPCSTR szData, const char chDelimiter, const LPSTR szOut, const UINT cbOut, LPCSTR& rszPointer)
{
    LPSTR pchOut;
    LPSTR szOutEnd;
    BOOL fRet = FALSE;

    rszPointer = szData;
    pchOut = szOut;
    szOutEnd = szOut + cbOut;

    // Copy the string data until we encounter the delimiter character
    while ((*rszPointer) && (chDelimiter != *rszPointer) && (pchOut < szOutEnd - 1))
    {
        *pchOut++ = *rszPointer++;
    }
    *pchOut = '\0';

    // Skip forward until the delimiter character, if we haven't reached it yet
    if (chDelimiter != *rszPointer)
    {
        rszPointer = strchr(rszPointer, chDelimiter);
        if (!rszPointer)
        {
            // We didn't find the delimiter in the string -- fail
            goto Error;
        }
    }
    fRet = TRUE;

Error:
    if (!fRet)
    {
        rszPointer = szData;
    }
    return fRet;
}

#ifdef _LINUX_
DWORD MtpCreateFolder(LIBMTP_mtpdevice_t *pDevice, const char *szFolder, uint32_t &dwFolderId)
{
    DWORD  dwTicks;
    LIBMTP_folder_t  *pMtpFolder, *pCurFolder;

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    szFolder = "CartInfo";
    dwFolderId = 0;

    pMtpFolder = LIBMTP_Get_Folder_List(pDevice);
    pCurFolder = pMtpFolder;
    while ( pCurFolder )
    {
        if ( !strcmp(szFolder, pCurFolder->name) )
        {
            dwFolderId = pCurFolder->folder_id;
            break;
        }
        pCurFolder = pCurFolder->sibling;
    }

    DBG(D_INFO, (L"%s: folder? %p\n", FULL_FUNC_NAME, pCurFolder));
    if ( !pCurFolder )
    {
        DBG(D_INFO, (L"%s: start creating folder(%hs)\n", FULL_FUNC_NAME, szFolder));
        dwTicks = GetTickCount();
        dwFolderId = LIBMTP_Create_Folder(pDevice, (char*)szFolder, 0, 0);
        dwTicks = GetTickCount() - dwTicks;
        DBG(D_INFO, (L"%s: finish creating folder(%hs): FolderId(%d) @ %dms\n", FULL_FUNC_NAME, szFolder, dwFolderId, dwTicks));
        if ( !dwFolderId )
        {
            LIBMTP_Dump_Errorstack(pDevice);
            LIBMTP_Clear_Errorstack(pDevice);
        } 
    }

    LIBMTP_destroy_folder_t(pMtpFolder);

    return (dwFolderId ? ERROR_SUCCESS: ERROR_NOT_SUPPORTED);
}

DWORD MtpDeleteFile(LIBMTP_mtpdevice_t *pDevice, uint32_t dwFolderId, const char *szFile)
{
    int  ret;
    DWORD  dwTicks;
    LIBMTP_file_t  *pMtpFile, *pCurFile;

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    pMtpFile = LIBMTP_Get_Filelisting(pDevice);
    for ( pCurFile = pMtpFile; pCurFile; pCurFile = pCurFile->next )
    {
        if ( (dwFolderId == pCurFile->parent_id) && (!strcmp(szFile, pCurFile->filename)) )
        {
            break;
        }
    }

    DBG(D_INFO, (L"%s: file? %p\n", FULL_FUNC_NAME, pCurFile));
    if ( pCurFile )
    {
        DBG(D_INFO, (L"%s: start deleting file(%hs)\n", FULL_FUNC_NAME, szFile));
        dwTicks = GetTickCount();
        ret = LIBMTP_Delete_Object(pDevice, pCurFile->item_id);
        dwTicks = GetTickCount() - dwTicks;
        DBG(D_INFO, (L"%s: finish deleting file(%hs): ret(%d) @ %dms\n", FULL_FUNC_NAME, szFile, ret, dwTicks));
        if ( ret )
        {
            LIBMTP_Dump_Errorstack(pDevice);
            LIBMTP_Clear_Errorstack(pDevice);
        } 
    }

    while ( pMtpFile )
    {
        pCurFile = pMtpFile;
        pMtpFile = pMtpFile->next;

        LIBMTP_destroy_file_t(pCurFile);
    }

    return ERROR_SUCCESS;
}
#endif
