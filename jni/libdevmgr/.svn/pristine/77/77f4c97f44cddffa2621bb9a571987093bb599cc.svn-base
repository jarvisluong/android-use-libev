#ifndef _ADB_CMD_START_SERVICE_H
#define _ADB_CMD_START_SERVICE_H


#include "AdbCmd.h"


class AdbCmdStartService : public AdbCmd
{
public:
    AdbCmdStartService(const char *szDevId, const char *szSrvName, bool bAdmin, const char *szArgs);
    virtual ~AdbCmdStartService();

protected:
    virtual DWORD BuildCmdline();
    virtual void ProcessResult();

protected:
    string                      _DevId;
    string                      _SrvName;
    bool                        _bAdmin;
    string                      _Args;
};


#endif

