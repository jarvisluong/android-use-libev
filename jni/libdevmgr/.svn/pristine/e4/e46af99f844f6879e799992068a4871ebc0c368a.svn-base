#ifndef _ADB_CMD_UNINSTALL_APK_H
#define _ADB_CMD_UNINSTALL_APK_H


#include "AdbCmd.h"


class AdbCmdUninstallApk : public AdbCmd
{
public:
    AdbCmdUninstallApk(const char *szDevId, const char *szApkName);
    virtual ~AdbCmdUninstallApk();

protected:
    virtual DWORD BuildCmdline();
    virtual void ProcessResult();

protected:
    string                      _DevId;
    string                      _ApkName;
};


#endif

