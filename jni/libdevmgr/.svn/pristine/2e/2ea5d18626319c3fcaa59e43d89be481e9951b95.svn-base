#include <StdAfx.h>
#include "TransSockInst.h"
#include "TransCenter.h"


TransCenter *TransCenter::_instance = NULL;

TransCenter *TransCenter::Instance()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    if ( !_instance )
    {
        DBG(D_VERB, (L"%s: %ls\n", FULL_FUNC_NAME, L"CreateInstance"));
        _instance = new TransCenter();
    }
    return _instance;
}

void TransCenter::Destroy()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    delete _instance;
    _instance = NULL;
}

TransCenter::TransCenter()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    pthread_mutex_init(&_TransLock, NULL);
}

TransCenter::~TransCenter()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    assert(_TransList.empty());

    pthread_mutex_destroy(&_TransLock);
}

DWORD TransCenter::Load()
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    return ERROR_SUCCESS;
}

DWORD TransCenter::Unload()
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    return ERROR_SUCCESS;
}

HANDLE TransCenter::InitListener(PTRANS_ADDR pAddr)
{
    DWORD  dwRet;
    SOCKET  s;

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    dwRet = CreateAndBind(pAddr, true, s);
    if ( dwRet != ERROR_SUCCESS )
    {
        DBG(D_WARN, (L"%s: CreateAndBindSocket failed: %d\n", FULL_FUNC_NAME, dwRet));
        return NULL;
    }

    return reinterpret_cast<HANDLE>(s);
}

DWORD TransCenter::WaitConnectIn(HANDLE hListener, TransInst **ppTrans, DWORD dwMilliseconds)
{
    DWORD  dwRet;
    SOCKET  s, s_new;

    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    s = reinterpret_cast<intptr_t>(hListener);

    if ( s == INVALID_SOCKET )
    {
        DBG(D_WARN, (L"%s: invalid socket\n", FULL_FUNC_NAME));
        return ERROR_INVALID_HANDLE;
    }

    dwRet = Accept(s, dwMilliseconds, s_new);
    if ( dwRet != ERROR_SUCCESS )
    {
        DBG(D_FUNC, (L"%s: AcceptNewConnect failed: %d\n", FULL_FUNC_NAME, dwRet));
        return dwRet;
    }

    dwRet = BindTransHandler(s_new, ppTrans);
    if ( dwRet != ERROR_SUCCESS )
    {
        DBG(D_WARN, (L"%s: BindTransHandler failed: %d\n", FULL_FUNC_NAME, dwRet));
        closesocket(s_new);
        return dwRet;
    }

    return ERROR_SUCCESS;
}

DWORD TransCenter::FinishListener(HANDLE hListener)
{
    SOCKET  s;

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    s = reinterpret_cast<intptr_t>(hListener);

    closesocket(s);

    return ERROR_SUCCESS;
}

DWORD TransCenter::ConnectOut(PTRANS_ADDR pAddr, TransInst **ppTrans)
{
    DWORD  dwRet;
    SOCKET  s;

    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    dwRet = CreateAndConnect(pAddr, s);
    if ( dwRet != ERROR_SUCCESS )
    {
        DBG(D_VERB, (L"%s: CreateAndConnect failed: %d\n", FULL_FUNC_NAME, dwRet));
        return dwRet;
    }

    dwRet = BindTransHandler(s, ppTrans);
    if ( dwRet != ERROR_SUCCESS )
    {
        DBG(D_WARN, (L"%s: BindTransHandler failed: %d\n", FULL_FUNC_NAME, dwRet));
        closesocket(s);
        return dwRet;
    }

    return ERROR_SUCCESS;
}

DWORD TransCenter::CreateTransInst(PTRANS_ADDR pAddr, TransInst **ppTrans)
{
    DWORD  dwRet;
    SOCKET  s;

    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    dwRet = CreateAndBind(pAddr, false, s);
    if ( dwRet != ERROR_SUCCESS )
    {
        DBG(D_WARN, (L"%s: CreateAndBindSocket failed: %d\n", FULL_FUNC_NAME, dwRet));
        return dwRet;
    }

    dwRet = BindTransHandler(s, ppTrans);
    if ( dwRet != ERROR_SUCCESS )
    {
        DBG(D_WARN, (L"%s: BindTransHandler failed: %d\n", FULL_FUNC_NAME, dwRet));
        closesocket(s);
        return dwRet;
    }

    (*ppTrans)->GetAddr(pAddr);

    return ERROR_SUCCESS;
}

DWORD TransCenter::FinishTransInst(TransInst *pTrans)
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    pthread_mutex_lock(&_TransLock);
    _TransList.remove(pTrans);
    pthread_mutex_unlock(&_TransLock);

    delete pTrans;

    return ERROR_SUCCESS;
}

DWORD TransCenter::CreateAndBind(PTRANS_ADDR pAddr, bool bListen, SOCKET &rs)
{
    DWORD  dwRet;
    struct sockaddr_in  addr;

    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    if ( pAddr->bType != ADDR_TYPE_IPV4 )
    {
        DBG(D_WARN, (L"%s: type(%d) not support\n", FULL_FUNC_NAME, pAddr->bType));
        return ERROR_NOT_SUPPORTED;
    }

    if ( pAddr->IPv4Addr.bStream )
    {
        rs = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    }
    else
    {
        rs = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    }

    if ( rs == INVALID_SOCKET )
    {
        dwRet = WSAGetLastError();
        DBG(D_WARN, (L"%s: create socket failed: %d\n", FULL_FUNC_NAME, dwRet));
        return dwRet;
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = pAddr->IPv4Addr.dwIP;
    addr.sin_port = pAddr->IPv4Addr.wPort;
    if ( bind(rs, (struct sockaddr*)&addr, sizeof(addr)) )
    {
        dwRet = WSAGetLastError();
        DBG(D_WARN, (L"%s: bind socket failed: %d\n", FULL_FUNC_NAME, dwRet));
        goto err;
    }

    if ( bListen && pAddr->IPv4Addr.bStream )
    {
        if ( listen(rs, 5) == SOCKET_ERROR )
        {
            dwRet = WSAGetLastError();
            DBG(D_WARN, (L"%s: listen failed: %d\n", FULL_FUNC_NAME,dwRet));
            goto err;
        }
    }

    return ERROR_SUCCESS;

err:
    closesocket(rs);

    return dwRet;
}

DWORD TransCenter::CreateAndConnect(PTRANS_ADDR pAddr, SOCKET &rs)
{
    DWORD  dwRet;
    struct sockaddr_in  addr;

    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    if ( pAddr->bType != ADDR_TYPE_IPV4 )
    {
        DBG(D_WARN, (L"%s: type(%d) not support\n", FULL_FUNC_NAME, pAddr->bType));
        return ERROR_NOT_SUPPORTED;
    }

    if ( pAddr->IPv4Addr.bStream )
    {
        rs = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    }
    else
    {
        rs = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    }

    if ( rs == INVALID_SOCKET )
    {
        dwRet = WSAGetLastError();
        DBG(D_WARN, (L"%s: create socket failed: %d\n", FULL_FUNC_NAME, dwRet));
        return dwRet;
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = pAddr->IPv4Addr.dwIP;
    addr.sin_port = pAddr->IPv4Addr.wPort;

    if ( connect(rs, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR )
    {
        dwRet = WSAGetLastError();
        DBG(D_VERB, (L"%s: connect failed: %d\n", FULL_FUNC_NAME, dwRet));
        closesocket(rs);
        return dwRet;
    }

    return ERROR_SUCCESS;
}

DWORD TransCenter::Accept(SOCKET s, DWORD dwMilliseconds, SOCKET &rs_new)
{
    DWORD  dwRet;
    int  ret;
    fd_set  rd_fds;
    timeval  tv;

    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    FD_ZERO(&rd_fds);
    FD_SET(s, &rd_fds);
    tv.tv_sec = dwMilliseconds / 1000;
    tv.tv_usec = 0;

    ret = select(s + 1, &rd_fds, NULL, NULL, &tv);

    if ( ret == 0 )
    {
        return WAIT_TIMEOUT;
    }

    if ( ret == SOCKET_ERROR )
    {
        dwRet = WSAGetLastError();
        DBG(D_ERROR, (L"%s: select failed: %d\n", FULL_FUNC_NAME, dwRet));
        return dwRet;
    }

    rs_new = accept(s, NULL, NULL);
    if ( rs_new == INVALID_SOCKET )
    {
        dwRet = WSAGetLastError();
        DBG(D_ERROR, (L"%s: accept failed: %d\n", FULL_FUNC_NAME, dwRet));
        return dwRet;
    }

    return ERROR_SUCCESS;
}

DWORD TransCenter::BindTransHandler(SOCKET s, TransInst **ppTrans)
{
    TransSockInst  *pSockTrans;

    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    pSockTrans = new TransSockInst(s);
    if ( !pSockTrans )
    {
        DBG(D_WARN, (L"%s: trans new failed\n", FULL_FUNC_NAME));
        return ERROR_OUTOFMEMORY;
    }

    pthread_mutex_lock(&_TransLock);
    _TransList.push_back(pSockTrans);
    pthread_mutex_unlock(&_TransLock);

    *ppTrans = pSockTrans;

    return ERROR_SUCCESS;
}

