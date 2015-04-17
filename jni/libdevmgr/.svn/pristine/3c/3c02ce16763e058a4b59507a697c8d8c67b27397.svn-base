#include <StdAfx.h>
#include "SqlWrapper.h"


#ifdef USE_PGSQL
#define SQLCLOSE(x)             PQfinish(x)
#define SQLERRNO(x)             (-1)

typedef struct _PGSQL_RES
{
    int                         nRowCnt;
    int                         nColCnt;
    int                         nCurRow;
    char                     ** pFields;
    PGresult                  * res;
}
PGSQL_RES;
#endif
#ifdef USE_MYSQL
#define SQLCLOSE(x)             mysql_close(x)
#define SQLERRNO(x)             mysql_errno(x)
#endif
#ifdef USE_SQLITE
#define SQLCLOSE(x)             sqlite3_close(x)
#define SQLERRNO(x)             sqlite3_errcode(x)

typedef struct _SQLITE_RES
{
    int                         nRowCnt;
    int                         nColCnt;
    int                         nCurRow;
    int                         nCurIdx;
    char                     ** ppRows;
}
SQLITE_RES;
#endif


SqlWrapper *SqlWrapper::_instance = NULL;

SqlWrapper *SqlWrapper::Instance()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    if ( !_instance )
    {
        DBG(D_VERB, (L"%s: %ls\n", FULL_FUNC_NAME, L"CreateInstance"));
        _instance = new SqlWrapper();
    }
    return _instance;
}

void SqlWrapper::Destroy()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    delete _instance;
    _instance = NULL;
}

SqlWrapper::SqlWrapper()
    : _hDbConn(NULL)
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    pthread_mutex_init(&_hDbLock, NULL);
}

SqlWrapper::~SqlWrapper()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    pthread_mutex_destroy(&_hDbLock);
}

DWORD SqlWrapper::Load(const char *szHost, const char *szUser, const char *szPwd, const char *szDb, WORD wPort)
{
    DWORD  dwRet;
#ifdef USE_PGSQL
    char  szPort[10];
#endif
#ifdef USE_MYSQL
    MYSQL  *pSqlCon;
#endif
#ifdef USE_SQLITE
    int  ret;
#endif

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

#ifdef USE_PGSQL
    _snprintf(szPort, _countof(szPort), "%d", wPort);
    _hDbConn = PQsetdbLogin(szHost, szPort, NULL, NULL, szDb, szUser, szPwd);
    if ( PQstatus(_hDbConn) != CONNECTION_OK )
#endif
#ifdef USE_MYSQL
    _hDbConn = mysql_init(NULL);
    pSqlCon = mysql_real_connect(_hDbConn, szHost, szUser, szPwd, szDb, wPort, NULL, 0);
    if ( pSqlCon != _hDbConn )
#endif
#ifdef USE_SQLITE
    ret = sqlite3_open(szDb, &_hDbConn);
    if ( ret )
#endif
    {
        dwRet = SQLERRNO(_hDbConn);
        DBG(D_ERROR, (L"%s: open Db failed: %d\n", FULL_FUNC_NAME, dwRet));
        goto err1;
    }

#ifdef USE_MYSQL
    mysql_set_character_set(_hDbConn, "utf8");
#endif

    return ERROR_SUCCESS;

err1:
    SQLCLOSE(_hDbConn);
    _hDbConn = NULL;

    return dwRet;
}

void SqlWrapper::Unload()
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    if ( _hDbConn )
    {
        SQLCLOSE(_hDbConn);
        _hDbConn = NULL;
    }
}

void SqlWrapper::Lock()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    pthread_mutex_lock(&_hDbLock);
}

void SqlWrapper::Unlock()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    pthread_mutex_unlock(&_hDbLock);
}

DWORD SqlWrapper::ExecSql(const char *sql)
{
    DWORD  dwRet = ERROR_SUCCESS;
    int  ret;
#ifdef USE_PGSQL
    char  *msg;
    PGresult  *res;
    ExecStatusType  status;
#endif

    DBG(D_INFO, (L"%s: \"%hs\"\n", FULL_FUNC_NAME, sql));
#if 0
{
    char  *d, buf[600];
    const char  *s;
    s = sql;
    d = buf;
    while ( *s )
    {
        if ( ((*s) >=0x20) && ((*s) <=0x7E) )
        {
            *d = *s;
        }
        else
        {
            *d = '?';
        }
        s++;
        d++;
    }
    *d = 0;
    DBG(D_INFO, (L"%s: \"%hs\"\n", FULL_FUNC_NAME, buf));
}
#endif

    Lock();

#ifdef USE_PGSQL
    res = PQexec(_hDbConn, sql);
    status = PQresultStatus(res);
    if ( status != PGRES_COMMAND_OK )
    {
        msg = PQresultErrorMessage(res);
        DBG(D_WARN, (L"%s: \"%hs\" failed: %d, %hs\n", FULL_FUNC_NAME, sql, status, msg ? msg : "?"));
        ret = -1;
    }
    else
    {
        ret = 0;
    }
    PQclear(res);
#endif
#ifdef USE_MYSQL
    ret = mysql_query(_hDbConn, sql);
#endif
#ifdef USE_SQLITE
    ret = sqlite3_exec(_hDbConn, sql, NULL, NULL, NULL);
#endif
    if ( ret )
    {
        dwRet = SQLERRNO(_hDbConn);
        DBG(D_WARN, (L"%s: query(%hs) failed: %d\n", FULL_FUNC_NAME, sql, dwRet));
    }

    Unlock();

    return dwRet;
}

DWORD SqlWrapper::BeginGetTable(const char *sql, void **phRes)
{
    DWORD  dwRet = ERROR_SUCCESS;
    int  ret;
#ifdef USE_PGSQL
    char  *msg;
    PGresult  *res;
    ExecStatusType  status;
    PGSQL_RES  *pRes;
#endif
#ifdef USE_MYSQL
    MYSQL_RES  *pRes;
#endif
#ifdef USE_SQLITE
    int  nRow, nCol;
    char  **ppRows;
    SQLITE_RES  *pRes;
#endif

    DBG(D_INFO, (L"%s: \"%hs\"\n", FULL_FUNC_NAME, sql));

    *phRes = NULL;

    Lock();

#ifdef USE_PGSQL
    res = PQexec(_hDbConn, sql);
    status = PQresultStatus(res);
    if ( status != PGRES_TUPLES_OK )
    {
        msg = PQresultErrorMessage(res);
        DBG(D_WARN, (L"%s: \"%hs\" failed: %d, %hs\n", FULL_FUNC_NAME, sql, status, msg ? msg : "?"));
        ret = -1;
        PQclear(res);
    }
    else
    {
        ret = 0;
    }
#endif

#ifdef USE_MYSQL
    ret = mysql_query(_hDbConn, sql);
#endif
#ifdef USE_SQLITE
    ret = sqlite3_get_table(_hDbConn, sql, &ppRows, &nRow, &nCol, NULL);
#endif
    if ( ret )
    {
        dwRet = SQLERRNO(_hDbConn);
        DBG(D_WARN, (L"%s: query(%hs) failed: %d\n", FULL_FUNC_NAME, sql, dwRet));
        goto out;
    }

#ifdef USE_PGSQL
    pRes = new PGSQL_RES;
    pRes->nRowCnt = PQntuples(res);
    pRes->nColCnt = PQnfields(res);
    pRes->nCurRow = 0;
    pRes->pFields = new char*[pRes->nColCnt];
    pRes->res = res;
#endif

#ifdef USE_MYSQL
    pRes = mysql_store_result(_hDbConn);
    if ( !pRes )
    {
        dwRet = SQLERRNO(_hDbConn);
        DBG(D_WARN, (L"%s: mysql_store_result failed: %d\n", FULL_FUNC_NAME, dwRet));
        goto out;
    }
#endif

#ifdef USE_SQLITE
    pRes = new SQLITE_RES;
    pRes->nRowCnt = nRow;
    pRes->nColCnt = nCol;
    pRes->nCurRow = 0;
    pRes->nCurIdx = 0;
    pRes->ppRows = ppRows;
#endif

    *phRes = (void*)pRes;

out:
    Unlock();

    return dwRet;
}

char **SqlWrapper::FetchRow(void *hRes)
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    assert(hRes);

#ifdef USE_PGSQL
    PGSQL_RES *pRes = (PGSQL_RES*)hRes;
    if ( pRes->nCurRow >= pRes->nRowCnt )
    {
        return NULL;
    }
    for ( int i = 0; i < pRes->nColCnt; i++ )
    {
        pRes->pFields[i] = PQgetvalue(pRes->res, pRes->nCurRow, i);
    }
    pRes->nCurRow++;
    return pRes->pFields;
#endif

#ifdef USE_MYSQL
    return mysql_fetch_row((MYSQL_RES*)hRes);
#endif

#ifdef USE_SQLITE
    SQLITE_RES *pRes = (SQLITE_RES*)hRes;
    if ( pRes->nCurRow >= pRes->nRowCnt )
    {
        return NULL;
    }
    pRes->nCurRow++;
    pRes->nCurIdx += pRes->nColCnt;
    return &pRes->ppRows[pRes->nCurIdx];
#endif
}

void SqlWrapper::EndGetTable(void *hRes)
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    if ( hRes )
    {
#ifdef USE_PGSQL
        PGSQL_RES *pRes = (PGSQL_RES*)hRes;
        delete []pRes->pFields;
        PQclear(pRes->res);
        delete pRes;
#endif
#ifdef USE_MYSQL
        mysql_free_result((MYSQL_RES*)hRes);
#endif
#ifdef USE_SQLITE
        SQLITE_RES *pRes = (SQLITE_RES*)hRes;
        sqlite3_free_table(pRes->ppRows);
        delete pRes;
#endif
    }
}

