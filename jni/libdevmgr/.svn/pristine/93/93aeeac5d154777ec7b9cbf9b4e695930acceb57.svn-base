#ifndef _ADB_CMD_SCREEN_CAP_H
#define _ADB_CMD_SCREEN_CAP_H


#include "AdbCmd.h"


class AdbCmdScreenCap : public AdbCmd
{
public:
    AdbCmdScreenCap(const char *szDevId, const char *szFileName);
    virtual ~AdbCmdScreenCap();

protected:
    virtual DWORD BuildCmdline();
    virtual void ProcessResult();

protected:
    string                      _DevId;
    string                      _FileName;
};


#endif

