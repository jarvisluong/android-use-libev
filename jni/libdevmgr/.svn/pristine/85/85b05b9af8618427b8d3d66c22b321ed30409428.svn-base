#include <StdAfx.h>
#include "DbTableBase.h"


DbTableBase::DbTableBase()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    pthread_mutex_init(&_ObserverLock, NULL);
}

DbTableBase::~DbTableBase()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    assert(_ObserverList.empty());

    pthread_mutex_destroy(&_ObserverLock);
}

DWORD DbTableBase::RegisterObserver(DbTableObserver *pObserver)
{
    list<DbTableObserver*>::iterator  itr;

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    pthread_mutex_lock(&_ObserverLock);
    itr = find(_ObserverList.begin(), _ObserverList.end(), pObserver);
    if ( itr != _ObserverList.end() )
    {
        DBG(D_WARN, (L"%s: already exist\n", FULL_FUNC_NAME));
        assert(0);
        pthread_mutex_unlock(&_ObserverLock);
        return ERROR_ALREADY_EXISTS;
    }
    _ObserverList.push_back(pObserver);
    pthread_mutex_unlock(&_ObserverLock);

    return ERROR_SUCCESS;
}

DWORD DbTableBase::UnregisterObserver(DbTableObserver *pObserver)
{
    list<DbTableObserver*>::iterator  itr;

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    pthread_mutex_lock(&_ObserverLock);
    itr = find(_ObserverList.begin(), _ObserverList.end(), pObserver);
    if ( itr == _ObserverList.end() )
    {
        DBG(D_WARN, (L"%s: not found\n", FULL_FUNC_NAME));
        assert(0);
        pthread_mutex_unlock(&_ObserverLock);
        return ERROR_NOT_FOUND;
    }
    _ObserverList.erase(itr);
    pthread_mutex_unlock(&_ObserverLock);

    return ERROR_SUCCESS;
}

void DbTableBase::Notify(DBMSG_CODE MsgCode, const char *szDevId)
{
    list<DbTableObserver*>::iterator  itr;

    DBG(D_VERB, (L"%s: code(%d), dev(%hs)\n", FULL_FUNC_NAME, MsgCode, szDevId));

    pthread_mutex_lock(&_ObserverLock);
    for ( itr = _ObserverList.begin(); itr != _ObserverList.end(); itr++ )
    {
        (*itr)->Notify(MsgCode, szDevId);
    }
    pthread_mutex_unlock(&_ObserverLock);
}

