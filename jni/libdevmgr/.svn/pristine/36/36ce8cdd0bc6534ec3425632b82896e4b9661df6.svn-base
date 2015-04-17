#ifndef _ADB_CMD_GET_SYSINFO_H
#define _ADB_CMD_GET_SYSINFO_H


#include "AdbCmd.h"


class AdbCmdGetSysInfo : public AdbCmd
{
public:
    AdbCmdGetSysInfo(const char *szDevId, DWORD dwMask);
    virtual ~AdbCmdGetSysInfo();

    virtual DWORD Execute();

protected:
    virtual DWORD BuildCmdline();
    virtual void ProcessResult();

protected:
    string                      _DevId;
    DWORD                       _dwMask;
};


#endif

