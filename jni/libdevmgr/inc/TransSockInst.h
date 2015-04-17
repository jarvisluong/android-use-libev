#ifndef _TRANS_SOCK_INST_H
#define _TRANS_SOCK_INST_H


#include "TransInst.h"


class TransSockInst : public TransInst
{
public:
    static DWORD Startup();
    static void Cleanup();

private:
    static void Register(TransSockInst *pThis);
    static void Unregister(TransSockInst *pThis);

    static void CloseSocket(SOCKET s);
    static void CleanupSocketList();

    static void *MuxThread(void *arg);

private:
    static bool                 _bMuxQuit;
    static pthread_t            _hMuxThread;
    static sem_t                _hMuxEvent;
    static pthread_mutex_t      _MuxLock;
    static list<TransSockInst*> _MuxList;

    static pthread_mutex_t      _SocketLock;
    static list<SOCKET>         _SocketList;

public:
    TransSockInst(SOCKET s);
    virtual ~TransSockInst();

    virtual DWORD Start(TransHandler *pTransHandler);
    virtual void Stop();

    virtual void Suspend();
    virtual void Resume();

    virtual DWORD SetRecvBufSize(DWORD  dwSize);

    virtual DWORD GetAddr(PTRANS_ADDR pAddr);
    virtual DWORD GetPeerAddr(PTRANS_ADDR pAddr);

    virtual DWORD PollRecv(DWORD dwMilliseconds);
    virtual DWORD PollSend(DWORD dwMilliseconds);

    virtual DWORD Recv(BufferItem *pBufItem);
    virtual DWORD Send(BufferItem *pBufItem);

private:
    SOCKET                      _s;
    bool                        _bIgnore;

    TransHandler              * _pTransHandler;
};


#endif
