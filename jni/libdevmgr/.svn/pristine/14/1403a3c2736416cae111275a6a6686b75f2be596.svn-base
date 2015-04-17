#include <StdAfx.h>
#include "TransCenter.h"
#include "MsgClient.h"


MsgClient::MsgClient()
    : TransStreamHandler(NULL)
    , _bQuit(false)
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    pthread_mutex_init(&_TransLock, NULL);

    SET_PTH_INVALID(_hThread);
    sem_init(&_hWakeEvent, 0, 0);
}

MsgClient::~MsgClient()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    sem_destroy(&_hWakeEvent);
    assert(!IS_PTH_VALID(_hThread));

    pthread_mutex_destroy(&_TransLock);
}

DWORD MsgClient::Load(PTRANS_ADDR pServerAddr)
{
    DWORD  dwRet;
    int  ret;

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    memcpy(&_ServerAddr, pServerAddr, sizeof(_ServerAddr));

    _biSend.Alloc(PDU_MAX_LENGTH + PDU_HEAD_RESERVE);

    Init(&_hWakeEvent);

    _bQuit = false;
    ret = pthread_create(&_hThread, NULL, ThreadProc, this);
    if ( ret )
    {
        dwRet = GetLastError();
        DBG(D_ERROR, (L"%s: create work thread failed: %d\n", FULL_FUNC_NAME, dwRet));
        SET_PTH_INVALID(_hThread);
        goto err1;
    }

    return ERROR_SUCCESS;

err1:
    Fini();

    return dwRet;
}

void MsgClient::Unload()
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    if ( IS_PTH_VALID(_hThread) )
    {
        _bQuit = true;
        sem_post(&_hWakeEvent);
        pthread_join(_hThread, NULL);
        SET_PTH_INVALID(_hThread);
    }

    Fini();
}

DWORD MsgClient::CmdUpdated()
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    return SendDbMsg(C_DBMSG_CMDTAB_UPDATED, "");
}

DWORD MsgClient::UpdateSysInfo(const char *szDevId)
{
    DBG(D_INFO, (L"%s: dev(%hs)\n", FULL_FUNC_NAME, szDevId));

    return SendDbMsg(C_DBMSG_UPDATE_SYSINFO, szDevId);
}

DWORD MsgClient::UpdateBattInfo(const char *szDevId)
{
    DBG(D_INFO, (L"%s: dev(%hs)\n", FULL_FUNC_NAME, szDevId));

    return SendDbMsg(C_DBMSG_UPDATE_BATTINFO, szDevId);
}

DWORD MsgClient::UpdateDiskInfo(const char *szDevId)
{
    DBG(D_INFO, (L"%s: dev(%hs)\n", FULL_FUNC_NAME, szDevId));

    return SendDbMsg(C_DBMSG_UPDATE_DISKINFO, szDevId);
}

DWORD MsgClient::UpdateAppList(const char *szDevId)
{
    DBG(D_INFO, (L"%s: dev(%hs)\n", FULL_FUNC_NAME, szDevId));

    return SendDbMsg(C_DBMSG_UPDATE_APPLIST, szDevId);
}

DWORD MsgClient::FindMe(const char *szDevId)
{
    DBG(D_INFO, (L"%s: dev(%hs)\n", FULL_FUNC_NAME, szDevId));

    return SendDbMsg(C_DBMSG_FIND_ME, szDevId);
}

void MsgClient::ProcessPDU(PPDU_COMM_HDR pHdr, PBYTE pBody)
{
    PPDU_DBMSG_BODY  pDbMsg;

    DBG(D_VERB, (L"%s: code(%d), err(%d), arg(%d), body(%d)\n", FULL_FUNC_NAME, pHdr->bCode, pHdr->bErr, pHdr->dwParam, pHdr->wBodySize));

    if ( pHdr->bCode == C_PDU_DBMSG )
    {
        assert(pHdr->wBodySize >= sizeof(PDU_DBMSG_BODY));
        pDbMsg = (PPDU_DBMSG_BODY)pBody;
        ProcessDbMsg((DBMSG_CODE)pHdr->dwParam, (char*)pDbMsg->szDevId[0]);
    }
}

DWORD MsgClient::SendDbMsg(DBMSG_CODE MsgCode, const char *szDevId)
{
    DWORD  dwRet;
    PPDU_DBMSG_BODY  pDbMsg;

    DBG(D_VERB, (L"%s: code(%d), dev(%hs)\n", FULL_FUNC_NAME, MsgCode, szDevId));

    pthread_mutex_lock(&_TransLock);

    if ( !_pTransInst )
    {
        pthread_mutex_unlock(&_TransLock);
        DBG(D_WARN, (L"%s: not connected\n", FULL_FUNC_NAME));
        return ERROR_NOT_CONNECTED;
    }

    _biSend.Reset(PDU_HEAD_RESERVE + sizeof(PDU_COMM_HDR));

    pDbMsg = (PPDU_DBMSG_BODY)_biSend.Put(sizeof(PDU_DBMSG_BODY));
    pDbMsg->dwDevNum = 1;
    strncpy((char*)pDbMsg->szDevId, szDevId, MAX_DEV_ID_LEN);

    dwRet = SendPDU((BYTE)C_PDU_DBMSG, true, false, (DWORD)MsgCode, &_biSend);

    pthread_mutex_unlock(&_TransLock);

    return dwRet;
}

DWORD MsgClient::WorkThread()
{
    DWORD  dwRet;
    TransInst  *pTrans;

    DBG(D_INFO, (L"%s: enter\n", FULL_FUNC_NAME));

    while ( !_bQuit )
    {
        dwRet = TransCenter::Instance()->ConnectOut(&_ServerAddr, &pTrans);
        if ( dwRet != ERROR_SUCCESS )
        {
            Sleep(1000);
            continue;
        }

        _pTransInst = pTrans;
        pTrans->Start(this);

        while ( !_bQuit )
        {
            sem_wait(&_hWakeEvent);
            if ( IsSessionEnd() )
            {
                break;
            }
        }

        pTrans->Stop();

        pthread_mutex_lock(&_TransLock);
        _pTransInst = NULL;
        pthread_mutex_unlock(&_TransLock);

        TransCenter::Instance()->FinishTransInst(pTrans);
    }

    DBG(D_INFO, (L"%s: leave\n", FULL_FUNC_NAME));

    return 0;
}

void *MsgClient::ThreadProc(void *arg)
{
    MsgClient  *pThis = static_cast<MsgClient*>(arg);

    pThis->WorkThread();

    return NULL;
}

