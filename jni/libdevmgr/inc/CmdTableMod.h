#ifndef _CMD_TABLE_MOD_H
#define _CMD_TABLE_MOD_H


#include "DbTableBase.h"
#include "SmartDevice.h"
#include "CmdObserver.h"
#include "DevObserver.h"


class CmdTableMod : public DbTableBase, public CmdObserver, public DevObserver
{
public:
    static CmdTableMod *Instance();
    static void Destroy();

private:
    static CmdTableMod        * _instance;

protected:
    CmdTableMod();
    virtual ~CmdTableMod();

public:
    DWORD Load();
    void Unload();

    void OnCmdUpdated();

    virtual void OnExecuting(SmartDevice *pDevice, DWORD dwCmdIdx, DEVCMD_CODE CmdCode);
    virtual void OnExecuted(SmartDevice *pDevice, DWORD dwCmdIdx, DEVCMD_CODE CmdCode, BufferItem *pbiResult);
    virtual void OnFailed(SmartDevice *pDevice, DWORD dwCmdIdx, DEVCMD_CODE CmdCode, DWORD dwResult);
    virtual void OnCancelled(SmartDevice *pDevice, DWORD dwCmdIdx, DEVCMD_CODE CmdCode);

    virtual void OnAttached(SmartDevice *pDevice);
    virtual void OnDetached(SmartDevice *pDevice);
    virtual void OnStateChanged(SmartDevice *pDevice);

protected:
    void UpdateResult(DWORD dwCmdIdx, DWORD dwResult);

private:
    void ProbeCmds();
    DWORD ProcessCmd(char **row);
    DWORD ConvertArgs(DEVCMD_CODE CmdCode, const char *szArgs, BufferItem &biArgs);

    DWORD MonThread();
    static void *MonThreadProc(void *arg);

private:
    volatile bool               _bQuit;
    pthread_t                   _hMonThread;
    sem_t                       _hWakeEvent;
};


#endif

