#ifndef _COMMON_SERVER_H
#define _COMMON_SERVER_H


#include "TransInst.h"
#include "TransHandler.h"


typedef struct _CLIENT_REC
{
    TransInst                 * pTrans;
    TransHandler              * pHandler;
    bool                        bReqDel;
    void                      * hPrivate;
}
CLIENT_REC, *PCLIENT_REC;


class CommonServer
{
public:
    CommonServer();
    virtual ~CommonServer();

public:
    virtual DWORD Load(PTRANS_ADDR pListenAddr, TransHandler *pProtypeHandler);
    virtual DWORD Unload();

protected:
    void CreateClientRec(TransInst *pTrans, TransHandler *pHandler);

    virtual void InitClientRec(PCLIENT_REC pRec);
    virtual void FreeClientRec(PCLIENT_REC pRec);

    PCLIENT_REC FindClientRec(TransHandler *pHandler);

private:
    DWORD AcceptThread();
    static void *AcceptThreadProc(void *arg);

    DWORD CleanupThread();
    static void *CleanupThreadProc(void *arg);

protected:
    TRANS_ADDR                  _ListenAddr;
    TransHandler              * _pProtypeHandler;

    pthread_mutex_t             _ClientLock;
    list<PCLIENT_REC>           _ClientList;

    sem_t                       _hEndEvent;

private:
    volatile bool               _bQuit;
    pthread_t                   _hAcceptThread;
    pthread_t                   _hCleanupThread;
};


#endif
