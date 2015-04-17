#include <StdAfx.h>
#include "RingBuffer.h"


RingBuffer::RingBuffer(const wchar_t *lpName, int nSize)
    : _name(lpName)
{
    DBG(D_FUNC, (L"%s: name(%ls), size(%d)\n", FULL_FUNC_NAME, lpName, nSize));

    _nPushPos = 0;
    _nPopPos = 0;
    _nSize = nSize + 1;
    _pRingBuf = new PBYTE[_nSize];
#ifndef NDEBUG
    memset(_pRingBuf, 0, sizeof(PBYTE) * _nSize);
#endif
}

RingBuffer::~RingBuffer()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    delete []_pRingBuf;
}

void RingBuffer::Reset()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    _nPushPos = 0;
    _nPopPos = 0;
#ifndef NDEBUG
    for ( int i = 0; i < _nSize; i++ )
    {
        _pRingBuf[i] = NULL;
    }
#endif
}

bool RingBuffer::IsEmpty()
{
    DBG(D_FUNC, (L"%s: %ls: push(%d), pop(%d), slot(%d)\n", FULL_FUNC_NAME, _name.c_str(), _nPushPos, _nPopPos, _nSize));

    return (_nPushPos == _nPopPos);
}

bool RingBuffer::IsFull()
{
    DBG(D_FUNC, (L"%s: %ls: push(%d), pop(%d), slot(%d)\n", FULL_FUNC_NAME, _name.c_str(), _nPushPos, _nPopPos, _nSize));

    return (((_nPushPos + 1) % _nSize) == _nPopPos);
}

void RingBuffer::PushAtBack(PBYTE pItem)
{
    DBG(D_FUNC, (L"%s: %ls: push(%d), pop(%d), slot(%d)\n", FULL_FUNC_NAME, _name.c_str(), _nPushPos, _nPopPos, _nSize));

    assert(!IsFull());

#ifndef NDEBUG
    assert(pItem);
    assert(!_pRingBuf[_nPushPos]);
#endif
    _pRingBuf[_nPushPos] = pItem;
    _nPushPos = (_nPushPos + 1) % _nSize;
}

PBYTE RingBuffer::PopAtFront()
{
    PBYTE  pItem;

    DBG(D_FUNC, (L"%s: %ls: push(%d), pop(%d), slot(%d)\n", FULL_FUNC_NAME, _name.c_str(), _nPushPos, _nPopPos, _nSize));

    assert(!IsEmpty());

    pItem = _pRingBuf[_nPopPos];
#ifndef NDEBUG
    assert(pItem);
    _pRingBuf[_nPopPos] = NULL;
#endif
    _nPopPos = (_nPopPos + 1) % _nSize;

    return pItem;
}

bool RingBuffer::PushAtBackSafe(PBYTE pItem)
{
    DBG(D_FUNC, (L"%s: %ls: push(%d), pop(%d), slot(%d)\n", FULL_FUNC_NAME, _name.c_str(), _nPushPos, _nPopPos, _nSize));

    if ( IsFull() )
    {
        return false;
    }

    PushAtBack(pItem);

    return true;
}

PBYTE RingBuffer::PopAtFrontSafe()
{
    DBG(D_FUNC, (L"%s: %ls: push(%d), pop(%d), slot(%d)\n", FULL_FUNC_NAME, _name.c_str(), _nPushPos, _nPopPos, _nSize));

    if ( IsEmpty() )
    {
        return NULL;
    }

    return PopAtFront();
}
