#include <StdAfx.h>
#include "TransInst.h"
#include "TransStreamHandler.h"


TransStreamHandler::TransStreamHandler(TransInst *pTransInst)
    : TransHandler(pTransInst)
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));
}

TransStreamHandler::~TransStreamHandler()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));
}

DWORD TransStreamHandler::Init(sem_t *pEndEvent)
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    TransHandler::Init(pEndEvent);

    _biRecv.Alloc(PDU_MAX_LENGTH + PDU_HEAD_RESERVE);
    _biRecvBuf.Alloc(PDU_MAX_LENGTH * 2 + PDU_HEAD_RESERVE);

    return ERROR_SUCCESS;
}

void TransStreamHandler::Fini()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    TransHandler::Fini();
}

void TransStreamHandler::OnStarted()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    _biRecv.Reset(PDU_HEAD_RESERVE);
}

void TransStreamHandler::OnStopped()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));
}

void TransStreamHandler::OnRecvReady()
{
    DWORD  dwRet;
    UINT  nBytes, nSize;
    WORD  wBodySize;
    PPDU_COMM_HDR  pHdr;
    PBYTE  pBody;

    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    _biRecvBuf.Reset();
#ifdef _DEBUG
    dwRet = _pTransInst->Recv(&_biRecvBuf, 0);
    assert(dwRet != WAIT_TIMEOUT);
#else
    dwRet = _pTransInst->Recv(&_biRecvBuf);
#endif
    if ( dwRet != ERROR_SUCCESS )
    {
        DBG(D_INFO, (L"%s: recv failed: %d\n", FULL_FUNC_NAME, dwRet));
        EndSession();
        return;
    }

    while ( true )
    {
        nSize = _biRecv.data_size();
        if ( nSize < sizeof(PDU_COMM_HDR) )
        {
            if ( !_biRecvBuf.data_size() )
            {
                break;
            }

            nBytes = min((UINT)(sizeof(PDU_COMM_HDR) - nSize), _biRecvBuf.data_size());
            memcpy(_biRecv.Put(nBytes), _biRecvBuf.data(), nBytes);
            _biRecvBuf.Pull(nBytes);
            continue;
        }

        pHdr = (PPDU_COMM_HDR)_biRecv.data();
        if ( pHdr->dwMagic != htonl(PDU_MAGIC_NUM) )
        {
            DBG(D_WARN, (L"%s: invalid magic: %x\n", FULL_FUNC_NAME, ntohl(pHdr->dwMagic)));
            EndSession();
            break;
        }

        nSize = _biRecv.data_size() - sizeof(PDU_COMM_HDR);
        wBodySize = ntohs(pHdr->wBodySize);
        if ( nSize < wBodySize )
        {
            if ( !_biRecvBuf.data_size() )
            {
                break;
            }

            nBytes = min(wBodySize - nSize, _biRecvBuf.data_size());
            memcpy(_biRecv.Put(nBytes), _biRecvBuf.data(), nBytes);
            _biRecvBuf.Pull(nBytes);
            continue;
        }


        pHdr->wBodySize = ntohs(pHdr->wBodySize);
        pHdr->dwParam = ntohl(pHdr->dwParam);
        pBody = _biRecv.Pull(sizeof(PDU_COMM_HDR));

        ProcessPDU(pHdr, pBody);

        _biRecv.Reset(PDU_HEAD_RESERVE);
    }
}

DWORD TransStreamHandler::SendPDU(BYTE bCode, bool bReq, bool bErr, DWORD dwParam, BufferItem *pBodyItem)
{
    PPDU_COMM_HDR  pHdr;

    DBG(D_VERB, (L"%s: code(%d), req(%d), err(%d), arg(%x), body(%d)\n", FULL_FUNC_NAME, bCode, bReq, bErr, dwParam, pBodyItem->data_size()));

    pHdr = (PPDU_COMM_HDR)pBodyItem->Push(sizeof(PDU_COMM_HDR));
    pHdr->dwMagic = htonl(PDU_MAGIC_NUM);
    pHdr->wBodySize = htons(pBodyItem->data_size() - sizeof(PDU_COMM_HDR));
    pHdr->bCode = bCode;
    pHdr->bReq = bReq;
    pHdr->bErr = bErr;
    pHdr->dwParam = htonl(dwParam);

    assert(_pTransInst);
    return _pTransInst->Send(pBodyItem, NET_SEND_TIMEOUT);
}

