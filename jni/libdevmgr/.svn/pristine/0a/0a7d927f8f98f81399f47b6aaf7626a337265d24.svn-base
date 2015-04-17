#include <StdAfx.h>
#include "ExtCmdCountStr.h"


ExtCmdCountStr::ExtCmdCountStr(const char *szSource, const char *szMatch)
    : _Source(szSource)
    , _Match(szMatch)
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));
}

ExtCmdCountStr::~ExtCmdCountStr()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));
}

DWORD ExtCmdCountStr::BuildCmdline()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    _Cmdline = _Source + " | grep -c \"" + _Match + "\"";

    return ERROR_SUCCESS;
}

void ExtCmdCountStr::ProcessResult()
{
    BOOL  ret;
    const char  *szOutput, *p;
    DWORD  *pdwCount;

    szOutput = (const char*)_biOutput.data();
    DBG(D_INFO, (L"%s: output(%hs)\n", FULL_FUNC_NAME, szOutput));

    _biResult.Alloc(sizeof(DWORD));
    pdwCount = (DWORD*)_biResult.Put(sizeof(DWORD));

    ret = ParseDWord(szOutput, FALSE, *pdwCount, p);
    if ( !ret )
    {
        *pdwCount = 0;
    }
}

