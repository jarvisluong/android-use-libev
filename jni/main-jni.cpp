#include <StdAfx.h>
#include <jni.h>
#include <android/native_window_jni.h>
#include "RingBuffer.h"
#include "TransSockInst.h"
#include "TransCenter.h"
#include "MsgClient.h"


typedef struct _MSG_ITEM
{
    DBMSG_CODE                  MsgCode;
    char                        szDevId[MAX_DEV_ID_LEN];
}
MSG_ITEM, *PMSG_ITEM;


static volatile bool            gbQuit;
static pthread_t                ghThread;
static sem_t                    ghEvent;
static RingBuffer               gMsgRing(L"msg", 100);

static JavaVM                 * gpJavaVM;
static jobject                  gjThiz;
static jmethodID                gjMethodOnEvent;


class MyMsgClient : public MsgClient
{
public:
    static MyMsgClient *Instance();
    static void Destroy();

private:
    static MyMsgClient        * _instance;

protected:
    MyMsgClient();
    virtual ~MyMsgClient();

protected:
    virtual void OnStarted();
    virtual void OnStopped();
    virtual void ProcessDbMsg(DBMSG_CODE MsgCode, const char *szDevId);
};

MyMsgClient *MyMsgClient::_instance = NULL;

MyMsgClient *MyMsgClient::Instance()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    if ( !_instance )
    {
        DBG(D_VERB, (L"%s: %ls\n", FULL_FUNC_NAME, L"CreateInstance"));
        _instance = new MyMsgClient();
    }
    return _instance;
}

void MyMsgClient::Destroy()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    delete _instance;
    _instance = NULL;
}

MyMsgClient::MyMsgClient()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));
}

MyMsgClient::~MyMsgClient()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));
}

void MyMsgClient::OnStarted()
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));
    ProcessDbMsg((DBMSG_CODE)0x1000, "");
}

void MyMsgClient::OnStopped()
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    ProcessDbMsg((DBMSG_CODE)0x1001, "");
}

void MyMsgClient::ProcessDbMsg(DBMSG_CODE MsgCode, const char *szDevId)
{
    PMSG_ITEM  pMsg;
    bool  bSignal;

    DBG(D_INFO, (L"%s: code(%x), dev(%hs)\n", FULL_FUNC_NAME, MsgCode, szDevId));

    bSignal = gMsgRing.IsEmpty();

    if ( !gMsgRing.IsFull() )
    {
        pMsg = new MSG_ITEM;
        pMsg->MsgCode = MsgCode;
        strncpy(pMsg->szDevId, szDevId, MAX_DEV_ID_LEN);
        gMsgRing.PushAtBack((PBYTE)pMsg);
    }

    if ( bSignal )
    {
        sem_post(&ghEvent);
    }
}


static void *WorkThreadProc(void *arg)
{
    JNIEnv  *env;
    JavaVMAttachArgs  args;
    jclass jcls;
    PMSG_ITEM  pMsg;

    DBG(D_INFO, (L"%s: enter\n", FULL_FUNC_NAME));

    args.version = JNI_VERSION_1_4;
    args.name = "NativeThread";
    args.group = NULL;

    gpJavaVM->AttachCurrentThread(&env, &args);

    while ( !gbQuit )
    {
        sem_wait(&ghEvent);

        while ( !gMsgRing.IsEmpty() )
        {
            pMsg = (PMSG_ITEM)gMsgRing.PopAtFront();
            DBG(D_INFO, (L"%s: OnMsg: code(%d), dev(%hs)\n", FULL_FUNC_NAME, pMsg->MsgCode, pMsg->szDevId));
            jstring jstrDevId = env->NewStringUTF(pMsg->szDevId);
            env->CallVoidMethod(gjThiz, gjMethodOnEvent, pMsg->MsgCode, jstrDevId);
            env->DeleteLocalRef(jstrDevId);
        }
    }

    gpJavaVM->DetachCurrentThread();

    DBG(D_INFO, (L"%s: leave\n", FULL_FUNC_NAME));

    return NULL;
}


extern "C" void Java_com_intel_tabletmanager_services_SyncService_nativeInit(JNIEnv *env, jobject thiz)
{
    jclass  jClass;

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    TransSockInst::Startup();
    TransCenter::Instance()->Load();

    gjThiz = env->NewGlobalRef(thiz);

    jClass = env->FindClass("com/intel/tabletmanager/services/SyncService");
    gjMethodOnEvent = env->GetMethodID(jClass, "OnEvent", "(ILjava/lang/String;)V");
    env->DeleteLocalRef(jClass);

    DBG(D_INFO, (L"%s: this(0x%x), OnEvent(0x%x)\n", FULL_FUNC_NAME, gjThiz, gjMethodOnEvent));

    sem_init(&ghEvent, 0, 0);
    gbQuit = false;
    pthread_create(&ghThread, NULL, WorkThreadProc, NULL);
}

extern "C" void Java_com_intel_tabletmanager_services_SyncService_nativeQuit(JNIEnv *env, jobject thiz)
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    if ( IS_PTH_VALID(ghThread) )
    {
        gbQuit = true;
        sem_post(&ghEvent);
        pthread_join(ghThread, NULL);
        SET_PTH_INVALID(ghThread);
    }
    sem_destroy(&ghEvent);

    env->DeleteGlobalRef(gjThiz);

    MyMsgClient::Instance()->Unload();
    TransCenter::Instance()->Unload();
    TransSockInst::Cleanup();

    MyMsgClient::Destroy();
    TransCenter::Destroy();
}

extern "C" void Java_com_intel_tabletmanager_services_SyncService_nativeConnect(JNIEnv *env, jobject thiz, jstring ip, int port)
{
    TRANS_ADDR  addr;
    const char  *szIp;

    szIp = env->GetStringUTFChars(ip, NULL);
    DBG(D_INFO, (L"%s: Ip(%hs): port(%d)\n", FULL_FUNC_NAME, szIp, port));

    addr.bType = ADDR_TYPE_IPV4;
    addr.IPv4Addr.bStream = 1;
    addr.IPv4Addr.dwIP = inet_addr(szIp);
    addr.IPv4Addr.wPort = htons(MSG_SERVER_LISTEN_PORT);
    MyMsgClient::Instance()->Load(&addr);

    env->ReleaseStringUTFChars(ip, szIp);
}

extern "C" void Java_com_intel_tabletmanager_services_SyncService_CmdUpdated(JNIEnv *env, jobject thiz)
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    MyMsgClient::Instance()->CmdUpdated();
}

extern "C" void Java_com_intel_tabletmanager_services_SyncService_UpdateSysInfo(JNIEnv *env, jobject thiz, jstring devid)
{
    const char  *szDevId;

    szDevId = env->GetStringUTFChars(devid, NULL);
    DBG(D_INFO, (L"%s: dev(%hs): port(%d)\n", FULL_FUNC_NAME, szDevId));

    MyMsgClient::Instance()->UpdateSysInfo(szDevId);

    env->ReleaseStringUTFChars(devid, szDevId);
}

extern "C" void Java_com_intel_tabletmanager_services_SyncService_UpdateBattInfo(JNIEnv *env, jobject thiz, jstring devid)
{
    const char  *szDevId;

    szDevId = env->GetStringUTFChars(devid, NULL);
    DBG(D_INFO, (L"%s: dev(%hs): port(%d)\n", FULL_FUNC_NAME, szDevId));

    MyMsgClient::Instance()->UpdateBattInfo(szDevId);

    env->ReleaseStringUTFChars(devid, szDevId);
}

extern "C" void Java_com_intel_tabletmanager_services_SyncService_UpdateDiskInfo(JNIEnv *env, jobject thiz, jstring devid)
{
    const char  *szDevId;

    szDevId = env->GetStringUTFChars(devid, NULL);
    DBG(D_INFO, (L"%s: dev(%hs): port(%d)\n", FULL_FUNC_NAME, szDevId));

    MyMsgClient::Instance()->UpdateDiskInfo(szDevId);

    env->ReleaseStringUTFChars(devid, szDevId);
}

extern "C" void Java_com_intel_tabletmanager_services_SyncService_UpdateAppList(JNIEnv *env, jobject thiz, jstring devid)
{
    const char  *szDevId;

    szDevId = env->GetStringUTFChars(devid, NULL);
    DBG(D_INFO, (L"%s: dev(%hs): port(%d)\n", FULL_FUNC_NAME, szDevId));

    MyMsgClient::Instance()->UpdateAppList(szDevId);

    env->ReleaseStringUTFChars(devid, szDevId);
}

extern "C" void Java_com_intel_tabletmanager_services_SyncService_FindMe(JNIEnv *env, jobject thiz, jstring devid)
{
    const char  *szDevId;

    szDevId = env->GetStringUTFChars(devid, NULL);
    DBG(D_INFO, (L"%s: dev(%hs): port(%d)\n", FULL_FUNC_NAME, szDevId));

    MyMsgClient::Instance()->FindMe(szDevId);

    env->ReleaseStringUTFChars(devid, szDevId);
}

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    DBG(D_LOG, (L"%s\n", FULL_FUNC_NAME));

    gpJavaVM = vm;

    return JNI_VERSION_1_4;
}

