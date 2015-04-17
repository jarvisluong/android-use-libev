#ifndef _ADB_CMD_DEL_FILE_H
#define _ADB_CMD_DEL_FILE_H


#include "AdbCmd.h"


class AdbCmdDelFile : public AdbCmd
{
public:
    AdbCmdDelFile(const char *szDevId, const char *szFileName);
    virtual ~AdbCmdDelFile();

protected:
    virtual DWORD BuildCmdline();
    virtual void ProcessResult();

protected:
    string                      _DevId;
    string                      _FileName;
};


#endif

