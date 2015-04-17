#ifndef _ADB_CMD_INSTALL_APK_H
#define _ADB_CMD_INSTALL_APK_H


#include "AdbCmd.h"


class AdbCmdInstallApk : public AdbCmd
{
public:
    AdbCmdInstallApk(const char *szDevId, const char *szApkPath);
    virtual ~AdbCmdInstallApk();

protected:
    virtual DWORD BuildCmdline();
    virtual void ProcessResult();

protected:
    string                      _DevId;
    string                      _ApkPath;
};


#endif

