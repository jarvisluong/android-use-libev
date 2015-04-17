#include <StdAfx.h>
#include "BufferItem.h"


BufferItem::BufferItem()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    _head = _data = _tail = _end = NULL;
    _data_len = 0;
}

BufferItem::~BufferItem()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    delete []_head;
}

void BufferItem::Alloc(UINT size)
{
    DBG(D_FUNC, (L"%s: %d\n", FULL_FUNC_NAME, size));

    if ( room() < size)
    {
        DBG(D_PERF, (L"%s: %d -> %d\n", FULL_FUNC_NAME, room(), size));
        delete []_head;
        _head = new BYTE[size];
        _end = _head + size;
    }

    Reset();
}

void BufferItem::Expand(UINT len)
{
    PBYTE  p;
    UINT  size;

    size = room() + len;
    DBG(D_PERF, (L"%s: %d -> %d\n", FULL_FUNC_NAME, room(), size));

    p = new BYTE[size];

    memcpy(p, _head, room());

    _data = p + head_size();
    _tail = _data + _data_len;
    _end = p + size;

    delete []_head;
    _head = p;
}

void BufferItem::Reset(UINT len)
{
    DBG(D_FUNC, (L"%s: %d\n", FULL_FUNC_NAME, len));

    assert((_head+len)<=_end);

    _data = _head + len;
    _tail = _data;
    _data_len = 0;
}

void BufferItem::Reserve(UINT len)
{
    DBG(D_FUNC, (L"%s: %d\n", FULL_FUNC_NAME, len));

    assert(!_data_len);

    _data += len;
    _tail = _data;
}

PBYTE BufferItem::Put(UINT len)
{
    PBYTE  p;

    DBG(D_FUNC, (L"%s: %d\n", FULL_FUNC_NAME, len));

    assert(_head);

    p = _tail;
    _tail += len;
    _data_len += len;

    assert(_tail <= _end);

    return p;
}

PBYTE BufferItem::Push(UINT len)
{
    DBG(D_FUNC, (L"%s: %d\n", FULL_FUNC_NAME, len));

    assert(_head);

    _data -= len;
    _data_len += len;

    assert(_data >= _head);

    return _data;
}

PBYTE BufferItem::Pull(UINT len)
{
    DBG(D_FUNC, (L"%s: %d\n", FULL_FUNC_NAME, len));

    assert(_head);

    _data += len;
    _data_len -= len;

    return _data;
}

UINT BufferItem::room()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    return (UINT)(_end - _head);
}

PBYTE BufferItem::data()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    return _data;
}

UINT BufferItem::data_size()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    return _data_len;
}

PBYTE BufferItem::head()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    return _head;
}

UINT BufferItem::head_size()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    return (UINT)(_data - _head);
}

PBYTE BufferItem::tail()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    return _tail;
}

UINT BufferItem::tail_size()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    return (UINT)(_end - _tail);
}

