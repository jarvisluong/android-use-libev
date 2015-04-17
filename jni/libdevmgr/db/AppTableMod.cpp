#include <StdAfx.h>
#include "ConfigCenter.h"
#include "SqlWrapper.h"
#include "DeviceCenter.h"
#include "AppTableMod.h"


AppTableMod *AppTableMod::_instance = NULL;

AppTableMod *AppTableMod::Instance()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    if ( !_instance )
    {
        DBG(D_VERB, (L"%s: %ls\n", FULL_FUNC_NAME, L"CreateInstance"));
        _instance = new AppTableMod();
    }
    return _instance;
}

void AppTableMod::Destroy()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    if ( _instance )
    {
        _instance->Release();
        _instance = NULL;
    }
}

AppTableMod::AppTableMod()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));
}

AppTableMod::~AppTableMod()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));
}

DWORD AppTableMod::Load()
{
    const char  *sql = "delete from tablet_apps";

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    SqlWrapper::Instance()->ExecSql(sql);

    DeviceCenter::Instance()->RegisterObserver(this);
    
    return ERROR_SUCCESS;
}

void AppTableMod::Unload()
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    DeviceCenter::Instance()->UnregisterObserver(this);
}

DWORD AppTableMod::UpdateAppList(const char *szDevId)
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

    dwRet = UpdateAppList(pDevice);

    pDevice->Release();

    return dwRet;
}

DWORD AppTableMod::UpdateAppList(SmartDevice *pDevice)
{
    BufferItem  biArgs;
    PARGS_GET_APPLIST  pArgsAL;

    DBG(D_INFO, (L"%s: id(%hs)\n", FULL_FUNC_NAME, pDevice->Id().c_str()));

    biArgs.Alloc(sizeof(ARGS_GET_APPLIST));
    pArgsAL = (PARGS_GET_APPLIST)biArgs.Put(sizeof(ARGS_GET_APPLIST));
    strcpy((char*)pArgsAL->szIconDir, ConfigCenter::_IconDir.c_str());
    pDevice->QueueCmd(0, C_DEVCMD_GET_APPLIST, &biArgs, this);

    return ERROR_SUCCESS;
}

void AppTableMod::OnExecuting(SmartDevice *pDevice, DWORD dwCmdIdx, DEVCMD_CODE CmdCode)
{
    DBG(D_VERB, (L"%s: idx(%d), cmd(%d)\n", FULL_FUNC_NAME, dwCmdIdx, CmdCode));
}

void AppTableMod::OnExecuted(SmartDevice *pDevice, DWORD dwCmdIdx, DEVCMD_CODE CmdCode, BufferItem *pbiResult)
{
    char  sql[500];
    PRET_GET_APPLIST  pAppList;
    PAPP_INFO  pAppInfo;

    DBG(D_VERB, (L"%s: idx(%d), cmd(%d)\n", FULL_FUNC_NAME, dwCmdIdx, CmdCode));

    if ( CmdCode == C_DEVCMD_GET_APPLIST )
    {
        // if put it in UpdateAppList(), thread_A(OnAttached()) and thread_B(DB_MSG) call at the same time
        // when the two cmd finished, the later "insert" operations will fail
        _snprintf(sql, _countof(sql), "delete from tablet_apps where tablet_serial='%s'", pDevice->Id().c_str());
        SqlWrapper::Instance()->ExecSql(sql);

        pAppList = (PRET_GET_APPLIST)pbiResult->data();
        pAppInfo = pAppList->AppInfo;

        for ( DWORD i = 0; i < pAppList->dwCount; i++, pAppInfo++ )
        {
            _snprintf(sql, _countof(sql),
                      "insert into tablet_apps (tablet_serial,app_name,app_apk_name,app_version,app_version_code,app_icon) values ('%s','%s','%s','%s',%u,'%s')",
                      pDevice->Id().c_str(),
                      pAppInfo->szName,
                      pAppInfo->szLongName,
                      pAppInfo->szVersion,
                      (UINT)pAppInfo->dwVersion,
                      pAppInfo->szIconName);
            SqlWrapper::Instance()->ExecSql(sql);
        }

        Notify(C_DBMSG_APPLIST_UPDATED, pDevice->Id().c_str());
    }
}

void AppTableMod::OnFailed(SmartDevice *pDevice, DWORD dwCmdIdx, DEVCMD_CODE CmdCode, DWORD dwResult)
{
    DBG(D_VERB, (L"%s: idx(%d), cmd(%d), result(%d)\n", FULL_FUNC_NAME, dwCmdIdx, CmdCode, dwResult));
}

void AppTableMod::OnCancelled(SmartDevice *pDevice, DWORD dwCmdIdx, DEVCMD_CODE CmdCode)
{
    DBG(D_VERB, (L"%s: idx(%d), cmd(%d)\n", FULL_FUNC_NAME, dwCmdIdx, CmdCode));
}

void AppTableMod::OnAttached(SmartDevice *pDevice)
{
    DBG(D_VERB, (L"%s: id(%hs)\n", FULL_FUNC_NAME, pDevice->Id().c_str()));
}

void AppTableMod::OnDetached(SmartDevice *pDevice)
{
    DBG(D_VERB, (L"%s: id(%hs)\n", FULL_FUNC_NAME, pDevice->Id().c_str()));
}

void AppTableMod::OnStateChanged(SmartDevice *pDevice)
{
    DBG(D_VERB, (L"%s: id(%hs)\n", FULL_FUNC_NAME, pDevice->Id().c_str()));

    if ( pDevice->GetState() == DEV_STATE_MAIN )
    {
//        UpdateAppList(pDevice);
    }
}
