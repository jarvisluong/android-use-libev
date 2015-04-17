#include <StdAfx.h>
#include <AdbCmdStartService.h>
#include <AdbCmdPushPullFile.h>
#include "AdbCmdGetApkList.h"


AdbCmdGetApkList::AdbCmdGetApkList(const char *szDevId, const char *szIconDir)
    : _DevId(szDevId)
    , _IconDir(szIconDir)
{
    DBG(D_VERB, (L"%s: id(%hs)\n", FULL_FUNC_NAME, szDevId));
}

AdbCmdGetApkList::~AdbCmdGetApkList()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));
}

DWORD AdbCmdGetApkList::Execute()
{
    AdbCmdStartService  cmd(_DevId.c_str(), ADB_SERVICED_NAME, true, "--ei DUMPAPP 1");

    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    cmd.Execute();
    Sleep(1000);
    
    AdbCmd::Execute();

    return ERROR_SUCCESS;
}

DWORD AdbCmdGetApkList::BuildCmdline()
{
    char  buf[MAX_ADB_CMD_LEN];

    _snprintf(buf, _countof(buf), ADB_FMT_CATFILE, _DevId.c_str(), ADB_APKLIST_FILE);
    DBG(D_VERB, (L"%s: cmd(%hs)\n", FULL_FUNC_NAME, buf));

    _Cmdline = buf;

    return ERROR_SUCCESS;
}

void AdbCmdGetApkList::ProcessResult()
{
    int  ret;
    AdbCmdPushPullFile  cmd(_DevId.c_str(), "", "", "", false);
    PRET_GET_APPLIST  pAppList;
    PAPP_INFO  pAppInfo;
    const char  *p;
    char  bufH[MAX_PATH];
    UINT  nCnt;

    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    _biResult.Alloc(sizeof(RET_GET_APPLIST) + (MAX_APP_LIST_NUM - 1) * sizeof(APP_INFO));
    pAppList = (PRET_GET_APPLIST)_biResult.Put(sizeof(RET_GET_APPLIST) - sizeof(APP_INFO));

    pAppList->hdr.dwResult = ERROR_SUCCESS;
    pAppList->dwCount = 0;

    ParseUInt((char*)_biOutput.data(), FALSE, nCnt, p);
    if ( nCnt <= 0 )
    {
        goto out;
    }

    if ( nCnt > (UINT)MAX_APP_LIST_NUM )
    {
        DBG(D_WARN, (L"%s: app count too large: %d\n", FULL_FUNC_NAME, nCnt));
        nCnt = MAX_APP_LIST_NUM;
    }

    pAppInfo = &pAppList->AppInfo[0];

    for (UINT i = 0; i < nCnt; i++, pAppInfo++ )
    {
        ret = MatchStringAnywhere(p, "\r\r\n", p);
        if ( !ret )
        {
            DBG(D_WARN, (L"%s: invalid format\n", FULL_FUNC_NAME));
//            assert(0);
            goto out;
        }

        ParseUnquotedString(p, '\t', (char*)pAppInfo->szName, MAX_APP_NAME_LEN, p);
        p++;
        ParseUnquotedString(p, '\t', (char*)pAppInfo->szLongName, MAX_APP_LONGNAME_LEN, p);
        p++;
        ParseUnquotedString(p, '\t', (char*)pAppInfo->szVersion, MAX_APP_VERSION_LEN, p);
        p++;
        ParseDWord(p, FALSE, pAppInfo->dwVersion, p);

        _snprintf((char*)pAppInfo->szIconName, MAX_APP_ICONNAME_LEN, "%s.png", (char*)pAppInfo->szLongName);

        pAppList->dwCount++;
        _biResult.Put(sizeof(APP_INFO));

        _snprintf(bufH, _countof(bufH), "%s%s", _IconDir.c_str(), (char*)pAppInfo->szIconName);
        if ( !_access(bufH, 0) )
        {
            continue;
        }

        cmd.SetPath(bufH, ADB_APKICON_DIR, (char*)pAppInfo->szIconName);
        cmd.Execute();
    }

out:
    pAppList->hdr.dwSize = _biResult.data_size();

    _snprintf(bufH, _countof(bufH), ADB_FMT_DELETE_FILE, _DevId.c_str(), ADB_APKLIST_FILE);
    ret = system(bufH);
}

