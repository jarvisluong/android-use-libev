#ifndef _POBJECT_H
#define _POBJECT_H


class PObject
{
protected:
    PObject();
    virtual ~PObject();

public:
    LONG AddRef();
    LONG Release();

private:
    pthread_mutex_t             _RefLock;
    LONG                        _lRefCnt;
};


#endif

