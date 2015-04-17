#include <StdAfx.h>
#ifdef _WIN32_
#include "setupapi.h"
#endif
#include "AdbCmdEnumDev.h"
#include "SmartDeviceAdb.h"
#include "DeviceCenter.h"
#include "AdbDeviceMon.h"


#ifdef _WIN32_
//const GUID  UsbDevIntfGuid = {0xA5DCBF10, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED};
const GUID  AdbDevIntfGuid = {0xF72FE0D4, 0xCBCB, 0x407d, 0x88, 0x14, 0x9E, 0xD6, 0x73, 0xD0, 0xDD, 0x6B};
#endif

AdbDeviceMon *AdbDeviceMon::_instance = NULL;

AdbDeviceMon *AdbDeviceMon::Instance()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    if ( !_instance )
    {
        DBG(D_VERB, (L"%s: %ls\n", FULL_FUNC_NAME, L"CreateInstance"));
        _instance = new AdbDeviceMon();
    }
    return _instance;
}

void AdbDeviceMon::Destroy()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    delete _instance;
    _instance = NULL;
}

AdbDeviceMon::AdbDeviceMon()
    : _bQuit(false)
#ifdef _LINUX_
//    , _CountStrCmd("cat /proc/bus/usb/devices", "Cls=ff(vend.) Sub=42")
    , _CountStrCmd("cat /sys/kernel/debug/usb/devices", "Cls=ff(vend.) Sub=42")
#endif
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    SET_PTH_INVALID(_hMonThread);

    sem_init(&_hWakeEvent, 0, 0);
}

AdbDeviceMon::~AdbDeviceMon()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    sem_destroy(&_hWakeEvent);

    assert(!IS_PTH_VALID(_hMonThread));
}

DWORD AdbDeviceMon::Load()
{
    DWORD  dwRet;
    int  ret;

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    ret = pthread_create(&_hMonThread, NULL, MonThreadProc, this);
    if ( ret )
    {
        dwRet = GetLastError();
        DBG(D_ERROR, (L"%s: create thread failed: %d\n", FULL_FUNC_NAME, dwRet));
        goto err;
    }

    return ERROR_SUCCESS;

err:
    return dwRet;
}

void AdbDeviceMon::Unload()
{
    SmartDeviceAdb  *pDevice;

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    if ( IS_PTH_VALID(_hMonThread) )
    {
        _bQuit = true;
        sem_post(&_hWakeEvent);
        pthread_join(_hMonThread, NULL);
        SET_PTH_INVALID(_hMonThread);
    }

    while ( !_DeviceList.empty() )
    {
        pDevice = _DeviceList.front();
        _DeviceList.pop_front();
        DeviceCenter::Instance()->OnDeviceDetached(pDevice);
        pDevice->Release();
    }
}

void AdbDeviceMon::OnChanged()
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    sem_post(&_hWakeEvent);
}

#ifdef _WIN32_
DWORD AdbDeviceMon::GetDeviceCount(DWORD &dwCount)
{
    DWORD  dwRet;
    HDEVINFO  hDevInfo;
    SP_DEVICE_INTERFACE_DATA  IntfData;

    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    dwCount = 0;

    hDevInfo = SetupDiGetClassDevs(&AdbDevIntfGuid, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
    if ( hDevInfo == INVALID_HANDLE_VALUE )
    {
        dwRet = GetLastError();
        DBG(D_WARN, (L"%s: SetupDiGetClassDevs failed: %d\n", FULL_FUNC_NAME, dwRet));
        goto err1;
    }

    for ( DWORD i = 0; ; i++ )
    {
        IntfData.cbSize = sizeof(IntfData);
        if ( !SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &AdbDevIntfGuid, i, &IntfData) )
        {
            break;
        }

        if ( IntfData.Flags & SPINT_ACTIVE )
        {
            dwCount++;
        }
    }

    dwRet = ERROR_SUCCESS;

err1:
    SetupDiDestroyDeviceInfoList(hDevInfo);

    return dwRet;
}
#endif

#ifdef _LINUX_
DWORD AdbDeviceMon::GetDeviceCount(DWORD &dwCount)
{
    DWORD  dwRet;
    DWORD  dwCnt, dwBytes;

    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    dwCount = -1;

    dwRet = _CountStrCmd.Execute();
    if ( dwRet != ERROR_SUCCESS )
    {
        DBG(D_WARN, (L"%s: ExtCmdCountStr failed: %d\n", FULL_FUNC_NAME, dwRet));
        return dwRet;
    }

    dwBytes = sizeof(dwCnt);
    dwRet = _CountStrCmd.GetResult((PBYTE)&dwCnt, dwBytes);
    if ( dwRet != ERROR_SUCCESS )
    {
        DBG(D_WARN, (L"%s: ExtCmdCountStr failed: %d\n", FULL_FUNC_NAME, dwRet));
        return dwRet;
    }

    dwCount = dwCnt;
    DBG(D_INFO, (L"%s: ExtCmdCountStr: cnt(%d)\n", FULL_FUNC_NAME, dwCount));

    return ERROR_SUCCESS;
}
#endif

DWORD AdbDeviceMon::EnumDevices()
{
    DWORD  dwRet;
    DWORD  dwSize;

    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    dwRet = _EnumDevCmd.Execute();
    if ( dwRet != ERROR_SUCCESS )
    {
        DBG(D_WARN, (L"%s: enum devices failed: %d\n", FULL_FUNC_NAME, dwRet));
        return dwRet;
    }

    _EnumDevCmd.GetResult(NULL, dwSize);
    assert(dwSize >= (sizeof(RET_ENUM_DEVICES) - MAX_DEV_ID_LEN));

    _biResult.Alloc(dwSize);
    _EnumDevCmd.GetResult(_biResult.Put(dwSize), dwSize);

    ProcessDevices((PRET_ENUM_DEVICES)_biResult.data());

    return ERROR_SUCCESS;
}

void AdbDeviceMon::ProcessDevices(PRET_ENUM_DEVICES pEnumDevices)
{
    list<SmartDeviceAdb*>  NewList;
    list<SmartDeviceAdb*>::iterator  itr;
    SmartDeviceAdb  *pDevice;

    DBG(D_INFO, (L"%s: %d\n", FULL_FUNC_NAME, pEnumDevices->dwDevNum));

    NewList.splice(NewList.begin(), _DeviceList);

    for ( DWORD i = 0; i < pEnumDevices->dwDevNum; i++ )
    {
        for ( itr = NewList.begin(); itr != NewList.end(); itr++ )
        {
            if ( (*itr)->IsEqual((const char*)pEnumDevices->szDevId[i]) )
            {
                break;
            }

        }

        if ( itr == NewList.end() )
        {
            pDevice = new SmartDeviceAdb((const char*)pEnumDevices->szDevId[i]);
            DeviceCenter::Instance()->OnDeviceAttached(pDevice);
        }
        else
        {
            pDevice = *itr;
            NewList.erase(itr);
        }
        _DeviceList.push_back(pDevice);
    }

    while ( !NewList.empty() )
    {
        pDevice = NewList.front();
        NewList.pop_front();
        DeviceCenter::Instance()->OnDeviceDetached(pDevice);
        pDevice->Release();
    }
}

void AdbDeviceMon::UpdateDevicesState(bool &bQueryAgain)
{
    list<SmartDeviceAdb*>::iterator  itr;
    SmartDeviceAdb  *pDevice;
    bool  bFinished, bChanged;

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    bQueryAgain = false;

    for ( itr = _DeviceList.begin(); itr != _DeviceList.end(); itr++ )
    {
        pDevice = *itr;

        pDevice->UpdateState(bFinished, bChanged);
        if ( !bFinished )
        {
            bQueryAgain = true;
        }
        if ( bChanged )
        {
            DeviceCenter::Instance()->OnDeviceStateChanged(pDevice);
        }
    }
}

DWORD AdbDeviceMon::MonThread()
{
    int  ret;
    bool  bQueryAgain;
    DWORD  dwCnt;
    
    DBG(D_INFO, (L"%s: enter\n", FULL_FUNC_NAME));

    bQueryAgain = false;

    while ( true )
    {
        if ( bQueryAgain )
        {
            ret = sem_timedwait2(&_hWakeEvent, ADB_DEV_QUERY_TIMER);
        }
        else
        {
            ret = sem_wait(&_hWakeEvent);
        }

        if ( _bQuit )
        {
            break;
        }

        if ( !ret )
        {
            GetDeviceCount(dwCnt);
            for ( int nRetry = 0; nRetry < ADB_DEV_PROBE_COUNT; nRetry++ )
            {
                EnumDevices();
                if ( _DeviceList.size() == dwCnt )
                {
                    break;
                }
                Sleep(ADB_DEV_PROBE_TIMER);
            }
        }

        UpdateDevicesState(bQueryAgain);
    }

    DBG(D_INFO, (L"%s: leave\n", FULL_FUNC_NAME));

    return 0;
}

void *AdbDeviceMon::MonThreadProc(void *arg)
{
    AdbDeviceMon  *pThis = static_cast<AdbDeviceMon*>(arg);

    pThis->MonThread();

    return NULL;
}

