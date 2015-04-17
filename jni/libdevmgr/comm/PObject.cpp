#include <StdAfx.h>
#include "PObject.h"


PObject::PObject()
    : _lRefCnt(1)
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    pthread_mutex_init(&_RefLock, NULL);
}

PObject::~PObject()
{
    assert(_lRefCnt == 0);

    pthread_mutex_destroy(&_RefLock);
}

LONG PObject::AddRef()
{
#ifdef _WIN32_
    return InterlockedIncrement(&_lRefCnt);
#else
    LONG  lRef;

    pthread_mutex_lock(&_RefLock);
    lRef = ++_lRefCnt;
    pthread_mutex_unlock(&_RefLock);

    return lRef;
#endif
}

LONG PObject::Release()
{
    LONG  lRef;

#ifdef _WIN32_
    lRef = InterlockedDecrement(&_lRefCnt);
#else
    pthread_mutex_lock(&_RefLock);
    lRef = --_lRefCnt;
    pthread_mutex_unlock(&_RefLock);
#endif

    if ( !lRef )
    {
        delete this;
    }

    return lRef;
}

