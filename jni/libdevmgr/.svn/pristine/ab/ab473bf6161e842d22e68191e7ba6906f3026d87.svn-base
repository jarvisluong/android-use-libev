#ifndef _RING_BUFFER_H
#define _RING_BUFFER_H


class RingBuffer
{
public:
    RingBuffer(const wchar_t *lpName, int nSize);
    ~RingBuffer();

    void Reset();

    bool IsEmpty();
    bool IsFull();

    void PushAtBack(PBYTE pItem);
    PBYTE PopAtFront();

    bool PushAtBackSafe(PBYTE pItem);
    PBYTE PopAtFrontSafe();

private:
    wstring                     _name;
    int                         _nPushPos;
    int                         _nPopPos;
    int                         _nSize;
    PBYTE                     * _pRingBuf;
};


#endif