#ifndef _SMART_DEVICE_ADB_H
#define _SMART_DEVICE_ADB_H


#include "SmartDevice.h"
#include "AdbCmd.h"


typedef struct _ADB_CMD_REC
{
    DWORD                       dwIdx;
    DEVCMD_CODE                 CmdCode;
    AdbCmd                    * pCmd;
    CmdObserver               * pObserver;
}
ADB_CMD_REC, *PADB_CMD_REC;


class SmartDeviceAdb : public SmartDevice
{
public:
    static DWORD Startup();
    static void Cleanup();

protected:
    static DWORD Request(SmartDeviceAdb *pThis, int nQueue);

private:
    static void *WorkThread(void *arg);

private:
    static volatile bool        _bExecQuit;
    static pthread_t            _hExecThread[ADB_DEV_THREAD_CNT];
    static sem_t                _hExecEvent[ADB_DEV_QUEUE_CNT];
    static pthread_mutex_t      _ExecLock[ADB_DEV_QUEUE_CNT];
    static list<SmartDeviceAdb*> _ExecList[ADB_DEV_QUEUE_CNT];

public:
    SmartDeviceAdb(const char *szDevId);
protected:
    virtual ~SmartDeviceAdb();

public:
    DWORD UpdateState(bool &bFinished, bool &bChanged);

    virtual DWORD QueueCmd(DWORD dwCmdIdx, DEVCMD_CODE CmdCode, BufferItem *pbiArgs, CmdObserver *pObserver);
    virtual void FlushCmds();

protected:
    virtual void Execute(int nQueue);
    virtual void ExecuteOne(PADB_CMD_REC pRec);

    AdbCmd *CreateCmd(DEVCMD_CODE CmdCode, BufferItem *pbiArgs);
    void FreeCmdRec(PADB_CMD_REC pRec);

protected:
    pthread_mutex_t             _CmdLock[ADB_DEV_QUEUE_CNT];
    queue<PADB_CMD_REC>         _CmdList[ADB_DEV_QUEUE_CNT];
    bool                        _bRunning[ADB_DEV_QUEUE_CNT];
};


#endif

