#include <StdAfx.h>
#include "TransInst.h"
#include "TransHandler.h"


TransHandler::TransHandler(TransInst *pTransInst)
    : _pTransInst(pTransInst)
    , _bSessionEnd(false)
    , _pEndEvent(NULL)
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));
}

TransHandler::~TransHandler()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));
}

DWORD TransHandler::Init(sem_t *pEndEvent)
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    _pEndEvent = pEndEvent;

    return ERROR_SUCCESS;
}

void TransHandler::Fini()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    _pEndEvent = NULL;
}

TransHandler *TransHandler::Clone(TransInst *pTransInst)
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    return NULL;
}

bool TransHandler::IsSessionEnd()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    return _bSessionEnd;
}

void TransHandler::EndSession()
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    _bSessionEnd = true;

    assert(_pTransInst);
    _pTransInst->Suspend();

    assert(_pEndEvent);
    sem_post(_pEndEvent);
}

