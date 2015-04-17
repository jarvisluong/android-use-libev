#ifndef _DB_TABLE_OBSERVER_H
#define _DB_TABLE_OBSERVER_H


#include "PObject.h"
#include "BufferItem.h"


class DbTableObserver
{
public:
    // be careful for the reentry
    virtual void Notify(DBMSG_CODE MsgCode, const char *szDevId) = 0;
};


#endif

