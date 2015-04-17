#include <StdAfx.h>
#include "TransHandler.h"
#include "TransInst.h"


TransInst::TransInst()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));
}

TransInst::~TransInst()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));
}

DWORD TransInst::Recv(BufferItem *pBufItem, DWORD dwMilliseconds)
{
    DWORD  dwRet;

    DBG(D_FUNC, (L"%s: %d @ %p\n", FULL_FUNC_NAME, pBufItem->tail_size(), pBufItem->tail()));

    dwRet = PollRecv(dwMilliseconds);
    if ( dwRet == WAIT_FAILED )
    {
        DBG(D_INFO, (L"%s: PollRecv failed\n", FULL_FUNC_NAME));
        return dwRet;
    }

    if ( dwRet == WAIT_TIMEOUT )
    {
        return dwRet;
    }

    return Recv(pBufItem);
}

DWORD TransInst::Send(BufferItem *pBufItem, DWORD dwMilliseconds)
{
    DWORD  dwRet;

    DBG(D_FUNC, (L"%s: %d @ %p\n", FULL_FUNC_NAME, pBufItem->data_size(), pBufItem->data()));

    dwRet = PollSend(dwMilliseconds);
    if ( dwRet == WAIT_FAILED )
    {
        DBG(D_INFO, (L"%s: PollSend failed\n", FULL_FUNC_NAME));
        return dwRet;
    }

    if ( dwRet == WAIT_TIMEOUT )
    {
        return dwRet;
    }

    return Send(pBufItem);
}
