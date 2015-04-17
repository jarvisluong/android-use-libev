#ifndef _ADB_CMD_GET_PROP_H
#define _ADB_CMD_GET_PROP_H


#include "AdbCmd.h"


class AdbCmdGetProp : public AdbCmd
{
public:
    AdbCmdGetProp(const char *szDevId);
    virtual ~AdbCmdGetProp();

    void SetPropName(const char *szName);

protected:
    virtual DWORD BuildCmdline();
    virtual void ProcessResult();

protected:
    string                      _DevId;
    string                      _PropName;
};


#endif

