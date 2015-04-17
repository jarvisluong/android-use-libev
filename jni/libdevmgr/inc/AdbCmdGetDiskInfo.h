#ifndef _ADB_CMD_GET_DISKINFO_H
#define _ADB_CMD_GET_DISKINFO_H


#include "AdbCmd.h"


class AdbCmdGetDiskInfo : public AdbCmd
{
public:
    AdbCmdGetDiskInfo(const char *szDevId);
    virtual ~AdbCmdGetDiskInfo();

protected:
    virtual DWORD BuildCmdline();
    virtual void ProcessResult();

protected:
    string                      _DevId;
};


#endif

