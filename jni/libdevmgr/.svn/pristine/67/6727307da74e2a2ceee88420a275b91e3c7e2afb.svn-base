#include <StdAfx.h>
#include <sys/stat.h>
#include "MtpDeviceMon.h"
#include "AdbCmdPushPullFile.h"


AdbCmdPushPullFile::AdbCmdPushPullFile(const char *szDevId, const char *szFilePathH, const char *szPathD, const char *szFileD, bool bPush)
    : _DevId(szDevId)
    , _FilePath_H(szFilePathH)
    , _Path_D(szPathD)
    , _File_D(szFileD)
    , _bPush(bPush)
{
    DBG(D_VERB, (L"%s: id(%hs), host(%hs), dev(%hs%s), bPush(%d)\n", FULL_FUNC_NAME, szDevId, szFilePathH, szPathD, szFileD, bPush));
}

AdbCmdPushPullFile::~AdbCmdPushPullFile()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));
}

DWORD AdbCmdPushPullFile::Execute()
{
    DWORD  dwRet;
    PRET_PUSH_PULL_FILE  pRet;

    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    _biResult.Alloc(sizeof(RET_PUSH_PULL_FILE));
    _biResult.Put(sizeof(RET_PUSH_PULL_FILE));

    if ( _bPush )
    {
        dwRet = MtpSendFile();
        if ( dwRet == ERROR_SUCCESS )
        {
            pRet = (PRET_PUSH_PULL_FILE)_biResult.data();
            pRet->hdr.dwSize = sizeof(RET_PUSH_PULL_FILE);
            pRet->hdr.dwResult = ERROR_SUCCESS;
            return ERROR_SUCCESS;
        }
    }

    DBG(D_INFO, (L"%s: use ADB method...\n", FULL_FUNC_NAME));
    return  AdbCmd::Execute();
}

void AdbCmdPushPullFile::SetPath(const char *szFilePathH, const char *szPathD, const char *szFileD)
{
    DBG(D_VERB, (L"%s: h(%hs), d(%hs%hs)\n", FULL_FUNC_NAME, szFilePathH, szPathD, szFileD));

    _FilePath_H = szFilePathH;
    _Path_D = szPathD;
    _File_D = szFileD;
}

DWORD AdbCmdPushPullFile::BuildCmdline()
{
    char  fname[MAX_PATH];
    char  buf[MAX_ADB_CMD_LEN];

    _snprintf(fname, _countof(fname), "%s%s", _Path_D.c_str(), _File_D.c_str());

    if ( _bPush )
    {
        _snprintf(buf, _countof(buf), ADB_FMT_PUSH_FILE, _DevId.c_str(), _FilePath_H.c_str(), fname);
    }
    else
    {
        _snprintf(buf, _countof(buf), ADB_FMT_PULL_FILE, _DevId.c_str(), fname, _FilePath_H.c_str());
    }
    DBG(D_VERB, (L"%s: cmd(%hs)\n", FULL_FUNC_NAME, buf));

    _Cmdline = buf;

    return ERROR_SUCCESS;
}

void AdbCmdPushPullFile::ProcessResult()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    AdbCmd::ProcessResult();
}

DWORD AdbCmdPushPullFile::MtpSendFile()
{
#ifdef _LINUX_
    DWORD  dwRet;
    int  ret;
    DWORD  dwTicks;
    LIBMTP_mtpdevice_t  *pDevice;
    LIBMTP_file_t  *pMtpFile;
    uint32_t  dwFolderId;
    struct stat si;

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    dwRet = MtpDeviceMon::Instance()->GetMtpDevice(_DevId.c_str(), pDevice);
    if ( dwRet != ERROR_SUCCESS )
    {
        DBG(D_WARN, (L"%s: mtp(%hs) not found\n", FULL_FUNC_NAME, _DevId.c_str()));
        goto err1;
    }

    ret = stat(_FilePath_H.c_str(), &si);
    if ( ret )
    {
        dwRet = GetLastError();
        DBG(D_WARN, (L"%s: stat(%hs) failed: %d\n", FULL_FUNC_NAME, _FilePath_H.c_str(), dwRet));
        goto err2;
    }

    dwRet = MtpCreateFolder(pDevice, _Path_D.c_str(), dwFolderId);
    if ( dwRet != ERROR_SUCCESS )
    {
        DBG(D_WARN, (L"%s: MtpCreateFolder(\"%hs\") on dev(%hs) failed: %d\n", FULL_FUNC_NAME, _Path_D.c_str(), _DevId.c_str(), dwRet));
        goto err2;
    }

    MtpDeleteFile(pDevice, dwFolderId, _File_D.c_str());

    pMtpFile = LIBMTP_new_file_t();
    pMtpFile->parent_id = dwFolderId;
    pMtpFile->storage_id = 0;
    pMtpFile->filename = strdup((char*)_File_D.c_str());
    pMtpFile->filesize = si.st_size;
    pMtpFile->filetype = LIBMTP_FILETYPE_UNKNOWN;

    DBG(D_INFO, (L"%s: start sending(%hs) to dev(%hs)\n", FULL_FUNC_NAME, _FilePath_H.c_str(), _DevId.c_str()));
    dwTicks = GetTickCount();
    ret = LIBMTP_Send_File_From_File(pDevice, _FilePath_H.c_str(), pMtpFile, NULL, NULL);
    dwTicks = (GetTickCount() - dwTicks) / 1000;
    DBG(D_INFO, (L"%s: finish sending(%hs) to dev(%hs): ret(%d) @ %ds\n", FULL_FUNC_NAME, _FilePath_H.c_str(), _DevId.c_str(), ret, dwTicks));
    if ( ret )
    {
        LIBMTP_Dump_Errorstack(pDevice);
        LIBMTP_Clear_Errorstack(pDevice);
        dwRet = ret;
        goto err3;
    } 

    dwRet = ERROR_SUCCESS;

err3:
    LIBMTP_destroy_file_t(pMtpFile);

err2:
    MtpDeviceMon::Instance()->ReleaseMtpDevice(pDevice);

err1:
    return dwRet;
#else
    return ERROR_NOT_SUPPORTED;
#endif
}
