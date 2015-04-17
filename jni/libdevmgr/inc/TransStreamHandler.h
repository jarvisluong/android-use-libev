#ifndef _TRANS_STREAM_HANDLER_H
#define _TRANS_STREAM_HANDLER_H


#include "BufferItem.h"
#include "TransHandler.h"


class TransStreamHandler : public TransHandler
{
public:
    TransStreamHandler(TransInst *pTransInst);
    virtual ~TransStreamHandler();

    virtual DWORD Init(sem_t *pEndEvent);
    virtual void Fini();

    virtual void OnStarted();
    virtual void OnStopped();
    virtual void OnRecvReady();

protected:
    virtual void ProcessPDU(PPDU_COMM_HDR pHdr, PBYTE pBody) = 0;

    DWORD SendPDU(BYTE bCode, bool bReq, bool bErr, DWORD dwParam, BufferItem *pBodyItem);

private:
    BufferItem                  _biRecv;
    BufferItem                  _biRecvBuf;
};


#endif
