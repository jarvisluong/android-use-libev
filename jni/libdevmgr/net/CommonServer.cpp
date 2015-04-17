#include <StdAfx.h>
#include "TransCenter.h"
#include "CommonServer.h"


CommonServer::CommonServer()
    : _pProtypeHandler(NULL)
    , _bQuit(false)
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    _ListenAddr.bType = ADDR_TYPE_UNKNOWN;

    pthread_mutex_init(&_ClientLock, NULL);

    SET_PTH_INVALID(_hAcceptThread);
    SET_PTH_INVALID(_hCleanupThread);

    sem_init(&_hEndEvent, 0, 0);
}

CommonServer::~CommonServer()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    sem_destroy(&_hEndEvent);

    pthread_mutex_destroy(&_ClientLock);
}

DWORD CommonServer::Load(PTRANS_ADDR pListenAddr, TransHandler *pProtypeHandler)
{
    DWORD  dwRet;
    int  ret;

    DBG(D_LOG, (L"%s\n", FULL_FUNC_NAME));

    memcpy(&_ListenAddr, pListenAddr, sizeof(_ListenAddr));

    _pProtypeHandler = pProtypeHandler;

    try
    {
        _bQuit = false;

        ret = pthread_create(&_hCleanupThread, NULL, CleanupThreadProc, this);
        if ( ret )
        {
            dwRet = GetLastError();
            DBG(D_ERROR, (L"%s: create cleanup thread failed: %d\n", FULL_FUNC_NAME, dwRet));
            SET_PTH_INVALID(_hCleanupThread);
            throw dwRet;
        }

        ret = pthread_create(&_hAcceptThread, NULL, AcceptThreadProc, this);
        if ( ret )
        {
            dwRet = GetLastError();
            DBG(D_ERROR, (L"%s: create listen thread failed: %d\n", FULL_FUNC_NAME, dwRet));
            SET_PTH_INVALID(_hAcceptThread);
            throw dwRet;
        }

        dwRet = ERROR_SUCCESS;
    }
    catch(...)
    {
        DBG(D_WARN, (L"%s exception\n", FULL_FUNC_NAME));
        Unload();
    }

    return dwRet;
}

DWORD CommonServer::Unload()
{
    PCLIENT_REC  pRec;

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    _bQuit = true;

    if ( IS_PTH_VALID(_hAcceptThread) )
    {
        pthread_join(_hAcceptThread, NULL);
        SET_PTH_INVALID(_hAcceptThread);
    }

    if ( IS_PTH_VALID(_hCleanupThread) )
    {
        sem_post(&_hEndEvent);
        pthread_join(_hCleanupThread, NULL);
        SET_PTH_INVALID(_hCleanupThread);
    }

    pthread_mutex_lock(&_ClientLock);
    while ( !_ClientList.empty() )
    {
        pRec = _ClientList.front();
        _ClientList.pop_front();

        pthread_mutex_unlock(&_ClientLock);
        FreeClientRec(pRec);
        pthread_mutex_lock(&_ClientLock);
    }
    pthread_mutex_unlock(&_ClientLock);

    return ERROR_SUCCESS;
}

void CommonServer::CreateClientRec(TransInst *pTrans, TransHandler *pHandler)
{
    PCLIENT_REC pRec;

    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    try
    {
        pRec = new CLIENT_REC;
        memset(pRec, 0, sizeof(CLIENT_REC));

        pRec->pTrans = pTrans;
        pRec->pHandler = pHandler;
    }
    catch(...)
    {
        pRec = NULL;
    }

    if ( !pRec )
    {
        DBG(D_WARN, (L"%s: drop connection\n", FULL_FUNC_NAME));

        pTrans->Stop();
        pHandler->Fini();
        delete pHandler;
        TransCenter::Instance()->FinishTransInst(pTrans);

        return;
    }

    InitClientRec(pRec);

    pthread_mutex_lock(&_ClientLock);
    _ClientList.push_back(pRec);
    pthread_mutex_unlock(&_ClientLock);

    pHandler->Init(&_hEndEvent);
    pTrans->Start(pHandler);
}

void CommonServer::InitClientRec(PCLIENT_REC pRec)
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));
}

void CommonServer::FreeClientRec(PCLIENT_REC pRec)
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    pRec->pTrans->Stop();
    pRec->pHandler->Fini();
    delete pRec->pHandler;
    TransCenter::Instance()->FinishTransInst(pRec->pTrans);

    delete pRec;
}

PCLIENT_REC CommonServer::FindClientRec(TransHandler *pHandler)
{
    PCLIENT_REC  pRec;
    list<PCLIENT_REC>::iterator  itr;

    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    pRec = NULL;
    for ( itr = _ClientList.begin(); itr != _ClientList.end(); itr++ )
    {
       if ( (*itr)->pHandler == pHandler )
       {
           pRec = (*itr);
           break;
       }
    }

    return pRec;
}

DWORD CommonServer::AcceptThread()
{
    DWORD  dwRet;
    HANDLE  hListener;
    TransInst  *pTrans;
    TransHandler  *pHandler;

    DBG(D_INFO, (L"%s: enter\n", FULL_FUNC_NAME));

    hListener = NULL;

    while ( !_bQuit )
    {
        while ( !_bQuit )
        {
            hListener = TransCenter::Instance()->InitListener(&_ListenAddr);
            if ( hListener )
            {
                break;
            }
            Sleep(5000);
        }

        while ( !_bQuit )
        {
            dwRet = ERROR_OPERATION_ABORTED;
            while ( !_bQuit )
            {
                dwRet = TransCenter::Instance()->WaitConnectIn(hListener, &pTrans, NET_RECV_TIMEOUT);
                if ( dwRet == ERROR_SUCCESS )
                {
                    break;
                }
            }

            if ( dwRet == ERROR_SUCCESS )
            {
                pHandler = _pProtypeHandler->Clone(pTrans);
                CreateClientRec(pTrans, pHandler);
            }
        }

        if ( hListener )
        {
            TransCenter::Instance()->FinishListener(hListener);
        }
    }

    DBG(D_INFO, (L"%s: leave\n", FULL_FUNC_NAME));

    return 0;
}

void *CommonServer::AcceptThreadProc(void *arg)
{
    CommonServer  *pThis = static_cast<CommonServer*>(arg);

    pThis->AcceptThread();

    return NULL;
}

DWORD CommonServer::CleanupThread()
{
    CLIENT_REC  *pRec;
    list<PCLIENT_REC>::iterator  itr;
    list<PCLIENT_REC>  EndList;

    DBG(D_INFO, (L"%s: enter\n", FULL_FUNC_NAME));

    while ( true )
    {
        sem_wait(&_hEndEvent);

        if ( _bQuit )
        {
            break;
        }

        pthread_mutex_lock(&_ClientLock);
        for ( itr = _ClientList.begin(); itr != _ClientList.end(); )
        {
           if ( (*itr)->bReqDel || (*itr)->pHandler->IsSessionEnd() )
           {
               EndList.push_back(*itr);
               itr = _ClientList.erase(itr);
           }
           else
           {
               itr++;
           }
        }
        pthread_mutex_unlock(&_ClientLock);

        // must guarantee no one else will remove item from _ClientList except CleanThread
        // so items in EndList will always be valid 
        while ( !EndList.empty() )
        {
            pRec = EndList.front();
            EndList.pop_front();

            // FreeClientRec may block in Unregister@TransSockInst::Fini, when Mux call OnRecvReady()
            // OnRecvReady may call OnClientXXX, so dead lock
            FreeClientRec(pRec);
        }
    }

    DBG(D_INFO, (L"%s: leave\n", FULL_FUNC_NAME));

    return 0;
}

void *CommonServer::CleanupThreadProc(void *arg)
{
    CommonServer  *pThis = static_cast<CommonServer*>(arg);

    pThis->CleanupThread();

    return NULL;
}

