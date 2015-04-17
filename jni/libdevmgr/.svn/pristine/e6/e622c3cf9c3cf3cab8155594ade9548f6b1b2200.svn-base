#include <StdAfx.h>
#include "AdbCmdDelFile.h"


AdbCmdDelFile::AdbCmdDelFile(const char *szDevId, const char *szFileName)
    : _DevId(szDevId)
    , _FileName(szFileName)
{
    DBG(D_VERB, (L"%s: id(%hs)\n", FULL_FUNC_NAME, szDevId));
}

AdbCmdDelFile::~AdbCmdDelFile()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));
}

DWORD AdbCmdDelFile::BuildCmdline()
{
    char  buf[MAX_ADB_CMD_LEN];

    _snprintf(buf, _countof(buf), ADB_FMT_DELETE_FILE,
              _DevId.c_str(),
              _FileName.c_str());

    _Cmdline = buf;

    DBG(D_VERB, (L"%s: cmd(%hs)\n", FULL_FUNC_NAME, _Cmdline.c_str()));

    return ERROR_SUCCESS;
}

void AdbCmdDelFile::ProcessResult()
{
    PRET_DEL_FILE  pDelFile;

    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    _biResult.Alloc(sizeof(RET_DEL_FILE));
    pDelFile = (PRET_DEL_FILE)_biResult.Put(sizeof(RET_DEL_FILE));

    pDelFile->hdr.dwSize = _biResult.data_size();
    pDelFile->hdr.dwResult = ERROR_SUCCESS;
}

