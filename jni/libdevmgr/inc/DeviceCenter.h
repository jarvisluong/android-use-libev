#ifndef _DEVICE_CENTER_H
#define _DEVICE_CENTER_H


#include "SmartDevice.h"
#include "DevObserver.h"


class DeviceCenter
{
public:
    static DeviceCenter *Instance();
    static void Destroy();

private:
    static DeviceCenter       * _instance;

protected:
    DeviceCenter();
    ~DeviceCenter();

public:
    DWORD Load();
    void Unload();

    DWORD OnDeviceAttached(SmartDevice *pDevice);
    DWORD OnDeviceDetached(SmartDevice *pDevice);
    void OnDeviceStateChanged(SmartDevice *pDevice);

    DWORD RegisterObserver(DevObserver *pObserver);
    DWORD UnregisterObserver(DevObserver *pObserver);

    DWORD GetDevice(const char *szId, SmartDevice *&pDevice);

private:
    pthread_mutex_t             _DeviceLock;
    list<SmartDevice*>          _DeviceList;

    pthread_mutex_t             _ObserverLock;
    list<DevObserver*>          _ObserverList;
};


#endif

