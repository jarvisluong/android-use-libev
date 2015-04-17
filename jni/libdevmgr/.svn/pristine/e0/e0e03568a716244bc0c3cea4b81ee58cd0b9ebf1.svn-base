#ifndef _TRANS_CENTER_H
#define _TRANS_CENTER_H


#include "TransInst.h"


class TransCenter
{
public:
    static TransCenter *Instance();
    static void Destroy();

private:
    static TransCenter        * _instance;

protected:
    TransCenter();
    ~TransCenter();

public:
    DWORD Load();
    DWORD Unload();

    HANDLE InitListener(PTRANS_ADDR pAddr);
    DWORD WaitConnectIn(HANDLE hListener, TransInst **ppTrans, DWORD dwMilliseconds);
    DWORD FinishListener(HANDLE hListener);

    DWORD ConnectOut(PTRANS_ADDR pAddr, TransInst **ppTrans);

    DWORD CreateTransInst(PTRANS_ADDR pAddr, TransInst **ppTrans);

    DWORD FinishTransInst(TransInst *pTrans);

private:
    DWORD CreateAndBind(PTRANS_ADDR pAddr, bool bListen, SOCKET &rs);
    DWORD Accept(SOCKET s, DWORD dwMilliseconds, SOCKET &rs_new);
    DWORD CreateAndConnect(PTRANS_ADDR pAddr, SOCKET &rs);
    DWORD BindTransHandler(SOCKET s, TransInst **ppTrans);

private:
    pthread_mutex_t             _TransLock;
    list<TransInst*>            _TransList;
};


#endif
