#ifndef _EXT_CMD_COUNT_STR_H
#define _EXT_CMD_COUNT_STR_H


#include "AdbCmd.h"


class ExtCmdCountStr : public AdbCmd
{
public:
    ExtCmdCountStr(const char *szSource, const char *szMatch);
    virtual ~ExtCmdCountStr();

protected:
    virtual DWORD BuildCmdline();
    virtual void ProcessResult();

protected:
    string                      _Source;
    string                      _Match;
};


#endif

