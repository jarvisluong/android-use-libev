#ifndef _ADB_CMD_H
#define _ADB_CMD_H


#include "BufferItem.h"


class AdbCmd
{
public:
    AdbCmd();
    virtual ~AdbCmd();

    virtual DWORD Execute();
    DWORD GetResult(PBYTE pBuf, DWORD &dwSize);

protected:
    virtual DWORD BuildCmdline() = 0;
    virtual void OnChildOutput();
    virtual void OnChildExit();
    virtual void ProcessResult();

private:
    DWORD CreateChildProcess();
    void ProcessChildOutput();
    void Cleanup();

protected:
    string                      _Cmdline;

#ifdef _WIN32_
    HANDLE                      _hChildProcess;
    HANDLE                      _hParentRd;
    HANDLE                      _hParentWr;
#endif

#ifdef _LINUX_
    pid_t                       _hChildProcess;
    int                         _hParentRd;
    int                         _hParentWr;
#endif

    BufferItem                  _biOutput;
    BufferItem                  _biResult;
};


#endif

