#include <StdAfx.h>
#include "AdbCmdGetDiskInfo.h"


AdbCmdGetDiskInfo::AdbCmdGetDiskInfo(const char *szDevId)
    : _DevId(szDevId)
{
    DBG(D_VERB, (L"%s: id(%hs)\n", FULL_FUNC_NAME, szDevId));
}

AdbCmdGetDiskInfo::~AdbCmdGetDiskInfo()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));
}

DWORD AdbCmdGetDiskInfo::BuildCmdline()
{
    char  buf[MAX_ADB_CMD_LEN];

    _snprintf(buf, _countof(buf), ADB_FMT_DUMPSYS, _DevId.c_str(), "diskstats");
    DBG(D_VERB, (L"%s: cmd(%hs)\n", FULL_FUNC_NAME, buf));

    _Cmdline = buf;

    return ERROR_SUCCESS;
}

void AdbCmdGetDiskInfo::ProcessResult()
{
    BOOL  ret;
    PRET_GET_DISKINFO  pInfo;
    const char  *p;
    UINT  nFree = 0, nTotal = 0;

    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    p = (char*)_biOutput.data();

    _biResult.Alloc(sizeof(RET_GET_DISKINFO));
    pInfo = (PRET_GET_DISKINFO)_biResult.Put(sizeof(RET_GET_DISKINFO));

    ret = MatchStringAnywhere(p, "Data-Free: ", p);
    if ( ret )
    {
        ParseUInt(p, FALSE, nFree, p);

        ret = MatchStringBeginning(p, "K / ", p);
        if ( ret )
        {
            ParseUInt(p, FALSE, nTotal, p);
        }
    }


    pInfo->hdr.dwSize = _biResult.data_size();
    pInfo->hdr.dwResult = ERROR_SUCCESS;
    pInfo->dwFreeKB = nFree;
    pInfo->dwTotalKB = nTotal;
}

