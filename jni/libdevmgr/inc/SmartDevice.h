#ifndef _SMART_DEVICE_H
#define _SMART_DEVICE_H


#include "BufferItem.h"
#include "PObject.h"
#include "CmdObserver.h"


class SmartDevice : public PObject
{
public:
    SmartDevice(const char *szDevId);
protected:
    virtual ~SmartDevice();

public:
    const string &Id() const;
    bool IsEqual(const char *szId) const;
    DEV_STATE GetState() const;

    virtual DWORD QueueCmd(DWORD dwCmdIdx, DEVCMD_CODE CmdCode, BufferItem *pbiArgs, CmdObserver *pObserver) = 0;
    virtual void FlushCmds() = 0;

protected:
    string                      _DeviceId;
    DEV_STATE                   _DeviceState;
};


#endif

