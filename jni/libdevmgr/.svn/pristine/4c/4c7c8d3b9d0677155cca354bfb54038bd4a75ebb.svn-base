#ifndef _DB_TABLE_BASE_H
#define _DB_TABLE_BASE_H


#include "BufferItem.h"
#include "DbTableObserver.h"


class DbTableBase
{
public:
    DbTableBase();
    virtual ~DbTableBase();

    DWORD RegisterObserver(DbTableObserver *pObserver);
    DWORD UnregisterObserver(DbTableObserver *pObserver);

protected:
    void Notify(DBMSG_CODE MsgCode, const char *szDevId);

private:
    pthread_mutex_t             _ObserverLock;
    list<DbTableObserver*>      _ObserverList;
};


#endif

