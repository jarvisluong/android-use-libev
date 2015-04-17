#include <StdAfx.h>
#include "MsgServer.h"


MsgServer *MsgServer::_instance = NULL;

MsgServer *MsgServer::Instance()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    if ( !_instance )
    {
        DBG(D_VERB, (L"%s: %ls\n", FULL_FUNC_NAME, L"CreateInstance"));
        _instance = new MsgServer();
    }
    return _instance;
}

void MsgServer::Destroy()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    delete _instance;
    _instance = NULL;
}

MsgServer::MsgServer()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));
}

MsgServer::~MsgServer()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));
}

DWORD MsgServer::Load(PTRANS_ADDR pListenAddr, TransHandler *pProtypeHandler)
{
    DWORD  dwRet;

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    dwRet = CommonServer::Load(pListenAddr, pProtypeHandler);

    return dwRet;
}

DWORD MsgServer::Unload()
{
    DWORD  dwRet;

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    dwRet = CommonServer::Unload();

    return dwRet;
}

