#include <StdAfx.h>
#include "MtpDeviceMon.h"


MtpDeviceMon *MtpDeviceMon::_instance = NULL;

MtpDeviceMon *MtpDeviceMon::Instance()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    if ( !_instance )
    {
        DBG(D_VERB, (L"%s: %ls\n", FULL_FUNC_NAME, L"CreateInstance"));
        _instance = new MtpDeviceMon();
    }
    return _instance;
}

void MtpDeviceMon::Destroy()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    delete _instance;
    _instance = NULL;
}

MtpDeviceMon::MtpDeviceMon()
    : _bQuit(false)
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    pthread_mutex_init(&_DeviceLock, NULL);

    SET_PTH_INVALID(_hMonThread);

    sem_init(&_hWakeEvent, 0, 0);
}

MtpDeviceMon::~MtpDeviceMon()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    sem_destroy(&_hWakeEvent);

    assert(!IS_PTH_VALID(_hMonThread));

    pthread_mutex_destroy(&_DeviceLock);
}

DWORD MtpDeviceMon::Load()
{
    DWORD  dwRet;
    int  ret;

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

#ifdef _LINUX_
    LIBMTP_Init();
#endif

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

void MtpDeviceMon::Unload()
{
    PMTPDEVICE_REC  pRec;

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    if ( IS_PTH_VALID(_hMonThread) )
    {
        _bQuit = true;
        sem_post(&_hWakeEvent);
        pthread_join(_hMonThread, NULL);
        SET_PTH_INVALID(_hMonThread);
    }

    pthread_mutex_lock(&_DeviceLock);
    while ( !_DeviceList.empty() )
    {
        pRec = _DeviceList.front();
        _DeviceList.pop_front();
        assert(!pRec->dwRefCnt);
        FreeRec(pRec);
    }
    pthread_mutex_unlock(&_DeviceLock);

#ifdef _LINUX_
    LIBMTP_Fini();
#endif
}

#ifdef _LINUX_
DWORD MtpDeviceMon::GetMtpDevice(const char *szId, LIBMTP_mtpdevice_t *&pDevice)
{
    DWORD  dwRet;
    list<PMTPDEVICE_REC>::iterator  itr;
    PMTPDEVICE_REC  pRec;

    DBG(D_VERB, (L"%s: %hs\n", FULL_FUNC_NAME, szId));

    pthread_mutex_lock(&_DeviceLock);

    for ( itr = _DeviceList.begin(); itr != _DeviceList.end(); itr++ )
    {
        pRec = *itr;
        if ( !strcmp(pRec->szId, szId) )
        {
            break;
        }
    }

    if ( itr == _DeviceList.end() )
    {
        DBG(D_WARN, (L"%s: dev(%hs) not found\n", FULL_FUNC_NAME, szId));
        pDevice = NULL;
        dwRet = ERROR_NOT_FOUND;
    }
    else
    {
        DBG(D_INFO, (L"%s: dev(%hs), ref(%d++)\n", FULL_FUNC_NAME, szId, pRec->dwRefCnt));
        pRec->dwRefCnt++;
        pDevice = pRec->pMtpDevice;
        assert(pDevice);
        dwRet = ERROR_SUCCESS;
    }

    pthread_mutex_unlock(&_DeviceLock);

    return dwRet;
}

void MtpDeviceMon::ReleaseMtpDevice(LIBMTP_mtpdevice_t *pDevice)
{
    list<PMTPDEVICE_REC>::iterator  itr;
    PMTPDEVICE_REC  pRec;

    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    pthread_mutex_lock(&_DeviceLock);

    for ( itr = _DeviceList.begin(); itr != _DeviceList.end(); itr++ )
    {
        pRec = *itr;
        if ( pRec->pMtpDevice == pDevice )
        {
            break;
        }
    }
    assert(itr != _DeviceList.end());

    DBG(D_INFO, (L"%s: dev(%hs), remove(%d), ref(%d--)\n", FULL_FUNC_NAME, pRec->szId, pRec->bRemove, pRec->dwRefCnt));
    pRec->dwRefCnt--;
    if ( pRec->bRemove && (!pRec->dwRefCnt) )
    {
        _DeviceList.erase(itr);
        FreeRec(pRec);
    }

    pthread_mutex_unlock(&_DeviceLock);
}
#endif

void MtpDeviceMon::OnChanged()
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    sem_post(&_hWakeEvent);
}

DWORD MtpDeviceMon::EnumDevices()
{
#ifdef _LINUX_
    LIBMTP_error_number_t  ret;
    LIBMTP_raw_device_t  *pDevices;
    int  nCount;

    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    ret = LIBMTP_Detect_Raw_Devices(&pDevices, &nCount);
    if ( ret != LIBMTP_ERROR_NONE )
    {
        DBG(D_WARN, (L"%s: LIBMTP_Detect_Raw_Devices failed: %d\n", FULL_FUNC_NAME, ret));
        return ERROR_NOT_SUPPORTED;
    }

    ProcessDevices(pDevices, nCount);

    free(pDevices);

    return ERROR_SUCCESS;
#else
    return ERROR_SUCCESS;
#endif
}

#ifdef _LINUX_
void MtpDeviceMon::ProcessDevices(LIBMTP_raw_device_t *pDevices, int nCount)
{
    DWORD  dwRet;
    list<PMTPDEVICE_REC>  DupList, NewList;
    list<PMTPDEVICE_REC>::iterator  itr;
    PMTPDEVICE_REC  pRec;

    DBG(D_INFO, (L"%s: %d\n", FULL_FUNC_NAME, nCount));

    pthread_mutex_lock(&_DeviceLock);

    DupList.splice(DupList.begin(), _DeviceList);

    for ( int i = 0; i < nCount; i++ )
    {
        for ( itr = DupList.begin(); itr != DupList.end(); itr++ )
        {
            if ( IsEqual((*itr)->pRawDevice, &pDevices[i]) )
            {
                break;
            }

        }

        if ( itr == DupList.end() )
        {
            pRec = CreateRec(&pDevices[i]);
            NewList.push_back(pRec);
        }
        else
        {
            pRec = *itr;
            DupList.erase(itr);
            _DeviceList.push_back(pRec);
        }
    }

    while ( !DupList.empty() )
    {
        pRec = DupList.front();
        DupList.pop_front();
        if ( pRec->dwRefCnt )
        {
            pRec->bRemove = true;
            _DeviceList.push_back(pRec);
        }
        else
        {
            FreeRec(pRec);
        }
    }

    pthread_mutex_unlock(&_DeviceLock);

    for ( itr = NewList.begin(); itr != NewList.end(); )
    {
        pRec = *itr;
        dwRet = InitRec(pRec);
        if ( dwRet != ERROR_SUCCESS )
        {
            DBG(D_WARN, (L"%s: InitRec failed: %d\n", FULL_FUNC_NAME, dwRet));
            itr = NewList.erase(itr);
            FreeRec(pRec);
        }
        else
        {
            itr++;
        }
    }

    pthread_mutex_lock(&_DeviceLock);
    _DeviceList.splice(_DeviceList.end(), NewList);
    pthread_mutex_unlock(&_DeviceLock);
}

bool MtpDeviceMon::IsEqual(LIBMTP_raw_device_t *pDevice1, LIBMTP_raw_device_t *pDevice2)
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    if (
         (pDevice1->bus_location == pDevice2->bus_location)
      && (pDevice1->devnum == pDevice2->devnum)
       )
    {
        return true;
    }

    return false;
}

PMTPDEVICE_REC MtpDeviceMon::CreateRec(LIBMTP_raw_device_t *pDevice)
{
    PMTPDEVICE_REC  pRec;

    pRec = new MTPDEVICE_REC;
    memset(pRec, 0, sizeof(MTPDEVICE_REC));

    pRec->szId = LIBMTP_Get_Serialnumber3(pDevice);
//    pRec->szId = LIBMTP_Get_Serialnumber2(pRec->pMtpDevice);

    assert((!pDevice->device_entry.vendor) && (!pDevice->device_entry.product));
    pRec->pRawDevice = new LIBMTP_raw_device_t;
    memcpy(pRec->pRawDevice, pDevice, sizeof(LIBMTP_raw_device_t));

    DBG(D_INFO, (L"%s: dev(%hs)\n", FULL_FUNC_NAME, pRec->szId));

    return pRec;
}
#endif

DWORD MtpDeviceMon::InitRec(PMTPDEVICE_REC pRec)
{
#ifdef _LINUX_
    DWORD  dwTicks;

    DBG(D_INFO, (L"%s: start Open_Raw_Device(%hs)\n", FULL_FUNC_NAME, pRec->szId));
    dwTicks = GetTickCount();
    pRec->pMtpDevice = LIBMTP_Open_Raw_Device/*_Uncached*/(pRec->pRawDevice);
    dwTicks = GetTickCount() - dwTicks;
    if ( !pRec->pMtpDevice )
    {
        DBG(D_WARN, (L"%s: Open_Raw_Device(%hs) failed\n", FULL_FUNC_NAME, pRec->szId));
#if 1
{
        char  buf[MAX_ADB_CMD_LEN];
        _snprintf(buf, _countof(buf), ADB_FMT_REBOOT_DEVICE, pRec->szId);
        DBG(D_INFO, (L"%s: rebooting device(%hs) and retry\n", FULL_FUNC_NAME, pRec->szId));
        system(buf);
}
#endif
        return ERROR_NOT_SUPPORTED;
    }
    DBG(D_INFO, (L"%s: finish Open_Raw_Device(%hs) @ %dms\n", FULL_FUNC_NAME, pRec->szId, dwTicks));

    return ERROR_SUCCESS;
#else
    return ERROR_SUCCESS;
#endif
}

void MtpDeviceMon::FreeRec(PMTPDEVICE_REC pRec)
{
#ifdef _LINUX_
    DBG(D_INFO, (L"%s: dev(%hs)\n", FULL_FUNC_NAME, (pRec->szId?pRec->szId:"?")));

    if ( pRec->pMtpDevice )
    {
        LIBMTP_Release_Device(pRec->pMtpDevice);
    }
    if ( pRec->pRawDevice )
    {
        delete pRec->pRawDevice;
    }
    if ( pRec->szId )
    {
        free(pRec->szId);
    }
    delete pRec;
#endif
}

DWORD MtpDeviceMon::MonThread()
{
    DBG(D_INFO, (L"%s: enter\n", FULL_FUNC_NAME));

    while ( true )
    {
        sem_wait(&_hWakeEvent);

        if ( _bQuit )
        {
            break;
        }

        EnumDevices();
    }

    DBG(D_INFO, (L"%s: leave\n", FULL_FUNC_NAME));

    return 0;
}

void *MtpDeviceMon::MonThreadProc(void *arg)
{
    MtpDeviceMon  *pThis = static_cast<MtpDeviceMon*>(arg);

    pThis->MonThread();

    return NULL;
}

