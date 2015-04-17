#include <StdAfx.h>
#include "AdbCmdGetProp.h"


AdbCmdGetProp::AdbCmdGetProp(const char *szDevId)
    : _DevId(szDevId)
{
    DBG(D_VERB, (L"%s: id(%hs)\n", FULL_FUNC_NAME, szDevId));
}

AdbCmdGetProp::~AdbCmdGetProp()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));
}

void AdbCmdGetProp::SetPropName(const char *szName)
{
    DBG(D_VERB, (L"%s: name(%hs)\n", FULL_FUNC_NAME, szName));

    _PropName = szName;
}

DWORD AdbCmdGetProp::BuildCmdline()
{
    char  buf[MAX_ADB_CMD_LEN];

    _snprintf(buf, _countof(buf), ADB_FMT_GET_PROP, _DevId.c_str(), _PropName.c_str());
    DBG(D_VERB, (L"%s: cmd(%hs)\n", FULL_FUNC_NAME, buf));

    _Cmdline = buf;

    return ERROR_SUCCESS;
}

void AdbCmdGetProp::ProcessResult()
{
    BOOL  ret;
    UINT  size;
    char  *p;
    const char  *p1, *p2;
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    p1 = (char*)_biOutput.data();
    size = _biOutput.data_size();

    _biResult.Alloc(size);

    ret = MatchStringAnywhere(p1, "error", p1);
    if ( ret )
    {
        p = (char*)_biResult.Put(1);
        *p = 0;
        return;
    }

    ret = ParseUnquotedString(p1, '\r', (char*)_biResult.head(), size, p2);
    if ( ret )
    {
        _biResult.Put(p2 - p1 + 1);
    }
}

