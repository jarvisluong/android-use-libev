#include <StdAfx.h>
#include "AdbCmdGetProp.h"
#include "AdbCmdInstallApk.h"
#include "AdbCmdUninstallApk.h"
#include "AdbCmdPushPullFile.h"
#include "AdbCmdStartService.h"
#include "AdbCmdGetSysInfo.h"
#include "AdbCmdGetBattInfo.h"
#include "AdbCmdGetDiskInfo.h"
#include "AdbCmdGetApkList.h"
#include "AdbCmdScreenCap.h"
#include "AdbCmdDelFile.h"
#include "SmartDeviceAdb.h"


volatile bool  SmartDeviceAdb::_bExecQuit;
pthread_t  SmartDeviceAdb::_hExecThread[ADB_DEV_THREAD_CNT];
sem_t  SmartDeviceAdb::_hExecEvent[ADB_DEV_QUEUE_CNT];
pthread_mutex_t  SmartDeviceAdb::_ExecLock[ADB_DEV_QUEUE_CNT];
list<SmartDeviceAdb*>  SmartDeviceAdb::_ExecList[ADB_DEV_QUEUE_CNT];

DWORD SmartDeviceAdb::Startup()
{
    DWORD  dwRet;
    int  ret;
    int  i, j, k;

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    assert(_countof(ADB_CMD_QUEUE_MAP) == C_DEVCMD_LAST_ONE);
    assert(_countof(ADB_CMD_THREAD_MAP) == ADB_DEV_QUEUE_CNT);

    _bExecQuit = false;

    for ( i = 0; i < ADB_DEV_THREAD_CNT; i++ )
    {
        SET_PTH_INVALID(_hExecThread[i]);
    }

    for ( i = 0; i < ADB_DEV_QUEUE_CNT; i++ )
    {
        sem_init(&_hExecEvent[i], 0, 0);
        pthread_mutex_init(&_ExecLock[i], NULL);
    }

    k = 0;
    for ( i = 0; i < ADB_DEV_QUEUE_CNT; i++ )
    {
        for ( j = 0; j < ADB_CMD_THREAD_MAP[i]; j++ )
        {
            assert(k < ADB_DEV_THREAD_CNT);
            ret = pthread_create(&_hExecThread[k], NULL, WorkThread, reinterpret_cast<void*>(i));
            if ( ret )
            {
                dwRet = GetLastError();
                DBG(D_ERROR, (L"%s: create thread_%d for queue(%d) failed: %d\n", FULL_FUNC_NAME, j, i, dwRet));
                SET_PTH_INVALID(_hExecThread[k]);
                goto err1;
            }
            k++;
        }
    }

    return ERROR_SUCCESS;

err1:
    Cleanup();

    return dwRet;
}

void SmartDeviceAdb::Cleanup()
{
    int  i, j;

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    _bExecQuit = true;

    for ( i = 0; i < ADB_DEV_QUEUE_CNT; i++ )
    {
        for ( j = 0; j < ADB_CMD_THREAD_MAP[i]; j++ )
        {
            sem_post(&_hExecEvent[i]);
        }
    }

    for ( i = 0; i < ADB_DEV_THREAD_CNT; i++ )
    {
        if ( IS_PTH_VALID(_hExecThread[i]) )
        {
            pthread_join(_hExecThread[i], NULL);
            SET_PTH_INVALID(_hExecThread[i]);
        }
    }

    for ( i = 0; i < ADB_DEV_QUEUE_CNT; i++ )
    {
        sem_destroy(&_hExecEvent[i]);
        pthread_mutex_destroy(&_ExecLock[i]);
    }
}

DWORD SmartDeviceAdb::Request(SmartDeviceAdb *pThis, int nQueue)
{
    bool bEmpty;

    DBG(D_VERB, (L"%s: queue(%d)\n", FULL_FUNC_NAME, nQueue));

    assert(nQueue < ADB_DEV_QUEUE_CNT);

    pthread_mutex_lock(&_ExecLock[nQueue]);

    if ( find(_ExecList[nQueue].begin(), _ExecList[nQueue].end(), pThis) != _ExecList[nQueue].end() )
    {
        assert(0);
        DBG(D_WARN, (L"%s: dev(%p) alreaady exist in queue(%d)\n", FULL_FUNC_NAME));
        pthread_mutex_unlock(&_ExecLock[nQueue]);
        return ERROR_ALREADY_EXISTS;
    }

    bEmpty = _ExecList[nQueue].empty();
    pThis->AddRef();
    _ExecList[nQueue].push_back(pThis);

    pthread_mutex_unlock(&_ExecLock[nQueue]);

    if ( bEmpty )
    {
        sem_post(&_hExecEvent[nQueue]);
    }

    return ERROR_SUCCESS;
}

void *SmartDeviceAdb::WorkThread(void *arg)
{
    int  nQueue;
    sem_t  *pSem;
    pthread_mutex_t  *pLock;
    list<SmartDeviceAdb*>  *pList;
    SmartDeviceAdb  *pDevice;

    nQueue = reinterpret_cast<intptr_t>(arg);

    DBG(D_INFO, (L"%s: queue(%d) enter\n", FULL_FUNC_NAME, nQueue));

    pSem = &_hExecEvent[nQueue];
    pLock = &_ExecLock[nQueue];
    pList = &_ExecList[nQueue];

    while ( !_bExecQuit )
    {
        sem_wait(pSem);
        
        pthread_mutex_lock(pLock);
        while ( !pList->empty() )
        {
            pDevice = pList->front();
            pList->pop_front();

            pthread_mutex_unlock(pLock);
            pDevice->Execute(nQueue);
            pDevice->Release();
            pthread_mutex_lock(pLock);
        }
        pthread_mutex_unlock(pLock);
    }

    DBG(D_INFO, (L"%s: queue(%d) leave\n", FULL_FUNC_NAME, nQueue));

    return NULL;
}

SmartDeviceAdb::SmartDeviceAdb(const char *szDevId)
    : SmartDevice(szDevId)
{
    int  i;

    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    _DeviceState = DEV_STATE_BOOTING;

    for ( i = 0; i < ADB_DEV_QUEUE_CNT; i++ )
    {
        pthread_mutex_init(&_CmdLock[i], NULL);
        _bRunning[i] = false;
    }
}

SmartDeviceAdb::~SmartDeviceAdb()
{
    int  i;

    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    FlushCmds();

    for ( i = 0; i < ADB_DEV_QUEUE_CNT; i++ )
    {
        pthread_mutex_destroy(&_CmdLock[i]);
    }
}

DWORD SmartDeviceAdb::UpdateState(bool &bFinished, bool &bChanged)
{
    AdbCmdGetProp  cmd(_DeviceId.c_str());
    DWORD  dwBytes;
    BYTE  buf[200];

    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    bFinished = true;
    bChanged = false;

    if ( _DeviceState != DEV_STATE_BOOTING )
    {
        return ERROR_SUCCESS;
    }

    cmd.SetPropName("ro.bootmode");
    cmd.Execute();
    dwBytes = sizeof(buf);
    cmd.GetResult(buf, dwBytes);
    if ( buf[0] == 'c' )
    {
        DBG(D_INFO, (L"%s: dev(%hs) DEV_STATE_CHARGER\n", FULL_FUNC_NAME, _DeviceId.c_str()));
        _DeviceState = DEV_STATE_CHARGER;
        bChanged = true;
        return ERROR_SUCCESS;
    }

    cmd.SetPropName("dev.bootcomplete");
    cmd.Execute();
    dwBytes = sizeof(buf);
    cmd.GetResult(buf, dwBytes);
    if ( buf[0] == '1' )
    {
        DBG(D_INFO, (L"%s: dev(%hs) DEV_STATE_MAIN\n", FULL_FUNC_NAME, _DeviceId.c_str()));
        _DeviceState = DEV_STATE_MAIN;
        bChanged = true;
        return ERROR_SUCCESS;
    }

    bFinished = false;
    DBG(D_INFO, (L"%s: dev(%hs) DEV_STATE_BOOTING\n", FULL_FUNC_NAME, _DeviceId.c_str()));

    return ERROR_SUCCESS;
}

DWORD SmartDeviceAdb::QueueCmd(DWORD dwCmdIdx, DEVCMD_CODE CmdCode, BufferItem *pbiArgs, CmdObserver *pObserver)
{
    PADB_CMD_REC  pRec;
    AdbCmd  *pCmd;
    int  nQueue;

    DBG(D_INFO, (L"%s: idx(%d), code(%d)\n", FULL_FUNC_NAME, dwCmdIdx, CmdCode));

    pRec = NULL;
    pCmd = NULL;
    try
    {
        pRec = new ADB_CMD_REC;
        pCmd = CreateCmd(CmdCode, pbiArgs);
    }
    catch(...)
    {
        DBG(D_ERROR, (L"%s exception\n", FULL_FUNC_NAME));
        if ( pRec )
        {
            delete pRec;
            pRec = NULL;
        }
        if ( pCmd )
        {
            delete pCmd;
            pCmd = NULL;
        }
    }

    if ( !pRec )
    {
        return ERROR_OUTOFMEMORY;
    }

    pRec->dwIdx = dwCmdIdx;
    pRec->CmdCode = CmdCode;
    pRec->pCmd = pCmd;
    pObserver->AddRef();
    pRec->pObserver = pObserver;

    nQueue = ADB_CMD_QUEUE_MAP[CmdCode];

    pthread_mutex_lock(&_CmdLock[nQueue]);
    _CmdList[nQueue].push(pRec);
    // assure no thread process the queue
    if ( (_CmdList[nQueue].size() == 1) && (!_bRunning[nQueue]) )
    {
        Request(this, nQueue);
    }
    pthread_mutex_unlock(&_CmdLock[nQueue]);

    return ERROR_SUCCESS;
}

void SmartDeviceAdb::FlushCmds()
{
    PADB_CMD_REC  pRec;
    int  i;

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    for ( i = 0; i < ADB_DEV_QUEUE_CNT; i++ )
    {
        pthread_mutex_lock(&_CmdLock[i]);
        while ( !_CmdList[i].empty() )
        {
            pRec = _CmdList[i].front();
            _CmdList[i].pop();

            pthread_mutex_unlock(&_CmdLock[i]);

            pRec->pObserver->OnCancelled(this, pRec->dwIdx, pRec->CmdCode);

            FreeCmdRec(pRec);

            pthread_mutex_lock(&_CmdLock[i]);
        }
        pthread_mutex_unlock(&_CmdLock[i]);
    }
}

void SmartDeviceAdb::Execute(int nQueue)
{
    PADB_CMD_REC  pRec;

    DBG(D_INFO, (L"%s: dev(%hs), queue(%d)\n", FULL_FUNC_NAME, _DeviceId.c_str(), nQueue));

    assert(nQueue < ADB_DEV_QUEUE_CNT);

    pthread_mutex_lock(&_CmdLock[nQueue]);

    _bRunning[nQueue] = true;

    while ( !_CmdList[nQueue].empty() )
    {
        pRec = _CmdList[nQueue].front();
        _CmdList[nQueue].pop();

        pthread_mutex_unlock(&_CmdLock[nQueue]);

        ExecuteOne(pRec);
        FreeCmdRec(pRec);

        pthread_mutex_lock(&_CmdLock[nQueue]);
    }

    _bRunning[nQueue] = false;

    pthread_mutex_unlock(&_CmdLock[nQueue]);
}

void SmartDeviceAdb::ExecuteOne(PADB_CMD_REC pRec)
{
    DWORD  dwRet;
    BufferItem  biResult;
    DWORD  dwSize;

    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    if ( _DeviceState != DEV_STATE_MAIN )
    {
        // if ASUS's tablet power off, some cmd such as "dumpsys battery" will hang
        pRec->pObserver->OnCancelled(this, pRec->dwIdx, pRec->CmdCode);
        return;
    }


    pRec->pObserver->OnExecuting(this, pRec->dwIdx, pRec->CmdCode);

    dwRet = pRec->pCmd->Execute();
    if ( dwRet != ERROR_SUCCESS )
    {
        pRec->pObserver->OnFailed(this, pRec->dwIdx, pRec->CmdCode, dwRet);
        return;
    }

    pRec->pCmd->GetResult(NULL, dwSize);
    if ( dwSize )
    {
        biResult.Alloc(dwSize);
        pRec->pCmd->GetResult(biResult.Put(dwSize), dwSize);
    }
    pRec->pObserver->OnExecuted(this, pRec->dwIdx, pRec->CmdCode, &biResult);
}

AdbCmd *SmartDeviceAdb::CreateCmd(DEVCMD_CODE CmdCode, BufferItem *pbiArgs)
{
    AdbCmd  *pCmd;

    DBG(D_VERB, (L"%s: code(%d)\n", FULL_FUNC_NAME, CmdCode));

    switch ( CmdCode )
    {
        case C_DEVCMD_INSTALL_APP:
        case C_DEVCMD_INSTALL_INFOVIEW:
        {
            PARGS_INSTALL_APP  pIns = (PARGS_INSTALL_APP)pbiArgs->data();
            pCmd = new AdbCmdInstallApk(_DeviceId.c_str(), (char*)pIns->szAppPath);
            break;
        }

        case C_DEVCMD_UNINSTALL_APP:
        {
            PARGS_UNINSTALL_APP  pUnins = (PARGS_UNINSTALL_APP)pbiArgs->data();
            pCmd = new AdbCmdUninstallApk(_DeviceId.c_str(), (char*)pUnins->szAppName);
            break;
        }

        case C_DEVCMD_PUSH_FILE:
        case C_DEVCMD_PULL_FILE:
        {
            PARGS_PUSH_PULL_FILE  pPush = (PARGS_PUSH_PULL_FILE)pbiArgs->data();
            pCmd = new AdbCmdPushPullFile(_DeviceId.c_str(), (char*)pPush->szPathH, (char*)pPush->szPathD, (char*)pPush->szFileD, (CmdCode==C_DEVCMD_PUSH_FILE));
            break;
        }

        case C_DEVCMD_LABEL_SHOW:
        {
            char  buf[100];
            PARGS_SHOW_LABEL  pShowLabel = (PARGS_SHOW_LABEL)pbiArgs->data();
            _snprintf(buf, _countof(buf), "--ei FAST %d --ei NUM %u", pShowLabel->bFastCharging, (UINT)pShowLabel->dwNum);
            pCmd = new AdbCmdStartService(_DeviceId.c_str(), ADB_SERVICED_NAME, true, buf);
            break;
        }

        case C_DEVCMD_GET_SYSINFO:
        {
            PARGS_GET_SYSINFO  pSysInfo = (PARGS_GET_SYSINFO)pbiArgs->data();
            pCmd = new AdbCmdGetSysInfo(_DeviceId.c_str(), pSysInfo->dwMask);
            break;
        }

        case C_DEVCMD_GET_BATTINFO:
        {
//            PARGS_GET_BATTINFO  pBattInfo = (PARGS_GET_BATTINFO)pbiArgs->data();
            pCmd = new AdbCmdGetBattInfo(_DeviceId.c_str());
            break;
        }

        case C_DEVCMD_GET_DISKINFO:
        {
//            PARGS_GET_DISKINFO  pDiskInfo = (PARGS_GET_DISKINFO)pbiArgs->data();
            pCmd = new AdbCmdGetDiskInfo(_DeviceId.c_str());
            break;
        }

        case C_DEVCMD_GET_APPLIST:
        {
            PARGS_GET_APPLIST  pAppList = (PARGS_GET_APPLIST)pbiArgs->data();
            pCmd = new AdbCmdGetApkList(_DeviceId.c_str(), (char*)pAppList->szIconDir);
            break;
        }

        case C_DEVCMD_SCREEN_CAP:
        {
            PARGS_SCREEN_CAP  pScrCap = (PARGS_SCREEN_CAP)pbiArgs->data();
            pCmd = new AdbCmdScreenCap(_DeviceId.c_str(), (char*)pScrCap->szFileName);
            break;
        }

        case C_DEVCMD_DEL_FILE:
        {
            PARGS_DEL_FILE  pDelFile = (PARGS_DEL_FILE)pbiArgs->data();
            pCmd = new AdbCmdDelFile(_DeviceId.c_str(), (char*)pDelFile->szFileName);
            break;
        }

        case C_DEVCMD_FIND_ME:
        {
            const char  *cmd = "--ei SOUND 1 --ei VIBRATOR 1";
            pCmd = new AdbCmdStartService(_DeviceId.c_str(), ADB_SERVICED_NAME, true, cmd);
            break;
        }

        default:
            assert(0);
            pCmd = NULL;
    }

    return pCmd;
}

void SmartDeviceAdb::FreeCmdRec(PADB_CMD_REC pRec)
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    pRec->pObserver->Release();
    delete pRec->pCmd;
    delete pRec;
}

