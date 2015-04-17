#include <StdAfx.h>
#include "DevTableMod.h"
#include "AppTableMod.h"
#include "CmdTableMod.h"
#include "MsgServer.h"
#include "MsgServerHandler.h"


MsgServerHandler::MsgServerHandler(TransInst *pTransInst)
    : TransStreamHandler(pTransInst)
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));
}

MsgServerHandler::~MsgServerHandler()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));
}

DWORD MsgServerHandler::Init(sem_t *pEndEvent)
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    TransStreamHandler::Init(pEndEvent);

    DevTableMod::Instance()->RegisterObserver(this);
    AppTableMod::Instance()->RegisterObserver(this);
    CmdTableMod::Instance()->RegisterObserver(this);

    return ERROR_SUCCESS;
}

void MsgServerHandler::Fini()
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    DevTableMod::Instance()->UnregisterObserver(this);
    AppTableMod::Instance()->UnregisterObserver(this);
    CmdTableMod::Instance()->UnregisterObserver(this);

    TransStreamHandler::Fini();
}

TransHandler *MsgServerHandler::Clone(TransInst *pTransInst)
{
    TransHandler  *pClone;

    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    pClone = new MsgServerHandler(pTransInst);

    return pClone;
}

void MsgServerHandler::OnStarted()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    TransStreamHandler::OnStarted();
}

void MsgServerHandler::OnStopped()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    TransStreamHandler::OnStopped();
}


void MsgServerHandler::Notify(DBMSG_CODE MsgCode, const char *szDevId)
{
    BufferItem  biSend;
    PPDU_DBMSG_BODY  pDbMsg;

    DBG(D_VERB, (L"%s: code(%d), dev(%hs)\n", FULL_FUNC_NAME, szDevId, MsgCode));

    biSend.Alloc(PDU_HEAD_RESERVE + sizeof(PDU_COMM_HDR) + sizeof(PDU_DBMSG_BODY));
    biSend.Reserve(PDU_HEAD_RESERVE + sizeof(PDU_COMM_HDR));

    pDbMsg = (PPDU_DBMSG_BODY)biSend.Put(sizeof(PDU_DBMSG_BODY));
    pDbMsg->dwDevNum = 1;
    strncpy((char*)pDbMsg->szDevId, szDevId, MAX_DEV_ID_LEN);

    SendPDU((BYTE)C_PDU_DBMSG, false, false, (DWORD)MsgCode, &biSend);
}

void MsgServerHandler::ProcessPDU(PPDU_COMM_HDR pHdr, PBYTE pBody)
{
    PPDU_DBMSG_BODY  pDbMsg;

    DBG(D_VERB, (L"%s: code(%d), err(%d), arg(%x), body(%d)\n", FULL_FUNC_NAME, pHdr->bCode, pHdr->bErr, pHdr->dwParam, pHdr->wBodySize));

    if ( pHdr->bCode == C_PDU_DBMSG )
    {
        assert(pHdr->wBodySize >= sizeof(PDU_DBMSG_BODY));
        pDbMsg = (PPDU_DBMSG_BODY)pBody;
        ProcessDbMsg((DBMSG_CODE)pHdr->dwParam, (char*)pDbMsg->szDevId[0]);
    }
}

void MsgServerHandler::ProcessDbMsg(DBMSG_CODE MsgCode, const char *szDevId)
{
    DBG(D_VERB, (L"%s: code(%d), dev(%hs)\n", FULL_FUNC_NAME, MsgCode, szDevId));

    switch ( MsgCode )
    {
        case C_DBMSG_CMDTAB_UPDATED:
            CmdTableMod::Instance()->OnCmdUpdated();
            break;

        case C_DBMSG_UPDATE_SYSINFO:
            DevTableMod::Instance()->UpdateSysInfo(szDevId);
            break;

        case C_DBMSG_UPDATE_BATTINFO:
            DevTableMod::Instance()->UpdateBattInfo(szDevId);
            break;

        case C_DBMSG_UPDATE_DISKINFO:
            DevTableMod::Instance()->UpdateDiskInfo(szDevId);
            break;

        case C_DBMSG_UPDATE_APPLIST:
            AppTableMod::Instance()->UpdateAppList(szDevId);
            break;

        case C_DBMSG_FIND_ME:
            DevTableMod::Instance()->FindMe(szDevId);
            break;

        default:
            DBG(D_WARN, (L"%s: unknown code(%d)\n", FULL_FUNC_NAME, MsgCode));
            assert(0);
            break;
    }
}
