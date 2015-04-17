#ifndef _MSG_SERVER_HANDLER_H
#define _MSG_SERVER_HANDLER_H


#include "TransStreamHandler.h"
#include "DbTableObserver.h"


class MsgServerHandler : public TransStreamHandler, public DbTableObserver
{
public:
    MsgServerHandler(TransInst *pTransInst);
    virtual ~MsgServerHandler();

public:
    virtual DWORD Init(sem_t *pEndEvent);
    virtual void Fini();

    virtual TransHandler *Clone(TransInst *pTransInst);

    virtual void OnStarted();
    virtual void OnStopped();

    virtual void Notify(DBMSG_CODE MsgCode, const char *szDevId);

protected:
    virtual void ProcessPDU(PPDU_COMM_HDR pHdr, PBYTE pBody);
    virtual void ProcessDbMsg(DBMSG_CODE MsgCode, const char *szDevId);
};


#endif
