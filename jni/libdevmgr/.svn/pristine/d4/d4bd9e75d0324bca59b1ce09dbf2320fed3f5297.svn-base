#ifndef _ADB_DEVICE_MON_H
#define _ADB_DEVICE_MON_H


#include "AdbCmdEnumDev.h"
#ifdef _LINUX_
#include "ExtCmdCountStr.h"
#endif

class SmartDeviceAdb;

#ifdef _WIN32_
//extern const GUID  UsbDevIntfGuid;
extern const GUID  AdbDevIntfGuid;
#endif


class AdbDeviceMon
{
public:
    static AdbDeviceMon *Instance();
    static void Destroy();

private:
    static AdbDeviceMon       * _instance;

protected:
    AdbDeviceMon();
    ~AdbDeviceMon();

public:
    DWORD Load();
    void Unload();

    void OnChanged();

private:
    DWORD GetDeviceCount(DWORD &dwCount);
    DWORD EnumDevices();
    void ProcessDevices(PRET_ENUM_DEVICES pEnumDevices);

    void UpdateDevicesState(bool &bQueryAgain);

    DWORD MonThread();
    static void *MonThreadProc(void *arg);

private:
    list<SmartDeviceAdb*>       _DeviceList;

    BufferItem                  _biResult;
    AdbCmdEnumDev               _EnumDevCmd;

    volatile bool               _bQuit;
    pthread_t                   _hMonThread;
    sem_t                       _hWakeEvent;

#ifdef _LINUX_
    ExtCmdCountStr              _CountStrCmd;
#endif
};


#endif

