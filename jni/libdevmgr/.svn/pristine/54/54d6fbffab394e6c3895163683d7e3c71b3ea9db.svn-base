#include <StdAfx.h>
#include "AdbCmdPushPullFile.h"
#include "AdbCmdScreenCap.h"


AdbCmdScreenCap::AdbCmdScreenCap(const char *szDevId, const char *szFileName)
    : _DevId(szDevId)
    , _FileName(szFileName)
{
    DBG(D_VERB, (L"%s: id(%hs)\n", FULL_FUNC_NAME, szDevId));
}

AdbCmdScreenCap::~AdbCmdScreenCap()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));
}

DWORD AdbCmdScreenCap::BuildCmdline()
{
    char  buf[MAX_ADB_CMD_LEN];

    _snprintf(buf, _countof(buf), ADB_FMT_SCREENCAP, _DevId.c_str(), ADB_SCREENCAP_FILE);
    DBG(D_VERB, (L"%s: cmd(%hs)\n", FULL_FUNC_NAME, buf));

    _Cmdline = buf;

    return ERROR_SUCCESS;
}

void AdbCmdScreenCap::ProcessResult()
{
    AdbCmdPushPullFile  cmd(_DevId.c_str(), "", "", "", false);
    PRET_SCREEN_CAP  pScrCap;
    char  buf[MAX_PATH];

    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    _biResult.Alloc(sizeof(RET_SCREEN_CAP));
    pScrCap = (PRET_SCREEN_CAP)_biResult.Put(sizeof(RET_SCREEN_CAP));

    pScrCap->hdr.dwSize = _biResult.data_size();
    pScrCap->hdr.dwResult = ERROR_SUCCESS;

    strcpy((char*)pScrCap->szSuffix, "png");

    _snprintf(buf, _countof(buf), "%s.png", _FileName.c_str());
    cmd.SetPath(buf, "", ADB_SCREENCAP_FILE);
    cmd.Execute();
}

