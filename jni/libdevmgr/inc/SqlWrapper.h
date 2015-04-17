#ifndef _MYSQL_WRAPPER_H
#define _MYSQL_WRAPPER_H


#ifdef USE_PGSQL
#include "pgsql/libpq-fe.h"
#endif

#ifdef USE_MYSQL
#include "mysql/mysql.h"
#endif

#ifdef USE_SQLITE
#include "sqlite3.h"
#endif


class SqlWrapper
{
public:
    static SqlWrapper *Instance();
    static void Destroy();

private:
    static SqlWrapper         * _instance;

protected:
    SqlWrapper();
    ~SqlWrapper();

public:
    DWORD Load(const char *szHost, const char *szUser, const char *szPwd, const char *szDb, WORD wPort);
    void Unload();

    void Lock();
    void Unlock();

    DWORD ExecSql(const char *sql);

    DWORD BeginGetTable(const char *sql, void **phRes);
    char **FetchRow(void *hRes);
    void EndGetTable(void *hRes);

private:
#ifdef USE_PGSQL
    PGconn                    * _hDbConn;
#endif
#ifdef USE_MYSQL
    MYSQL                     * _hDbConn;
#endif
#ifdef USE_SQLITE
    sqlite3                   * _hDbConn;
#endif

    pthread_mutex_t             _hDbLock;
};


#endif
