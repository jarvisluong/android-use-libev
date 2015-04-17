#include <StdAfx.h>
#include "AdbCmdStartService.h"


AdbCmdStartService::AdbCmdStartService(const char *szDevId, const char *szSrvName, bool bAdmin, const char *szArgs)
    : _DevId(szDevId)
    , _SrvName(szSrvName)
    , _bAdmin(bAdmin)
    , _Args(szArgs)
{
    DBG(D_VERB, (L"%s: id(%hs)\n", FULL_FUNC_NAME, szDevId));
}

AdbCmdStartService::~AdbCmdStartService()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));
}

DWORD AdbCmdStartService::BuildCmdline()
{
    char  buf[MAX_ADB_CMD_LEN];

    _snprintf(buf, _countof(buf), ADB_FMT_START_SERVICE,
              _DevId.c_str(),
              _Args.c_str(),
              (_bAdmin ? "--user 0" : " "),
              _SrvName.c_str());

    _Cmdline = buf;

    DBG(D_VERB, (L"%s: cmd(%hs)\n", FULL_FUNC_NAME, _Cmdline.c_str()));

    return ERROR_SUCCESS;
}

void AdbCmdStartService::ProcessResult()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    _biResult.Alloc(sizeof(RET_START_SERVICE));
    _biResult.Put(sizeof(RET_START_SERVICE));

    AdbCmd::ProcessResult();
}

