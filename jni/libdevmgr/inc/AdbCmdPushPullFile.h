#ifndef _ADB_CMD_PUSH_PULL_FILE_H
#define _ADB_CMD_PUSH_PULL_FILE_H


#include "AdbCmd.h"


class AdbCmdPushPullFile : public AdbCmd
{
public:
    AdbCmdPushPullFile(const char *szDevId, const char *szFilePathH, const char *szPathD, const char *szFileD, bool bPush);
    virtual ~AdbCmdPushPullFile();

    virtual DWORD Execute();

    void SetPath(const char *szFilePathH, const char *szPathD, const char *szFileD);

protected:
    virtual DWORD BuildCmdline();
    virtual void ProcessResult();

    DWORD MtpSendFile();

protected:
    string                      _DevId;
    string                      _FilePath_H;
    string                      _Path_D;
    string                      _File_D;
    bool                        _bPush;
};


#endif

