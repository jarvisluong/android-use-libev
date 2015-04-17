#include <StdAfx.h>
#include "AdbCmdUninstallApk.h"


AdbCmdUninstallApk::AdbCmdUninstallApk(const char *szDevId, const char *szApkName)
    : _DevId(szDevId)
    , _ApkName(szApkName)
{
    DBG(D_VERB, (L"%s: id(%hs), apk(%hs)\n", FULL_FUNC_NAME, szDevId, szApkName));
}

AdbCmdUninstallApk::~AdbCmdUninstallApk()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));
}

DWORD AdbCmdUninstallApk::BuildCmdline()
{
    char  buf[MAX_ADB_CMD_LEN];

    _snprintf(buf, _countof(buf), ADB_FMT_UNINSTALL_APK, _DevId.c_str(), _ApkName.c_str());
    DBG(D_VERB, (L"%s: cmd(%hs)\n", FULL_FUNC_NAME, buf));

    _Cmdline = buf;

    return ERROR_SUCCESS;
}

void AdbCmdUninstallApk::ProcessResult()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    _biResult.Alloc(sizeof(RET_UNINSTALL_APP));
    _biResult.Put(sizeof(RET_UNINSTALL_APP));

    AdbCmd::ProcessResult();
}

