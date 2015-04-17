#ifndef _DEV_TABLE_MOD_H
#define _DEV_TABLE_MOD_H


#include "DbTableBase.h"
#include "SmartDevice.h"
#include "CmdObserver.h"
#include "DevObserver.h"


class DevTableMod : public DbTableBase, public CmdObserver, public DevObserver
{
public:
    static DevTableMod *Instance();
    static void Destroy();

private:
    static DevTableMod        * _instance;

protected:
    DevTableMod();
    virtual ~DevTableMod();

public:
    DWORD Load();
    void Unload();

    DWORD UpdateSysInfo(const char *szDevId);
    DWORD UpdateSysInfo(SmartDevice *pDevice);
    DWORD UpdateBattInfo(const char *szDevId);
    DWORD UpdateBattInfo(SmartDevice *pDevice);
    DWORD UpdateDiskInfo(const char *szDevId);
    DWORD UpdateDiskInfo(SmartDevice *pDevice);
    DWORD FindMe(const char *szDevId);
    DWORD FindMe(SmartDevice *pDevice);

    virtual void OnExecuting(SmartDevice *pDevice, DWORD dwCmdIdx, DEVCMD_CODE CmdCode);
    virtual void OnExecuted(SmartDevice *pDevice, DWORD dwCmdIdx, DEVCMD_CODE CmdCode, BufferItem *pbiResult);
    virtual void OnFailed(SmartDevice *pDevice, DWORD dwCmdIdx, DEVCMD_CODE CmdCode, DWORD dwResult);
    virtual void OnCancelled(SmartDevice *pDevice, DWORD dwCmdIdx, DEVCMD_CODE CmdCode);

    virtual void OnAttached(SmartDevice *pDevice);
    virtual void OnDetached(SmartDevice *pDevice);
    virtual void OnStateChanged(SmartDevice *pDevice);
};


#endif

