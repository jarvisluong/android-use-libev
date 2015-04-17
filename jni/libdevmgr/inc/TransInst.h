#ifndef _TRANS_INST_H
#define _TRANS_INST_H


#include "BufferItem.h"

class TransHandler;


class TransInst
{
public:
    TransInst();
    virtual ~TransInst();

    virtual DWORD Start(TransHandler *pTransHandler) = 0;
    virtual void Stop() = 0;

    virtual void Suspend() = 0;
    virtual void Resume() = 0;

    virtual DWORD SetRecvBufSize(DWORD  dwSize) = 0;

    virtual DWORD GetAddr(PTRANS_ADDR pAddr) = 0;
    virtual DWORD GetPeerAddr(PTRANS_ADDR pAddr) = 0;

    virtual DWORD PollRecv(DWORD dwMilliseconds) = 0;
    virtual DWORD PollSend(DWORD dwMilliseconds) = 0;

    virtual DWORD Recv(BufferItem *pBufItem) = 0;
    virtual DWORD Send(BufferItem *pBufItem) = 0;

    DWORD Recv(BufferItem *pBufItem, DWORD dwMilliseconds);
    DWORD Send(BufferItem *pBufItem, DWORD dwMilliseconds);
};


#endif
