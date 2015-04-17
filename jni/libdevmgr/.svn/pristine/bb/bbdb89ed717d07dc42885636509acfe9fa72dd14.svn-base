#include <StdAfx.h>
#include "DeviceCenter.h"


DeviceCenter *DeviceCenter::_instance = NULL;

DeviceCenter *DeviceCenter::Instance()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    if ( !_instance )
    {
        DBG(D_VERB, (L"%s: %ls\n", FULL_FUNC_NAME, L"CreateInstance"));
        _instance = new DeviceCenter();
    }
    return _instance;
}

void DeviceCenter::Destroy()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    delete _instance;
    _instance = NULL;
}

DeviceCenter::DeviceCenter()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    pthread_mutex_init(&_DeviceLock, NULL);
    pthread_mutex_init(&_ObserverLock, NULL);
}

DeviceCenter::~DeviceCenter()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    assert(_DeviceList.empty());
    assert(_ObserverList.empty());

    pthread_mutex_destroy(&_ObserverLock);
    pthread_mutex_destroy(&_DeviceLock);
}

DWORD DeviceCenter::Load()
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    return ERROR_SUCCESS;
}

void DeviceCenter::Unload()
{
    SmartDevice  *pDevice;
    DevObserver  *pObserver;

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    while ( !_DeviceList.empty() )
    {
        pDevice = _DeviceList.front();
        _DeviceList.pop_front();
        pDevice->Release();
    }

    while ( !_ObserverList.empty() )
    {
        pObserver = _ObserverList.front();
        _ObserverList.pop_front();
        pObserver->Release();
    }
}

DWORD DeviceCenter::OnDeviceAttached(SmartDevice *pDevice)
{
    list<SmartDevice*>::iterator  dev_itr;
    list<DevObserver*>::iterator  obs_itr;

    DBG(D_INFO, (L"%s: id(%hs)\n", FULL_FUNC_NAME, pDevice->Id().c_str()));

    pthread_mutex_lock(&_DeviceLock);
    dev_itr = find(_DeviceList.begin(), _DeviceList.end(), pDevice);
    if ( dev_itr != _DeviceList.end() )
    {
        DBG(D_WARN, (L"%s: already exist\n", FULL_FUNC_NAME));
        assert(0);
        pthread_mutex_unlock(&_DeviceLock);
        return ERROR_ALREADY_EXISTS;
    }
    _DeviceList.push_back(pDevice);
    pthread_mutex_unlock(&_DeviceLock);

    pDevice->AddRef();

    pthread_mutex_lock(&_ObserverLock);
    for ( obs_itr = _ObserverList.begin(); obs_itr != _ObserverList.end(); obs_itr++ )
    {
        (*obs_itr)->OnAttached(pDevice);
    }
    pthread_mutex_unlock(&_ObserverLock);

    return ERROR_SUCCESS;
}

DWORD DeviceCenter::OnDeviceDetached(SmartDevice *pDevice)
{
    list<SmartDevice*>::iterator  dev_itr;
    list<DevObserver*>::iterator  obs_itr;

    DBG(D_INFO, (L"%s: id(%hs)\n", FULL_FUNC_NAME, pDevice->Id().c_str()));

    pthread_mutex_lock(&_DeviceLock);
    dev_itr = find(_DeviceList.begin(), _DeviceList.end(), pDevice);
    if ( dev_itr == _DeviceList.end() )
    {
        DBG(D_WARN, (L"%s: not found\n", FULL_FUNC_NAME));
        assert(0);
        pthread_mutex_unlock(&_DeviceLock);
        return ERROR_NOT_FOUND;
    }
    _DeviceList.erase(dev_itr);
    pthread_mutex_unlock(&_DeviceLock);

    pDevice->Release();

    pthread_mutex_lock(&_ObserverLock);
    for ( obs_itr = _ObserverList.begin(); obs_itr != _ObserverList.end(); obs_itr++ )
    {
        (*obs_itr)->OnDetached(pDevice);
    }
    pthread_mutex_unlock(&_ObserverLock);

    return ERROR_SUCCESS;
}

void DeviceCenter::OnDeviceStateChanged(SmartDevice *pDevice)
{
    DBG(D_INFO, (L"%s: id(%hs), state(%d)\n", FULL_FUNC_NAME, pDevice->Id().c_str(), pDevice->GetState()));

    list<DevObserver*>::iterator  obs_itr;

    pthread_mutex_lock(&_ObserverLock);
    for ( obs_itr = _ObserverList.begin(); obs_itr != _ObserverList.end(); obs_itr++ )
    {
        (*obs_itr)->OnStateChanged(pDevice);
    }
    pthread_mutex_unlock(&_ObserverLock);
}

DWORD DeviceCenter::RegisterObserver(DevObserver *pObserver)
{
    list<SmartDevice*>::iterator  dev_itr;
    list<DevObserver*>::iterator  obs_itr;

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    pthread_mutex_lock(&_ObserverLock);
    obs_itr = find(_ObserverList.begin(), _ObserverList.end(), pObserver);
    if ( obs_itr != _ObserverList.end() )
    {
        DBG(D_WARN, (L"%s: already exist\n", FULL_FUNC_NAME));
        assert(0);
        pthread_mutex_unlock(&_ObserverLock);
        return ERROR_ALREADY_EXISTS;
    }
    _ObserverList.push_back(pObserver);
    pthread_mutex_unlock(&_ObserverLock);

    pObserver->AddRef();

    pthread_mutex_lock(&_DeviceLock);
    for ( dev_itr = _DeviceList.begin(); dev_itr != _DeviceList.end(); dev_itr++ )
    {
        pObserver->OnAttached(*dev_itr);
    }
    pthread_mutex_unlock(&_DeviceLock);

    return ERROR_SUCCESS;
}

DWORD DeviceCenter::UnregisterObserver(DevObserver *pObserver)
{
    list<DevObserver*>::iterator  itr;

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    pthread_mutex_lock(&_ObserverLock);
    itr = find(_ObserverList.begin(), _ObserverList.end(), pObserver);
    if ( itr == _ObserverList.end() )
    {
        DBG(D_WARN, (L"%s: not found\n", FULL_FUNC_NAME));
        assert(0);
        pthread_mutex_unlock(&_ObserverLock);
        return ERROR_NOT_FOUND;
    }
    _ObserverList.erase(itr);
    pthread_mutex_unlock(&_ObserverLock);

    pObserver->Release();

    return ERROR_SUCCESS;
}

DWORD DeviceCenter::GetDevice(const char *szId, SmartDevice *&pDevice)
{
    DWORD  dwRet = ERROR_NOT_FOUND;
    list<SmartDevice*>::iterator  itr;

    DBG(D_INFO, (L"%s: id(%hs)\n", FULL_FUNC_NAME, szId));

    pDevice = NULL;

    pthread_mutex_lock(&_DeviceLock);
    for ( itr = _DeviceList.begin(); itr != _DeviceList.end(); itr++ )
    {
        if ( (*itr)->IsEqual(szId) )
        {
            pDevice = *itr;
            pDevice->AddRef();
            dwRet = ERROR_SUCCESS;
            break;
        }
    }
    pthread_mutex_unlock(&_DeviceLock);

    return dwRet;
}

