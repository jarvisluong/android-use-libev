#include <StdAfx.h>
#include "ConfigCenter.h"
#include "SqlWrapper.h"
#include "DeviceCenter.h"
#include "CmdTableMod.h"


CmdTableMod *CmdTableMod::_instance = NULL;

CmdTableMod *CmdTableMod::Instance()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    if ( !_instance )
    {
        DBG(D_VERB, (L"%s: %ls\n", FULL_FUNC_NAME, L"CreateInstance"));
        _instance = new CmdTableMod();
    }
    return _instance;
}

void CmdTableMod::Destroy()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    if ( _instance )
    {
        _instance->Release();
        _instance = NULL;
    }
}

CmdTableMod::CmdTableMod()
    : _bQuit(false)
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    SET_PTH_INVALID(_hMonThread);
    sem_init(&_hWakeEvent, 0, 1);
}

CmdTableMod::~CmdTableMod()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    sem_destroy(&_hWakeEvent);
    assert(!IS_PTH_VALID(_hMonThread));
}

DWORD CmdTableMod::Load()
{
    DWORD  dwRet;
    int  ret;
    const char  *sql = "truncate op_requests";

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    SqlWrapper::Instance()->ExecSql(sql);

    _bQuit = false;
    ret = pthread_create(&_hMonThread, NULL, MonThreadProc, this);
    if ( ret )
    {
        dwRet = GetLastError();
        DBG(D_ERROR, (L"%s: create thread failed: %d\n", FULL_FUNC_NAME, dwRet));
        SET_PTH_INVALID(_hMonThread);
        goto err1;
    }

    DeviceCenter::Instance()->RegisterObserver(this);

    return ERROR_SUCCESS;

err1:
    return dwRet;
}

void CmdTableMod::Unload()
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    DeviceCenter::Instance()->UnregisterObserver(this);

    if ( IS_PTH_VALID(_hMonThread) )
    {
        _bQuit = true;
        sem_post(&_hWakeEvent);
        pthread_join(_hMonThread, NULL);
        SET_PTH_INVALID(_hMonThread);
    }
}

void CmdTableMod::OnCmdUpdated()
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    sem_post(&_hWakeEvent);
}

void CmdTableMod::OnExecuting(SmartDevice *pDevice, DWORD dwCmdIdx, DEVCMD_CODE CmdCode)
{
    DWORD  dwRet;
    char  sql[200];

    DBG(D_VERB, (L"%s: idx(%d), cmd(%d)\n", FULL_FUNC_NAME, dwCmdIdx, CmdCode));

    _snprintf(sql, _countof(sql), "update op_requests set is_processing=2 where id=%u", (UINT)dwCmdIdx);
    dwRet = SqlWrapper::Instance()->ExecSql(sql);
    if ( dwRet != ERROR_SUCCESS )
    {
        DBG(D_WARN, (L"%s: ExecSql(%hs) failed: %d\n", FULL_FUNC_NAME, sql, dwRet));
    }

    Notify(C_DBMSG_CMDTAB_UPDATED, "");
}

void CmdTableMod::OnExecuted(SmartDevice *pDevice, DWORD dwCmdIdx, DEVCMD_CODE CmdCode, BufferItem *pbiResult)
{
    PRET_COMM_HDR  pHdr;

    DBG(D_VERB, (L"%s: idx(%d), cmd(%d)\n", FULL_FUNC_NAME, dwCmdIdx, CmdCode));

    assert(pbiResult->data_size() >= sizeof(RET_COMM_HDR));
    pHdr = (PRET_COMM_HDR)pbiResult->data();

    UpdateResult(dwCmdIdx, pHdr->dwResult);
}

void CmdTableMod::OnFailed(SmartDevice *pDevice, DWORD dwCmdIdx, DEVCMD_CODE CmdCode, DWORD dwResult)
{
    DBG(D_VERB, (L"%s: idx(%d), cmd(%d), result(%d)\n", FULL_FUNC_NAME, dwCmdIdx, CmdCode, dwResult));

    UpdateResult(dwCmdIdx, dwResult);
}

void CmdTableMod::OnCancelled(SmartDevice *pDevice, DWORD dwCmdIdx, DEVCMD_CODE CmdCode)
{
    DBG(D_VERB, (L"%s: idx(%d), cmd(%d)\n", FULL_FUNC_NAME, dwCmdIdx, CmdCode));

    UpdateResult(dwCmdIdx, ERROR_CANCELLED);
}

void CmdTableMod::OnAttached(SmartDevice *pDevice)
{
    DBG(D_VERB, (L"%s: enter\n", FULL_FUNC_NAME));

    sem_post(&_hWakeEvent);
}

void CmdTableMod::OnDetached(SmartDevice *pDevice)
{
    DBG(D_VERB, (L"%s: enter\n", FULL_FUNC_NAME));
}

void CmdTableMod::OnStateChanged(SmartDevice *pDevice)
{
    DBG(D_VERB, (L"%s: id(%hs)\n", FULL_FUNC_NAME, pDevice->Id().c_str()));
}

void CmdTableMod::UpdateResult(DWORD dwCmdIdx, DWORD dwResult)
{
    DWORD  dwRet;
    char  sql[200];

    DBG(D_VERB, (L"%s: idx(%d), result(%d)\n", FULL_FUNC_NAME, dwCmdIdx, dwResult));

    _snprintf(sql, _countof(sql), "update op_requests set is_done=1,result=%u where id=%u", (UINT)dwResult, (UINT)dwCmdIdx);
    dwRet = SqlWrapper::Instance()->ExecSql(sql);
    if ( dwRet != ERROR_SUCCESS )
    {
        DBG(D_WARN, (L"%s: ExecSql(%hs) failed: %d\n", FULL_FUNC_NAME, sql, dwRet));
    }

    Notify(C_DBMSG_CMDTAB_UPDATED, "");
}

void CmdTableMod::ProbeCmds()
{
    DWORD  dwRet;
    void  *sqlResult;
    char  **sqlRow;
    const char  *sql = "select * from op_requests where (is_done=0 and is_processing=0)";// or (cmd_id=4 and result=21)";

    DBG(D_VERB, (L"%s: enter\n", FULL_FUNC_NAME));

    dwRet = SqlWrapper::Instance()->BeginGetTable(sql, &sqlResult);
    if ( dwRet != ERROR_SUCCESS )
    {
        DBG(D_WARN, (L"%s: ExecSql(%hs) failed: %d\n", FULL_FUNC_NAME, sql, dwRet));
        return;
    }

    while ( true )
    {
        sqlRow = SqlWrapper::Instance()->FetchRow(sqlResult);
        if ( !sqlRow )
        {
            break;
        }

        ProcessCmd(sqlRow);
    }

    SqlWrapper::Instance()->EndGetTable(sqlResult);
}

DWORD CmdTableMod::ProcessCmd(char **row)
{
    DWORD  dwRet;
    DWORD  dwIdx;
    DEVCMD_CODE  CmdCode;
    BufferItem  biArgs;
    SmartDevice  *pDevice;
    char  sql[200];

    DBG(D_INFO, (L"%s:\n  idx(%hs)\n  cmd(%hs)\n  id(%hs)\n  args(%hs)\n",
                FULL_FUNC_NAME, row[0], row[1], row[2], row[3]));

    dwIdx = atoi(row[0]);

    dwRet = DeviceCenter::Instance()->GetDevice(row[2], pDevice);
    if ( dwRet != ERROR_SUCCESS )
    {
        DBG(D_INFO, (L"%s: dev(%hs) not exist\n", FULL_FUNC_NAME, row[2]));
        goto out;
    }

    CmdCode = (DEVCMD_CODE)atoi(row[1]);

    dwRet = ConvertArgs(CmdCode, row[3], biArgs);
    if ( dwRet != ERROR_SUCCESS )
    {
        DBG(D_INFO, (L"%s: args(%hs) not valid\n", FULL_FUNC_NAME, row[3]));
        goto out;
    }

    dwRet = pDevice->QueueCmd(dwIdx, CmdCode, &biArgs, this);
    if ( dwRet != ERROR_SUCCESS )
    {
        DBG(D_INFO, (L"%s: args(%hs) not valid\n", FULL_FUNC_NAME, row[3]));
        goto out;
    }

out:
    if ( dwRet == ERROR_SUCCESS )
    {
        // set is_processing=2 may occur before set is_processing=1
        _snprintf(sql, _countof(sql), "update op_requests set is_processing=1 where id=%u and is_processing=0", (UINT)dwIdx);
    }
    else
    {
        _snprintf(sql, _countof(sql), "update op_requests set is_processing=1,is_done=1,result=%u where id=%u", (UINT)dwRet, (UINT)dwIdx);
    }

    dwRet = SqlWrapper::Instance()->ExecSql(sql);
    if ( dwRet != ERROR_SUCCESS )
    {
        DBG(D_WARN, (L"%s: ExecSql(%hs) failed: %d\n", FULL_FUNC_NAME, sql, dwRet));
    }

    Notify(C_DBMSG_CMDTAB_UPDATED, "");

    if ( pDevice )
    {
        pDevice->Release();
    }

    return dwRet;
}

DWORD CmdTableMod::ConvertArgs(DEVCMD_CODE CmdCode, const char *szArgs, BufferItem &biArgs)
{
    DWORD  dwRet = ERROR_SUCCESS;
    DWORD  dwLen, dwSize;

    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    switch ( CmdCode )
    {
        case C_DEVCMD_INSTALL_APP:
        case C_DEVCMD_INSTALL_INFOVIEW:
        {
            PARGS_INSTALL_APP  pInsApp;
            dwLen = ConfigCenter::_AppDir.size() + strlen(szArgs) + 1;
            dwSize = sizeof(ARGS_INSTALL_APP) - 1 + dwLen;
            biArgs.Alloc(dwSize);
            pInsApp = (PARGS_INSTALL_APP)biArgs.Put(dwSize);
            pInsApp->dwPathSize = dwLen;
            _snprintf((char*)pInsApp->szAppPath, dwLen, "%s%s", ConfigCenter::_AppDir.c_str(), szArgs);
            break;
        }

        case C_DEVCMD_UNINSTALL_APP:
        {
            PARGS_UNINSTALL_APP  pUninsApp;
            dwLen = strlen(szArgs) + 1;
            dwSize = sizeof(ARGS_UNINSTALL_APP) - 1 + dwLen;
            biArgs.Alloc(dwSize);
            pUninsApp = (PARGS_UNINSTALL_APP)biArgs.Put(dwSize);
            pUninsApp->dwNameSize = dwLen;
            strcpy((char*)pUninsApp->szAppName, szArgs);
            break;
        }

        case C_DEVCMD_PUSH_FILE:
        case C_DEVCMD_PULL_FILE:
        {
            char  *p;
            PARGS_PUSH_PULL_FILE  pPPFile;
            biArgs.Alloc(sizeof(ARGS_PUSH_PULL_FILE));
            pPPFile = (PARGS_PUSH_PULL_FILE)biArgs.Put(sizeof(ARGS_PUSH_PULL_FILE));
            p = strchr((char*)szArgs, ',');
            if ( p )
            {
                *p++ = 0;
            }
            else
            {
                p = (char*)szArgs;
            }
            _snprintf((char*)pPPFile->szPathH, MAX_PATH, "%s%s", ConfigCenter::_HostFileDir.c_str(), szArgs);
            strncpy((char*)pPPFile->szPathD, ConfigCenter::_DevFileDir.c_str(), MAX_PATH);
            strncpy((char*)pPPFile->szFileD, p, MAX_PATH);
            break;
        }

        case C_DEVCMD_LABEL_SHOW:
        {
            PARGS_SHOW_LABEL  pShowLabel;
            biArgs.Alloc(sizeof(ARGS_SHOW_LABEL));
            pShowLabel = (PARGS_SHOW_LABEL)biArgs.Put(sizeof(ARGS_SHOW_LABEL));
            pShowLabel->bFastCharging = (szArgs[0] == 'F');
            pShowLabel->dwNum = atoi(&szArgs[1]);
            break;
        }

        case C_DEVCMD_DEL_FILE:
        {
            PARGS_DEL_FILE  pDelFile;
            biArgs.Alloc(sizeof(ARGS_DEL_FILE));
            pDelFile = (PARGS_DEL_FILE)biArgs.Put(sizeof(ARGS_DEL_FILE));
            _snprintf((char*)pDelFile->szFileName, MAX_PATH, "%s%s", ConfigCenter::_DevFileDir.c_str(), szArgs);
            break;
        }

        default:
            dwRet = ERROR_NOT_SUPPORTED;
            break;
    }

    return dwRet;
}

DWORD CmdTableMod::MonThread()
{
    DBG(D_INFO, (L"%s: enter\n", FULL_FUNC_NAME));

    while ( true )
    {
        sem_wait(&_hWakeEvent);
        if ( _bQuit )
        {
            break;
        }

        ProbeCmds();
    }

    DBG(D_INFO, (L"%s: leave\n", FULL_FUNC_NAME));

    return 0;
}

void *CmdTableMod::MonThreadProc(void *arg)
{
    CmdTableMod  *pThis = static_cast<CmdTableMod*>(arg);

    pThis->MonThread();

    return NULL;
}

