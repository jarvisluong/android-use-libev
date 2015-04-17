#ifndef _CMD_OBSERVER_H
#define _CMD_OBSERVER_H


#include "PObject.h"
#include "BufferItem.h"

class SmartDevice;


class CmdObserver : virtual public PObject
{
public:
    // be careful for the reentry
    virtual void OnExecuting(SmartDevice *pDevice, DWORD dwCmdIdx, DEVCMD_CODE CmdCode) = 0;
    virtual void OnExecuted(SmartDevice *pDevice, DWORD dwCmdIdx, DEVCMD_CODE CmdCode, BufferItem *pbiResult) = 0;
    virtual void OnFailed(SmartDevice *pDevice, DWORD dwCmdIdx, DEVCMD_CODE CmdCode, DWORD dwReason) = 0;
    virtual void OnCancelled(SmartDevice *pDevice, DWORD dwCmdIdx, DEVCMD_CODE CmdCode) = 0;
};


#endif

