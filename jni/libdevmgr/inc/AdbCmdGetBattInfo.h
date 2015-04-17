#ifndef _ADB_CMD_GET_BATTINFO_H
#define _ADB_CMD_GET_BATTINFO_H


#include "AdbCmd.h"


class AdbCmdGetBattInfo : public AdbCmd
{
public:
    AdbCmdGetBattInfo(const char *szDevId);
    virtual ~AdbCmdGetBattInfo();

protected:
    virtual DWORD BuildCmdline();
    virtual void ProcessResult();

protected:
    string                      _DevId;
};


#endif

