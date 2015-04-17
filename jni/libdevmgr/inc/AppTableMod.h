#ifndef _APP_TABLE_MOD_H
#define _APP_TABLE_MOD_H


#include "DbTableBase.h"
#include "SmartDevice.h"
#include "CmdObserver.h"
#include "DevObserver.h"


class AppTableMod : public DbTableBase, public CmdObserver, public DevObserver
{
public:
    static AppTableMod *Instance();
    static void Destroy();

private:
    static AppTableMod        * _instance;

protected:
    AppTableMod();
    virtual ~AppTableMod();

public:
    DWORD Load();
    void Unload();

    DWORD UpdateAppList(const char *szDevId);
    DWORD UpdateAppList(SmartDevice *pDevice);

    virtual void OnExecuting(SmartDevice *pDevice, DWORD dwCmdIdx, DEVCMD_CODE CmdCode);
    virtual void OnExecuted(SmartDevice *pDevice, DWORD dwCmdIdx, DEVCMD_CODE CmdCode, BufferItem *pbiResult);
    virtual void OnFailed(SmartDevice *pDevice, DWORD dwCmdIdx, DEVCMD_CODE CmdCode, DWORD dwResult);
    virtual void OnCancelled(SmartDevice *pDevice, DWORD dwCmdIdx, DEVCMD_CODE CmdCode);

    virtual void OnAttached(SmartDevice *pDevice);
    virtual void OnDetached(SmartDevice *pDevice);
    virtual void OnStateChanged(SmartDevice *pDevice);
};


#endif

