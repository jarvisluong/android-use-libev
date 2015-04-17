#ifndef _MSG_SERVER_H
#define _MSG_SERVER_H


#include "CommonServer.h"


class MsgServer : public CommonServer
{
public:
    static MsgServer *Instance();
    static void Destroy();

private:
    static MsgServer          * _instance;

protected:
    MsgServer();
    virtual ~MsgServer();

public:
    virtual DWORD Load(PTRANS_ADDR pListenAddr, TransHandler *pProtypeHandler);
    virtual DWORD Unload();
};


#endif
