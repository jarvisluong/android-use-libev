#ifndef _MTP_DEVICE_MON_H
#define _MTP_DEVICE_MON_H


#ifdef _LINUX_
#include <libmtp.h>
#endif
#include "BufferItem.h"


typedef struct _MTPDEVICE_REC
{
    bool                        bRemove;
    DWORD                       dwRefCnt;

    char                      * szId;

#ifdef _LINUX_
    LIBMTP_raw_device_t       * pRawDevice;
    LIBMTP_mtpdevice_t        * pMtpDevice;
#endif
}
MTPDEVICE_REC, *PMTPDEVICE_REC;


class MtpDeviceMon
{
public:
    static MtpDeviceMon *Instance();
    static void Destroy();

private:
    static MtpDeviceMon       * _instance;

protected:
    MtpDeviceMon();
    ~MtpDeviceMon();

public:
    DWORD Load();
    void Unload();

#ifdef _LINUX_
    DWORD GetMtpDevice(const char *szId, LIBMTP_mtpdevice_t *&pDevice);
    void ReleaseMtpDevice(LIBMTP_mtpdevice_t *pDevice);
#endif

    void OnChanged();

private:
    DWORD EnumDevices();
#ifdef _LINUX_
    void ProcessDevices(LIBMTP_raw_device_t *pDevices, int nCount);
#endif

#ifdef _LINUX_
    static bool IsEqual(LIBMTP_raw_device_t *pDevice1, LIBMTP_raw_device_t *pDevice2);
    static PMTPDEVICE_REC CreateRec(LIBMTP_raw_device_t *pDevice);
#endif
    static DWORD InitRec(PMTPDEVICE_REC pRec);
    static void FreeRec(PMTPDEVICE_REC pRec);

    DWORD MonThread();
    static void *MonThreadProc(void *arg);

private:
    pthread_mutex_t             _DeviceLock;
    list<PMTPDEVICE_REC>        _DeviceList;

    volatile bool               _bQuit;
    pthread_t                   _hMonThread;
    sem_t                       _hWakeEvent;
};

#endif

