#ifndef _ADB_CMD_GET_APKLIST_H
#define _ADB_CMD_GET_APKLIST_H


#include "AdbCmd.h"


class AdbCmdGetApkList : public AdbCmd
{
public:
    AdbCmdGetApkList(const char *szDevId, const char *szIconDir);
    virtual ~AdbCmdGetApkList();

    virtual DWORD Execute();

protected:
    virtual DWORD BuildCmdline();
    virtual void ProcessResult();

protected:
    string                      _DevId;
    string                      _IconDir;
};


#endif

