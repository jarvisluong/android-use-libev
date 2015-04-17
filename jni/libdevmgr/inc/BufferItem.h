#ifndef _BUFFER_ITEM_H
#define _BUFFER_ITEM_H


class BufferItem
{
public:
    BufferItem();
    ~BufferItem();

    void Alloc(UINT size);
    void Expand(UINT len);
    void Reset(UINT len = 0);

    void Reserve(UINT len);
    PBYTE Put(UINT len);
    PBYTE Push(UINT len);
    PBYTE Pull(UINT len);

    UINT room();
    PBYTE data();
    UINT data_size();
    PBYTE head();
    UINT head_size();
    PBYTE tail();
    UINT tail_size();

private:
    UINT                        _data_len;
    PBYTE                       _head;
    PBYTE                       _data;
    PBYTE                       _tail;
    PBYTE                       _end;
};


#endif

