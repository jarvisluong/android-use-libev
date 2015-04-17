#include <StdAfx.h>
#include "TransHandler.h"
#include "TransSockInst.h"


bool  TransSockInst::_bMuxQuit = false;
#if defined(_ANDROID_) || defined(_LINUX_)
pthread_t  TransSockInst::_hMuxThread = 0;
#else
pthread_t  TransSockInst::_hMuxThread = {NULL};
#endif
sem_t  TransSockInst::_hMuxEvent;
pthread_mutex_t  TransSockInst::_MuxLock;
list<TransSockInst*>  TransSockInst::_MuxList;
pthread_mutex_t  TransSockInst::_SocketLock;
list<SOCKET>  TransSockInst::_SocketList;

DWORD TransSockInst::Startup()
{
    DWORD  dwRet;
    int  ret;

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    pthread_mutex_init(&_MuxLock, NULL);
    pthread_mutex_init(&_SocketLock, NULL);

    sem_init(&_hMuxEvent, 0, 0);

    _bMuxQuit = false;
    ret = pthread_create(&_hMuxThread, NULL, MuxThread, NULL);
    if ( ret )
    {
        dwRet = GetLastError();
        DBG(D_ERROR, (L"%s: create mux thread failed: %d\n", FULL_FUNC_NAME, dwRet));
        SET_PTH_INVALID(_hMuxThread);
        return dwRet;
    }

    return ERROR_SUCCESS;
}

void TransSockInst::Cleanup()
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    if ( IS_PTH_VALID(_hMuxThread) )
    {
        _bMuxQuit = true;
        sem_post(&_hMuxEvent);
        pthread_join(_hMuxThread, NULL);
        SET_PTH_INVALID(_hMuxThread);
    }

    CleanupSocketList();

    sem_destroy(&_hMuxEvent);

    pthread_mutex_destroy(&_MuxLock);
    pthread_mutex_destroy(&_SocketLock);
}

void TransSockInst::Register(TransSockInst *pThis)
{
    bool  bEmpty;

    DBG(D_VERB, (L"%s: %d\n", FULL_FUNC_NAME, pThis->_s));

    assert(_MuxList.size() < 64);

    pthread_mutex_lock(&_MuxLock);
#ifndef NDEBUG
    list<TransSockInst*>::iterator  itr;
    itr = find(_MuxList.begin(), _MuxList.end(), pThis);
    assert(itr == _MuxList.end());
#endif
    bEmpty = _MuxList.empty();
    _MuxList.push_back(pThis);
    pthread_mutex_unlock(&_MuxLock);

    if ( bEmpty )
    {
        sem_post(&_hMuxEvent);
    }
}

void TransSockInst::Unregister(TransSockInst *pThis)
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    pthread_mutex_lock(&_MuxLock);
#ifndef NDEBUG
    list<TransSockInst*>::iterator  itr;
    itr = find(_MuxList.begin(), _MuxList.end(), pThis);
    assert(itr != _MuxList.end());
#endif
    _MuxList.remove(pThis);
    pthread_mutex_unlock(&_MuxLock);
}

void TransSockInst::CloseSocket(SOCKET s)
{
    DBG(D_VERB, (L"%s: %d\n", FULL_FUNC_NAME, s));

    pthread_mutex_lock(&_SocketLock);
    _SocketList.push_back(s);
    pthread_mutex_unlock(&_SocketLock);

    // because first call Unregister() then Close(), so _MuxList may empty,
    // then CleanupSocketList() will never be called until quit or new connection in
    sem_post(&_hMuxEvent);
}

void TransSockInst::CleanupSocketList()
{
    SOCKET  s;

    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    pthread_mutex_lock(&_SocketLock);
    while ( !_SocketList.empty() )
    {
        s = _SocketList.front();
        _SocketList.pop_front();

        DBG(D_VERB, (L"%s: close(%d)\n", FULL_FUNC_NAME, s));
        closesocket(s);
    }
    pthread_mutex_unlock(&_SocketLock);
}

void *TransSockInst::MuxThread(void *arg)
{
    int  ret;
    int  nfds;
    SOCKET  s;
    fd_set  rd_fds;
    timeval  tv;
    list<TransSockInst*>::iterator  itr;

    DBG(D_INFO, (L"%s: enter\n", FULL_FUNC_NAME));

    while ( !_bMuxQuit )
    {
        CleanupSocketList();

        pthread_mutex_lock(&_MuxLock);

        nfds = 0;
        FD_ZERO(&rd_fds);
        for ( itr = _MuxList.begin(); itr != _MuxList.end(); itr++ )
        {
            if ( (*itr)->_bIgnore )
            {
                continue;
            }

            s = (*itr)->_s;

            FD_SET(s, &rd_fds);
            if ( (int)s > nfds ) 
            {
                nfds = s;
            }
        }

        if ( !nfds )
        {
            pthread_mutex_unlock(&_MuxLock);
            sem_wait(&_hMuxEvent);
            continue;
        }

        pthread_mutex_unlock(&_MuxLock);

        tv.tv_sec = NET_RECV_TIMEOUT / 1000;
        tv.tv_usec = 0;
        ret = select(nfds + 1, &rd_fds, NULL, NULL, &tv);
        if ( ret <= 0 )
        {
            continue;
        }

        pthread_mutex_lock(&_MuxLock);
        for ( itr = _MuxList.begin(); itr != _MuxList.end(); itr++ )
        {
            if ( FD_ISSET((*itr)->_s, &rd_fds) )
            {
                assert((*itr)->_pTransHandler);
                (*itr)->_pTransHandler->OnRecvReady();
            }
        }
        pthread_mutex_unlock(&_MuxLock);
    }

    DBG(D_INFO, (L"%s: leave\n", FULL_FUNC_NAME));

    return 0;
}

TransSockInst::TransSockInst(SOCKET s)
    : _s(s)
    , _bIgnore(false)
    , _pTransHandler(NULL)
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));
}

TransSockInst::~TransSockInst()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    // DON'T close socket here, because when select@MuxThread return,
    // the socket in rd_fds maybe closed by thrad A and reopened by thread B.
    // They will have the same socket number, so thread B do Recv@OnRecvReady£¬
    // no data arrived, so MuxThread will stuck.
    CloseSocket(_s);  // closesocket(_s);
    _s = INVALID_SOCKET;
}

DWORD TransSockInst::Start(TransHandler *pTransHandler)
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    assert(_s != INVALID_SOCKET);

    // SO_NOSIGPIPE not defined, use send(xxx, MSG_NOSIGNAL)
#if 0//defined(_LINUX_) || defined(_ANDROID_)
{
    DWORD  dwRet;
    int  val = 1;
    if ( setsockopt(_s, SOL_SOCKET, SO_NOSIGPIPE, (char*)&val, sizeof(val)) == SOCKET_ERROR )
    {
        dwRet = WSAGetLastError();
        DBG(D_WARN, (L"%s: setsockopt failed: %d\n", FULL_FUNC_NAME, dwRet));
    }
}
#endif

    assert(!_pTransHandler);
    _pTransHandler = pTransHandler;
    _pTransHandler->OnStarted();

    _bIgnore = false;
    Register(this);

    return ERROR_SUCCESS;
}

void TransSockInst::Stop()
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    assert(_s != INVALID_SOCKET);

    Unregister(this);

    _pTransHandler->OnStopped();
    _pTransHandler = NULL;
}

void TransSockInst::Suspend()
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    _bIgnore = true;
}

void TransSockInst::Resume()
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    _bIgnore = false;
}

DWORD TransSockInst::SetRecvBufSize(DWORD  dwSize)
{
    DWORD  dwRet;
    int  size;

    DBG(D_VERB, (L"%s: %d\n", FULL_FUNC_NAME, dwSize));

    size = (int)dwSize;
    if ( setsockopt(_s, SOL_SOCKET, SO_RCVBUF, (char*)&size, sizeof(size)) == SOCKET_ERROR )
    {
        dwRet = WSAGetLastError();
        DBG(D_WARN, (L"%s: setsockopt failed: %d\n", FULL_FUNC_NAME, dwRet));
        return dwRet;
    }

    return ERROR_SUCCESS;
}

DWORD TransSockInst::GetAddr(PTRANS_ADDR pAddr)
{
    struct sockaddr_in  addr;
    socklen_t  addrlen;

    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    addrlen = sizeof(addr);
    getsockname(_s, (struct sockaddr*)&addr, &addrlen);

    pAddr->bType = ADDR_TYPE_IPV4;
    pAddr->IPv4Addr.dwIP = addr.sin_addr.s_addr;
    pAddr->IPv4Addr.wPort = addr.sin_port;

    return ERROR_SUCCESS;
}

DWORD TransSockInst::GetPeerAddr(PTRANS_ADDR pAddr)
{
    struct sockaddr_in  addr;
    socklen_t  addrlen;

    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    addrlen = sizeof(addr);
    getpeername(_s, (struct sockaddr*)&addr, &addrlen);

    pAddr->bType = ADDR_TYPE_IPV4;
    pAddr->IPv4Addr.dwIP = addr.sin_addr.s_addr;
    pAddr->IPv4Addr.wPort = addr.sin_port;

    return ERROR_SUCCESS;
}

DWORD TransSockInst::PollRecv(DWORD dwMilliseconds)
{
    DWORD  dwRet;
    int  ret;
    fd_set  rd_fds;
    timeval  tv;

    DBG(D_FUNC, (L"%s: %d\n", FULL_FUNC_NAME, dwMilliseconds));

    FD_ZERO(&rd_fds);
    FD_SET(_s, &rd_fds);
    tv.tv_sec = dwMilliseconds / 1000;
    tv.tv_usec = 0;
    ret = select(_s + 1, &rd_fds, NULL, NULL, &tv);
    if ( ret < 0 )
    {
        dwRet = WSAGetLastError();
        DBG(D_WARN, (L"%s: select failed: %d\n", FULL_FUNC_NAME, dwRet));
        return dwRet;
    }

    if ( ret == 0 )
    {
        return WAIT_TIMEOUT;
    }

    return ERROR_SUCCESS;
}

DWORD TransSockInst::PollSend(DWORD dwMilliseconds)
{
    DWORD  dwRet;
    int  ret;
    fd_set  wr_fds;
    timeval  tv;

    DBG(D_FUNC, (L"%s: %d\n", FULL_FUNC_NAME, dwMilliseconds));

    FD_ZERO(&wr_fds);
    FD_SET(_s, &wr_fds);
    tv.tv_sec = dwMilliseconds / 1000;
    tv.tv_usec = 0;
    ret = select(_s + 1, NULL, &wr_fds, NULL, &tv);
    if ( ret < 0 )
    {
        dwRet = WSAGetLastError();
        DBG(D_WARN, (L"%s: select failed: %d\n", FULL_FUNC_NAME, dwRet));
        return dwRet;
    }

    if ( ret == 0 )
    {
        return WAIT_TIMEOUT;
    }

    return ERROR_SUCCESS;
}

DWORD TransSockInst::Recv(BufferItem *pBufItem)
{
    DWORD  dwRet;
    int  len;

    DBG(D_FUNC, (L"%s: %d @ %p\n", FULL_FUNC_NAME, pBufItem->tail_size(), pBufItem->tail()));

    len = recv(_s, (char*)pBufItem->tail(), pBufItem->tail_size(), 0);
    if ( len <= 0 )
    {
        dwRet = (len < 0) ? WSAGetLastError() : ERROR_NOT_CONNECTED;
        DBG(D_WARN, (L"%s: recv failed: %d\n", FULL_FUNC_NAME, dwRet));
        return dwRet;
    }

    pBufItem->Put(len);

    return ERROR_SUCCESS;
}

DWORD TransSockInst::Send(BufferItem *pBufItem)
{
    DWORD  dwRet;
    int  len;

    DBG(D_FUNC, (L"%s: %d @ %p\n", FULL_FUNC_NAME, pBufItem->data_size(), pBufItem->data()));

    while ( pBufItem->data_size() )
    {
        len = send(_s, (char*)pBufItem->data(), pBufItem->data_size(), MSG_NOSIGNAL);
        if ( len <= 0 )
        {
            dwRet = WSAGetLastError();
            DBG(D_WARN, (L"%s: send failed: %d\n", FULL_FUNC_NAME, dwRet));
            return dwRet;
        }

        pBufItem->Pull(len);
    }

    return ERROR_SUCCESS;
}
