#include <StdAfx.h>
#include "AdbCmdEnumDev.h"


AdbCmdEnumDev::AdbCmdEnumDev()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));
}

AdbCmdEnumDev::~AdbCmdEnumDev()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));
}

DWORD AdbCmdEnumDev::BuildCmdline()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    _Cmdline = ADB_FMT_ENUM_DEVICES;

    return ERROR_SUCCESS;
}

void AdbCmdEnumDev::ProcessResult()
{
    int  i;
    const char  *szOutput, *p;
    PRET_ENUM_DEVICES  pEnumDevices;

    szOutput = (const char*)_biOutput.data();
    DBG(D_INFO, (L"%s: output(%hs)\n", FULL_FUNC_NAME, szOutput));

    _biResult.Alloc(sizeof(RET_ENUM_DEVICES) + MAX_DEV_ID_LEN * (MAX_ADB_DEV_CNT - 1));
    pEnumDevices = (PRET_ENUM_DEVICES)_biResult.Put(sizeof(RET_ENUM_DEVICES) - MAX_DEV_ID_LEN);

    pEnumDevices->hdr.dwResult = ERROR_NOT_SUPPORTED;
    pEnumDevices->dwDevNum = 0;

    if ( !MatchStringBeginning(szOutput, "List of devices attached ", p) )
    {
        goto out;
    }
    SkipCRLFs(p, p, TRUE);

    for ( i = 0; i < MAX_ADB_DEV_CNT; )
    {
        if ( !ParseUnquotedString(p, '\t', (char*)pEnumDevices->szDevId[i], MAX_DEV_ID_LEN, p) )
        {
            break;
        }

        if ( !MatchStringBeginning(p, "\t", p) )
        {
            break;
        }

        if ( MatchStringBeginning(p, "device", p) )
        {
            i++;
        }

        SkipCRLFs(p, p, TRUE);

        _biResult.Put(MAX_DEV_ID_LEN);
    }
    pEnumDevices->dwDevNum = i;

    pEnumDevices->hdr.dwResult = ERROR_SUCCESS;

out:
    pEnumDevices->hdr.dwSize = _biResult.data_size();
}

