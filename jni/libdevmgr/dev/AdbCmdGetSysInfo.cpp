#include <StdAfx.h>
#include "AdbCmdGetProp.h"
#include "AdbCmdGetSysInfo.h"


AdbCmdGetSysInfo::AdbCmdGetSysInfo(const char *szDevId, DWORD dwMask)
    : _DevId(szDevId)
    , _dwMask(dwMask)
{
    DBG(D_VERB, (L"%s: id(%hs)\n", FULL_FUNC_NAME, szDevId));
}

AdbCmdGetSysInfo::~AdbCmdGetSysInfo()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));
}

DWORD AdbCmdGetSysInfo::Execute()
{
    AdbCmdGetProp  cmd(_DevId.c_str());
    PRET_GET_SYSINFO  pInfo;
    DWORD  dwBytes;

    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    _biResult.Alloc(sizeof(RET_GET_SYSINFO));
    pInfo = (PRET_GET_SYSINFO)_biResult.Put(sizeof(RET_GET_SYSINFO));

    pInfo->hdr.dwSize = _biResult.data_size();
    pInfo->hdr.dwResult = ERROR_SUCCESS;
    pInfo->dwMask = _dwMask;

    if ( _dwMask | SYSINFO_MASK_OSNAME )
    {
        strcpy((char*)pInfo->szOsName, "Android");
    }

    if ( _dwMask | SYSINFO_MASK_OSVER )
    {
        cmd.SetPropName("ro.build.version.release");
        cmd.Execute();
        dwBytes = MAX_SYSINFO_ITEM_LEN;
        cmd.GetResult(pInfo->szOsVer, dwBytes);
    }

    if ( _dwMask | SYSINFO_MASK_BRAND )
    {
        cmd.SetPropName("ro.product.brand");
        cmd.Execute();
        dwBytes = MAX_SYSINFO_ITEM_LEN;
        cmd.GetResult(pInfo->szBrand, dwBytes);
    }

    if ( _dwMask | SYSINFO_MASK_MODEL )
    {
        cmd.SetPropName("ro.product.model");
        cmd.Execute();
        dwBytes = MAX_SYSINFO_ITEM_LEN;
        cmd.GetResult(pInfo->szModel, dwBytes);
    }

    return ERROR_SUCCESS;
}

DWORD AdbCmdGetSysInfo::BuildCmdline()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    return ERROR_SUCCESS;
}

void AdbCmdGetSysInfo::ProcessResult()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));
}

