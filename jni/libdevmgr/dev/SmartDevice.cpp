#include <StdAfx.h>
#include "SmartDevice.h"


SmartDevice::SmartDevice(const char *szDevId)
    : _DeviceId(szDevId)
    , _DeviceState(DEV_STATE_OFF)
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));
}

SmartDevice::~SmartDevice()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));
}

const string &SmartDevice::Id() const
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    return _DeviceId;
}

bool SmartDevice::IsEqual(const char *szId) const
{
    DBG(D_FUNC, (L"%s: %s ? %s\n", FULL_FUNC_NAME, _DeviceId.c_str(), szId));

    return (_DeviceId ==szId);
}

DEV_STATE SmartDevice::GetState() const
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    return _DeviceState;
}

