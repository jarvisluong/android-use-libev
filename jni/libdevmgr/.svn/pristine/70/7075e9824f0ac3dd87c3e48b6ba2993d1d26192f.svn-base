#ifndef _GLOBAL_DEF_H
#define _GLOBAL_DEF_H


typedef enum _ADDR_TYPE
{
    ADDR_TYPE_IPV4,
    ADDR_TYPE_UNKNOWN
}
ADDR_TYPE;

typedef enum _PDU_CODE
{
    C_PDU_DBMSG = 0,
    C_PDU_DEVCMD
}
PDU_CODE;

typedef enum _DBMSG_CODE
{
    C_DBMSG_CMDTAB_UPDATED = 0,

    C_DBMSG_DEVTAB_UPDATED,

    C_DBMSG_UPDATE_SYSINFO,
    C_DBMSG_SYSINFO_UPDATED,

    C_DBMSG_UPDATE_BATTINFO,
    C_DBMSG_BATTINFO_UPDATED,

    C_DBMSG_UPDATE_DISKINFO,
    C_DBMSG_DISKINFO_UPDATED,

    C_DBMSG_UPDATE_APPLIST,
    C_DBMSG_APPLIST_UPDATED,

    C_DBMSG_FIND_ME
}
DBMSG_CODE;

typedef enum _DEVCMD_CODE
{
    C_DEVCMD_INSTALL_APP = 0,
    C_DEVCMD_UNINSTALL_APP,
    C_DEVCMD_PUSH_FILE,
    C_DEVCMD_PULL_FILE,

    C_DEVCMD_LABEL_SHOW,
    C_DEVCMD_SCREEN_CAP,

    C_DEVCMD_GET_SYSINFO,
    C_DEVCMD_GET_BATTINFO,
    C_DEVCMD_GET_DISKINFO,

    C_DEVCMD_GET_APPLIST,

    C_DEVCMD_DEL_FILE,
    C_DEVCMD_INSTALL_INFOVIEW,

    C_DEVCMD_FIND_ME,

    C_DEVCMD_LAST_ONE
}
DEVCMD_CODE;

typedef enum _DEV_STATE
{
    DEV_STATE_OFF = 0,
    DEV_STATE_BOOTING,
    DEV_STATE_CHARGER,
    DEV_STATE_MAIN
}
DEV_STATE;

typedef struct _STR2CODE_MAP
{
    const char                    * str;
    DWORD                           dwCode;
}
STR2CODE_MAP, *PSTR2CODE_MAP;


const int  MSG_SERVER_LISTEN_PORT = 3620;

const int  NET_RECV_TIMEOUT       = 5000;
const int  NET_SEND_TIMEOUT       = 5000;

const int  PDU_HEAD_RESERVE       = 256;
const int  PDU_MAX_LENGTH         = 1000;
const DWORD  PDU_MAGIC_NUM        = 0x5544505F;  // '_PDU'


const int  MAX_DEV_ID_LEN         = 64;
const int  MAX_FILE_SUFFIX_LEN    = 16;
const int  MAX_SYSINFO_ITEM_LEN   = 32;

const int  MAX_APP_LIST_NUM       = 256;
const int  MAX_APP_NAME_LEN       = 64;
const int  MAX_APP_LONGNAME_LEN   = 128;
const int  MAX_APP_ICONNAME_LEN   = 128;
const int  MAX_APP_VERSION_LEN    = 16;

const int  SYSINFO_MASK_OSNAME    = 0x0001;
const int  SYSINFO_MASK_OSVER     = 0x0002;
const int  SYSINFO_MASK_BRAND     = 0x0004;
const int  SYSINFO_MASK_MODEL     = 0x0008;


const int  MAX_ADB_CMD_LEN        = 500;

const int  MAX_ADB_DEV_CNT        = 128;

const int  ADB_DEV_PROBE_TIMER    = 500;
const int  ADB_DEV_PROBE_COUNT    = 4;

const int  ADB_DEV_QUERY_TIMER    = 5000;

const char * const  ADB_SERVICED_NAME     = "infoview.service";
const char * const  ADB_APKICON_DIR       = "/sdcard/tablet_info/";
const char * const  ADB_APKLIST_FILE      = "/sdcard/tablet_info/_app.lst";
const char * const  ADB_SCREENCAP_FILE    = "/sdcard/tablet_info/_scr.png";

const char * const  ADB_FMT_ENUM_DEVICES  = "adb devices";
const char * const  ADB_FMT_INSTALL_APK   = "adb -s %s install -r %s";
const char * const  ADB_FMT_UNINSTALL_APK = "adb -s %s uninstall %s";
const char * const  ADB_FMT_START_SERVICE = "adb -s %s shell am startservice %s %s -a %s";
const char * const  ADB_FMT_PUSH_FILE     = "adb -s %s push \"%s\" \"%s\"";
const char * const  ADB_FMT_PULL_FILE     = "adb -s %s pull \"%s\" \"%s\"";
const char * const  ADB_FMT_GET_PROP      = "adb -s %s shell getprop %s";
const char * const  ADB_FMT_DUMPSYS       = "adb -s %s shell dumpsys %s";
const char * const  ADB_FMT_CATFILE       = "adb -s %s shell cat \"%s\"";
const char * const  ADB_FMT_SCREENCAP     = "adb -s %s shell screencap -p %s";
const char * const  ADB_FMT_DELETE_FILE   = "adb -s %s shell rm \"%s\"";
const char * const  ADB_FMT_REBOOT_DEVICE = "adb -s %s reboot";

const int  ADB_DEV_QUEUE_CNT      = 4;

const int  ADB_DEV_THREAD_CNT     = (1+1+4+2);
const int  ADB_CMD_THREAD_MAP[]   =
{
    1,
    1,
    4,
    2
};

const int  ADB_CMD_QUEUE_MAP[]    =
{
    2,  // C_DEVCMD_INSTALL_APP
    2,  // C_DEVCMD_UNINSTALL_APP
    3,  // C_DEVCMD_PUSH_FILE
    3,  // C_DEVCMD_PULL_FILE
    1,  // C_DEVCMD_LABEL_SHOW
    1,  // C_DEVCMD_SCREEN_CAP
    0,  // C_DEVCMD_GET_SYSINFO
    0,  // C_DEVCMD_GET_BATTINFO
    0,  // C_DEVCMD_GET_DISKINFO
    1,  // C_DEVCMD_GET_APPLIST
    2,  // C_DEVCMD_DEL_FILE
    1,  // C_DEVCMD_INSTALL_INFOVIEW
    1   // C_DEVCMD_FIND_ME
};

const STR2CODE_MAP  ADB_CMD_RET_MAP[] =
{
    {"Success", ERROR_SUCCESS},
    {"can't find", ERROR_NOT_FOUND},
    {"does not exist", ERROR_NOT_FOUND},
    {"No such file or directory", ERROR_NOT_FOUND},
    {"device not found", ERROR_NOT_CONNECTED},
    {"device offline", ERROR_NOT_CONNECTED},
    {"Permission denied", ERROR_ACCESS_DENIED},
    {"INSTALL_FAILED_ALREADY_EXISTS", ERROR_ALREADY_EXISTS},
    {"no service started", ERROR_NOT_READY},
    {"Can't connect to activity manager", ERROR_NOT_READY},
    {"Could not access the Package Manager", ERROR_NOT_READY},
    {"bytes in", ERROR_SUCCESS},
    {"Starting service:", ERROR_SUCCESS}  // after "no service started"
};


#pragma pack(1)

typedef struct _IPV4_ADDR
{
    BYTE                            bStream;
    WORD                            wPort;  // network order
    DWORD                           dwIP;   // network order
}
IPV4_ADDR, *PIPV4_ADDR;

typedef struct _TRANS_ADDR
{
    BYTE                            bType;
    union
    {
        IPV4_ADDR                   IPv4Addr;
    };
}
TRANS_ADDR, *PTRANS_ADDR;

typedef struct _PDU_COMM_HDR
{
    DWORD                           dwMagic;
    WORD                            wBodySize;
    BYTE                            bCode;
    BYTE                            bReq : 1;
    BYTE                            bErr : 1;
    BYTE                            bReserved : 6;
    DWORD                           dwParam;
}
PDU_COMM_HDR, *PPDU_COMM_HDR;

typedef struct _PDU_DBMSG_BODY
{
    DWORD                           dwDevNum;
    BYTE                            szDevId[1][MAX_DEV_ID_LEN];
}
PDU_DBMSG_BODY, *PPDU_DBMSG_BODY;

typedef struct _APP_INFO
{
    BYTE                            szName[MAX_APP_NAME_LEN];
    BYTE                            szLongName[MAX_APP_LONGNAME_LEN];
    BYTE                            szIconName[MAX_APP_ICONNAME_LEN];
    BYTE                            szVersion[MAX_APP_VERSION_LEN];
    DWORD                           dwVersion;
}
APP_INFO, *PAPP_INFO;

typedef struct _ARGS_INSTALL_APP
{
    DWORD                           dwPathSize;
    BYTE                            szAppPath[1];
}
ARGS_INSTALL_APP, *PARGS_INSTALL_APP;

typedef struct _ARGS_UNINSTALL_APP
{
    DWORD                           dwNameSize;
    BYTE                            szAppName[1];
}
ARGS_UNINSTALL_APP, *PARGS_UNINSTALL_APP;

typedef struct _ARGS_PUSH_PULL_FILE
{
    BYTE                            szPathH[MAX_PATH];
    BYTE                            szPathD[MAX_PATH];
    BYTE                            szFileD[MAX_PATH];
}
ARGS_PUSH_PULL_FILE, *PARGS_PUSH_PULL_FILE;

typedef struct _ARGS_SHOW_LABEL
{
    BYTE                            bFastCharging;
    DWORD                           dwNum;
}
ARGS_SHOW_LABEL, *PARGS_SHOW_LABEL;

typedef struct _ARGS_GET_SYSINFO
{
    DWORD                           dwMask;
}
ARGS_GET_SYSINFO, *PARGS_GET_SYSINFO;

typedef struct _ARGS_GET_BATTINFO
{
    DWORD                           dwReserved;
}
ARGS_GET_BATTINFO, *PARGS_GET_BATTINFO;

typedef struct _ARGS_GET_DISKINFO
{
    DWORD                           dwReserved;
}
ARGS_GET_DISKINFO, *PARGS_GET_DISKINFO;

typedef struct _ARGS_GET_APPLIST
{
    BYTE                            szIconDir[MAX_PATH];
}
ARGS_GET_APPLIST, *PARGS_GET_APPLIST;

typedef struct _ARGS_SCREEN_CAP
{
    BYTE                            szFileName[MAX_PATH];
}
ARGS_SCREEN_CAP, *PARGS_SCREEN_CAP;

typedef struct _ARGS_DEL_FILE
{
    BYTE                            szFileName[MAX_PATH];
}
ARGS_DEL_FILE, *PARGS_DEL_FILE;

typedef struct _ARGS_FIND_ME
{
    DWORD                           dwReserved;
}
ARGS_FIND_ME, *PARGS_FIND_ME;

typedef struct _RET_COMM_HDR
{
    DWORD                           dwSize;
    DWORD                           dwResult;
}
RET_COMM_HDR, *PRET_COMM_HDR;

typedef struct _RET_ENUM_DEVICES
{
    RET_COMM_HDR                    hdr;
    DWORD                           dwDevNum;
    BYTE                            szDevId[1][MAX_DEV_ID_LEN];
}
RET_ENUM_DEVICES, *PRET_ENUM_DEVICES;

typedef struct _RET_INSTALL_APP
{
    RET_COMM_HDR                    hdr;
}
RET_INSTALL_APP, *PRET_INSTALL_APP;

typedef struct _RET_UNINSTALL_APP
{
    RET_COMM_HDR                    hdr;
}
RET_UNINSTALL_APP, *PRET_UNINSTALL_APP;

typedef struct _RET_PUSH_PULL_FILE
{
    RET_COMM_HDR                    hdr;
}
RET_PUSH_PULL_FILE, *PRET_PUSH_PULL_FILE;

typedef struct _RET_START_SERVICE
{
    RET_COMM_HDR                    hdr;
}
RET_START_SERVICE, *PRET_START_SERVICE;

typedef struct _RET_GET_SYSINFO
{
    RET_COMM_HDR                    hdr;

    DWORD                           dwMask;
    BYTE                            szOsName[MAX_SYSINFO_ITEM_LEN];
    BYTE                            szOsVer[MAX_SYSINFO_ITEM_LEN];
    BYTE                            szBrand[MAX_SYSINFO_ITEM_LEN];
    BYTE                            szModel[MAX_SYSINFO_ITEM_LEN];
}
RET_GET_SYSINFO, *PRET_GET_SYSINFO;

typedef struct _RET_GET_BATTNFO
{
    RET_COMM_HDR                    hdr;

    BYTE                            bCharging;
    BYTE                            bLevel;
}
RET_GET_BATTINFO, *PRET_GET_BATTINFO;

typedef struct _RET_GET_DISKINFO
{
    RET_COMM_HDR                    hdr;

    DWORD                           dwFreeKB;
    DWORD                           dwTotalKB;
}
RET_GET_DISKINFO, *PRET_GET_DISKINFO;

typedef struct _RET_GET_APPLIST
{
    RET_COMM_HDR                    hdr;

    DWORD                           dwCount;
    APP_INFO                        AppInfo[1];
}
RET_GET_APPLIST, *PRET_GET_APPLIST;

typedef struct _RET_SCREEN_CAP
{
    RET_COMM_HDR                    hdr;

    BYTE                            szSuffix[MAX_FILE_SUFFIX_LEN];
}
RET_SCREEN_CAP, *PRET_SCREEN_CAP;

typedef struct _RET_DEL_FILE
{
    RET_COMM_HDR                    hdr;
}
RET_DEL_FILE, *PRET_DEL_FILE;

typedef struct _RET_FIND_ME
{
    RET_COMM_HDR                    hdr;
}
RET_FIND_ME, *PRET_FIND_ME;

#pragma pack()


#endif
