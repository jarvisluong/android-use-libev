#include <StdAfx.h>
#include <AdbCmdStartService.h>
#include "AdbCmdInstallApk.h"


AdbCmdInstallApk::AdbCmdInstallApk(const char *szDevId, const char *szApkPath)
    : _DevId(szDevId)
    , _ApkPath(szApkPath)
{
    DBG(D_VERB, (L"%s: id(%hs), apk(%hs)\n", FULL_FUNC_NAME, szDevId, szApkPath));
}

AdbCmdInstallApk::~AdbCmdInstallApk()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));
}

DWORD AdbCmdInstallApk::BuildCmdline()
{
    char  buf[MAX_ADB_CMD_LEN];

    _snprintf(buf, _countof(buf), ADB_FMT_INSTALL_APK, _DevId.c_str(), _ApkPath.c_str());
    DBG(D_VERB, (L"%s: cmd(%hs)\n", FULL_FUNC_NAME, buf));

    _Cmdline = buf;

    return ERROR_SUCCESS;
}

void AdbCmdInstallApk::ProcessResult()
{
    AdbCmdStartService  cmd(_DevId.c_str(), ADB_SERVICED_NAME, true, "--ei REFRESH 1");

    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    _biResult.Alloc(sizeof(RET_INSTALL_APP));
    _biResult.Put(sizeof(RET_INSTALL_APP));

    AdbCmd::ProcessResult();

    cmd.Execute();
}

