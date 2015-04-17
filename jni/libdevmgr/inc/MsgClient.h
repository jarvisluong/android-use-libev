#ifndef _MSG_CLIENT_H
#define _MSG_CLIENT_H


#include "TransStreamHandler.h"


class MsgClient : public TransStreamHandler
{
public:
    MsgClient();
    virtual ~MsgClient();

public:
    DWORD Load(PTRANS_ADDR pServerAddr);
    void Unload();

    DWORD CmdUpdated();
    DWORD UpdateSysInfo(const char *szDevId);
    DWORD UpdateBattInfo(const char *szDevId);
    DWORD UpdateDiskInfo(const char *szDevId);
    DWORD UpdateAppList(const char *szDevId);
    DWORD FindMe(const char *szDevId);

protected:
    virtual void ProcessPDU(PPDU_COMM_HDR pHdr, PBYTE pBody);

    DWORD SendDbMsg(DBMSG_CODE MsgCode, const char *szDevId);
    virtual void ProcessDbMsg(DBMSG_CODE MsgCode, const char *szDevId) = 0;

private:
    DWORD WorkThread();
    static void *ThreadProc(void *arg);

private:
    TRANS_ADDR                  _ServerAddr;

    pthread_mutex_t             _TransLock;
    BufferItem                  _biSend;

    volatile bool               _bQuit;
    pthread_t                   _hThread;
    sem_t                       _hWakeEvent;
};


#endif
