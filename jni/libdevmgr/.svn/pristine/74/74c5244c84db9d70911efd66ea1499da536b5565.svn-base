#include <StdAfx.h>
#include "AdbCmdGetBattInfo.h"


AdbCmdGetBattInfo::AdbCmdGetBattInfo(const char *szDevId)
    : _DevId(szDevId)
{
    DBG(D_VERB, (L"%s: id(%hs)\n", FULL_FUNC_NAME, szDevId));
}

AdbCmdGetBattInfo::~AdbCmdGetBattInfo()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));
}

DWORD AdbCmdGetBattInfo::BuildCmdline()
{
    char  buf[MAX_ADB_CMD_LEN];

    _snprintf(buf, _countof(buf), ADB_FMT_DUMPSYS, _DevId.c_str(), "battery");
    DBG(D_VERB, (L"%s: cmd(%hs)\n", FULL_FUNC_NAME, buf));

    _Cmdline = buf;

    return ERROR_SUCCESS;
}

void AdbCmdGetBattInfo::ProcessResult()
{
    BOOL  ret;
    PRET_GET_BATTINFO  pInfo;
    const char  *p;
    char  buf1[20] = {0}, buf2[20] = {0};
    UINT  nLevel = 0;

    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    p = (char*)_biOutput.data();

    _biResult.Alloc(sizeof(RET_GET_BATTINFO));
    pInfo = (PRET_GET_BATTINFO)_biResult.Put(sizeof(RET_GET_BATTINFO));

    ret = MatchStringAnywhere(p, "  AC powered: ", p);
    if ( ret )
    {
        ParseUnquotedString(p, '\r', buf1, sizeof(buf1), p);
    }

    ret = MatchStringAnywhere(p, "  USB powered: ", p);
    if ( ret )
    {
        ParseUnquotedString(p, '\r', buf2, sizeof(buf2), p);
    }

    ret = MatchStringAnywhere(p, "  level: ", p);
    if ( ret )
    {
        ParseUInt(p, FALSE, nLevel, p);
    }

    pInfo->hdr.dwSize = _biResult.data_size();
    pInfo->hdr.dwResult = ERROR_SUCCESS;
    if ( (!strcmp(buf1, "true")) || (!strcmp(buf2, "true")) )
    {
        pInfo->bCharging = true;
    }
    else
    {
        pInfo->bCharging = false;
    }
    pInfo->bLevel = (BYTE)nLevel;
}

