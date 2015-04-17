#include <StdAfx.h>
#include "SqlWrapper.h"
#include "DeviceCenter.h"
#include "DevTableMod.h"


DevTableMod *DevTableMod::_instance = NULL;

DevTableMod *DevTableMod::Instance()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    if ( !_instance )
    {
        DBG(D_VERB, (L"%s: %ls\n", FULL_FUNC_NAME, L"CreateInstance"));
        _instance = new DevTableMod();
    }
    return _instance;
}

void DevTableMod::Destroy()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    if ( _instance )
    {
        _instance->Release();
        _instance = NULL;
    }
}

DevTableMod::DevTableMod()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));
}

DevTableMod::~DevTableMod()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));
}

DWORD DevTableMod::Load()
{
    const char  *sql = "delete from tablets";

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    SqlWrapper::Instance()->ExecSql(sql);

    DeviceCenter::Instance()->RegisterObserver(this);
    
    return ERROR_SUCCESS;
}

void DevTableMod::Unload()
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    DeviceCenter::Instance()->UnregisterObserver(this);
}

DWORD DevTableMod::UpdateSysInfo(const char *szDevId)
{
    DWORD  dwRet;
    SmartDevice  *pDevice;

    DBG(D_INFO, (L"%s: id(%hs)\n", FULL_FUNC_NAME, szDevId));

    dwRet = DeviceCenter::Instance()->GetDevice(szDevId, pDevice);
    if ( dwRet != ERROR_SUCCESS )
    {
        DBG(D_INFO, (L"%s: dev(%hs) not exist\n", FULL_FUNC_NAME, szDevId));
        return dwRet;
    }

    dwRet = UpdateSysInfo(pDevice);

    pDevice->Release();

    return dwRet;
}

DWORD DevTableMod::UpdateSysInfo(SmartDevice *pDevice)
{
    BufferItem  biArgs;
    PARGS_GET_SYSINFO  pArgs;

    DBG(D_INFO, (L"%s: id(%hs)\n", FULL_FUNC_NAME, pDevice->Id().c_str()));

    biArgs.Alloc(sizeof(ARGS_GET_SYSINFO));
    pArgs = (PARGS_GET_SYSINFO)biArgs.Put(sizeof(ARGS_GET_SYSINFO));
    pArgs->dwMask = 0xFFFFFFFF;
    pDevice->QueueCmd(0, C_DEVCMD_GET_SYSINFO, &biArgs, this);

    return ERROR_SUCCESS;
}

DWORD DevTableMod::UpdateBattInfo(const char *szDevId)
{
    DWORD  dwRet;
    SmartDevice  *pDevice;

    DBG(D_INFO, (L"%s: id(%hs)\n", FULL_FUNC_NAME, szDevId));

    dwRet = DeviceCenter::Instance()->GetDevice(szDevId, pDevice);
    if ( dwRet != ERROR_SUCCESS )
    {
        DBG(D_INFO, (L"%s: dev(%hs) not exist\n", FULL_FUNC_NAME, szDevId));
        return dwRet;
    }

    dwRet = UpdateBattInfo(pDevice);

    pDevice->Release();

    return dwRet;
}

DWORD DevTableMod::UpdateBattInfo(SmartDevice *pDevice)
{
    BufferItem  biArgs;
//    PARGS_GET_BATTINFO  pArgs;

    DBG(D_INFO, (L"%s: id(%hs)\n", FULL_FUNC_NAME, pDevice->Id().c_str()));

    biArgs.Alloc(sizeof(ARGS_GET_BATTINFO));
//    pArgs = (PARGS_GET_BATTINFO)biArgs.Put(sizeof(ARGS_GET_BATTINFO));
    pDevice->QueueCmd(0, C_DEVCMD_GET_BATTINFO, &biArgs, this);

    return ERROR_SUCCESS;
}

DWORD DevTableMod::UpdateDiskInfo(const char *szDevId)
{
    DWORD  dwRet;
    SmartDevice  *pDevice;

    DBG(D_INFO, (L"%s: id(%hs)\n", FULL_FUNC_NAME, szDevId));

    dwRet = DeviceCenter::Instance()->GetDevice(szDevId, pDevice);
    if ( dwRet != ERROR_SUCCESS )
    {
        DBG(D_INFO, (L"%s: dev(%hs) not exist\n", FULL_FUNC_NAME, szDevId));
        return dwRet;
    }

    dwRet = UpdateDiskInfo(pDevice);

    pDevice->Release();

    return dwRet;
}

DWORD DevTableMod::UpdateDiskInfo(SmartDevice *pDevice)
{
    BufferItem  biArgs;
//    PARGS_GET_DISKINFO  pArgs;

    DBG(D_INFO, (L"%s: id(%hs)\n", FULL_FUNC_NAME, pDevice->Id().c_str()));

    biArgs.Alloc(sizeof(ARGS_GET_DISKINFO));
//    pArgs = (PARGS_GET_DISKINFO)biArgs.Put(sizeof(ARGS_GET_DISKINFO));
    pDevice->QueueCmd(0, C_DEVCMD_GET_DISKINFO, &biArgs, this);

    return ERROR_SUCCESS;
}

DWORD DevTableMod::FindMe(const char *szDevId)
{
    DWORD  dwRet;
    SmartDevice  *pDevice;

    DBG(D_INFO, (L"%s: id(%hs)\n", FULL_FUNC_NAME, szDevId));

    dwRet = DeviceCenter::Instance()->GetDevice(szDevId, pDevice);
    if ( dwRet != ERROR_SUCCESS )
    {
        DBG(D_INFO, (L"%s: dev(%hs) not exist\n", FULL_FUNC_NAME, szDevId));
        return dwRet;
    }

    dwRet = FindMe(pDevice);

    pDevice->Release();

    return dwRet;
}

DWORD DevTableMod::FindMe(SmartDevice *pDevice)
{
    BufferItem  biArgs;
//    PARGS_FIND_ME  pArgs;

    DBG(D_INFO, (L"%s: id(%hs)\n", FULL_FUNC_NAME, pDevice->Id().c_str()));

    biArgs.Alloc(sizeof(ARGS_FIND_ME));
//    pArgs = (PARGS_FIND_ME)biArgs.Put(sizeof(ARGS_FIND_ME));
    pDevice->QueueCmd(0, C_DEVCMD_FIND_ME, &biArgs, this);

    return ERROR_SUCCESS;
}

void DevTableMod::OnExecuting(SmartDevice *pDevice, DWORD dwCmdIdx, DEVCMD_CODE CmdCode)
{
    DBG(D_VERB, (L"%s: idx(%d), cmd(%d)\n", FULL_FUNC_NAME, dwCmdIdx, CmdCode));
}

void DevTableMod::OnExecuted(SmartDevice *pDevice, DWORD dwCmdIdx, DEVCMD_CODE CmdCode, BufferItem *pbiResult)
{
    char  sql[300];
    DBMSG_CODE  MsgCode;

    DBG(D_VERB, (L"%s: idx(%d), cmd(%d)\n", FULL_FUNC_NAME, dwCmdIdx, CmdCode));

    switch ( CmdCode )
    {
        case C_DEVCMD_GET_SYSINFO:
        {
            PRET_GET_SYSINFO  pSysInfo;
            MsgCode = C_DBMSG_SYSINFO_UPDATED;
            assert(pbiResult->data_size() >= sizeof(RET_GET_SYSINFO));
            pSysInfo = (PRET_GET_SYSINFO)pbiResult->data();
            _snprintf(sql, _countof(sql),
                      "update tablets set model='%s',brand='%s',osver='%s' where serial='%s'",
                      pSysInfo->szModel, pSysInfo->szBrand, pSysInfo->szOsVer, pDevice->Id().c_str());
            break;
        }

        case C_DEVCMD_GET_BATTINFO:
        {
            PRET_GET_BATTINFO  pBattInfo;
            MsgCode = C_DBMSG_BATTINFO_UPDATED;
            assert(pbiResult->data_size() >= sizeof(RET_GET_BATTINFO));
            pBattInfo = (PRET_GET_BATTINFO)pbiResult->data();
            _snprintf(sql, _countof(sql),
                      "update tablets set battery=%d where serial='%s'",
                      pBattInfo->bLevel, pDevice->Id().c_str());
            break;
        }

        case C_DEVCMD_GET_DISKINFO:
        {
            PRET_GET_DISKINFO  pDiskInfo;
            MsgCode = C_DBMSG_DISKINFO_UPDATED;
            assert(pbiResult->data_size() >= sizeof(RET_GET_DISKINFO));
            pDiskInfo = (PRET_GET_DISKINFO)pbiResult->data();
            _snprintf(sql, _countof(sql),
                      "update tablets set space_used=%u,space_all=%u where serial='%s'",
                      (UINT)(pDiskInfo->dwTotalKB - pDiskInfo->dwFreeKB), (UINT)pDiskInfo->dwTotalKB, pDevice->Id().c_str());
            break;
        }

        default:
            sql[0] = 0;
            break;
    }

    if ( sql[0] )
    {
        SqlWrapper::Instance()->ExecSql(sql);

        Notify(MsgCode, pDevice->Id().c_str());
    }
}

void DevTableMod::OnFailed(SmartDevice *pDevice, DWORD dwCmdIdx, DEVCMD_CODE CmdCode, DWORD dwResult)
{
    DBG(D_VERB, (L"%s: idx(%d), cmd(%d), result(%d)\n", FULL_FUNC_NAME, dwCmdIdx, CmdCode, dwResult));
}

void DevTableMod::OnCancelled(SmartDevice *pDevice, DWORD dwCmdIdx, DEVCMD_CODE CmdCode)
{
    DBG(D_VERB, (L"%s: idx(%d), cmd(%d)\n", FULL_FUNC_NAME, dwCmdIdx, CmdCode));
}

void DevTableMod::OnAttached(SmartDevice *pDevice)
{
    char  sql[200];

    DBG(D_VERB, (L"%s: id(%hs)\n", FULL_FUNC_NAME, pDevice->Id().c_str()));

#ifdef USE_PGSQL
    _snprintf(sql, _countof(sql), "delete from tablets where serial='%s';insert into tablets (serial,connected) values ('%s',1)", pDevice->Id().c_str(), pDevice->Id().c_str());
#else
    _snprintf(sql, _countof(sql), "replace into tablets (serial,connected) values ('%s',1)", pDevice->Id().c_str());
#endif
    SqlWrapper::Instance()->ExecSql(sql);

    Notify(C_DBMSG_DEVTAB_UPDATED, pDevice->Id().c_str());
}

void DevTableMod::OnDetached(SmartDevice *pDevice)
{
    char  sql[200];

    DBG(D_VERB, (L"%s: id(%hs)\n", FULL_FUNC_NAME, pDevice->Id().c_str()));

//    _snprintf(sql, _countof(sql), "delete from tablets where serial='%s'", pDevice->Id().c_str());
    _snprintf(sql, _countof(sql), "update tablets set connected=0 where serial='%s'", pDevice->Id().c_str());
    SqlWrapper::Instance()->ExecSql(sql);

    Notify(C_DBMSG_DEVTAB_UPDATED, pDevice->Id().c_str());
}

void DevTableMod::OnStateChanged(SmartDevice *pDevice)
{
    char  sql[200];

    DBG(D_VERB, (L"%s: id(%hs)\n", FULL_FUNC_NAME, pDevice->Id().c_str()));

    _snprintf(sql, _countof(sql), "update tablets set connected=%d where serial='%s'", pDevice->GetState(), pDevice->Id().c_str());
    SqlWrapper::Instance()->ExecSql(sql);

    Notify(C_DBMSG_DEVTAB_UPDATED, pDevice->Id().c_str());

    if ( pDevice->GetState() == DEV_STATE_MAIN )
    {
        UpdateSysInfo(pDevice);
        UpdateBattInfo(pDevice);
        UpdateDiskInfo(pDevice);
    }
}
