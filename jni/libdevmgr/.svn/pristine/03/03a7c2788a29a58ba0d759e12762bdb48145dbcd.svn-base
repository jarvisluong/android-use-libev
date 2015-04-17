#include <StdAfx.h>
#ifdef _WIN32_
#include <Dbt.h>
#endif
#ifdef _LINUX_
#include <linux/netlink.h>
#endif
#include "MtpDeviceMon.h"
#include "AdbDeviceMon.h"
#include "UsbDeviceMon.h"


#ifdef _WIN32_
#define  WND_CLASSNAME          TEXT("UsbDevMonWnd")
#define  WM_DO_CLOSE            (WM_APP + 1)
#endif

UsbDeviceMon *UsbDeviceMon::_instance = NULL;

UsbDeviceMon *UsbDeviceMon::Instance()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    if ( !_instance )
    {
        DBG(D_VERB, (L"%s: %ls\n", FULL_FUNC_NAME, L"CreateInstance"));
        _instance = new UsbDeviceMon();
    }
    return _instance;
}

void UsbDeviceMon::Destroy()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    delete _instance;
    _instance = NULL;
}

UsbDeviceMon::UsbDeviceMon()
    : _bQuit(false)
#ifdef _WIN32_
    , _hMainWnd(NULL)
    , _hDevNotify(NULL)
#endif
#ifdef _LINUX_
    , _nlfd(INVALID_SOCKET)
#endif
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    SET_PTH_INVALID(_hMonThread);

#ifdef _WIN32_
    sem_init(&_hWakeEvent, 0, 0);

    SET_PTH_INVALID(_hWndThread);
#endif
}

UsbDeviceMon::~UsbDeviceMon()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

#ifdef _WIN32_
    assert(!IS_PTH_VALID(_hWndThread));

    sem_destroy(&_hWakeEvent);
#endif

    assert(!IS_PTH_VALID(_hMonThread));
}

DWORD UsbDeviceMon::Load()
{
    DWORD  dwRet;
    int  ret;

    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

#ifdef _LINUX_
    dwRet = CreateNetLink();
    if ( dwRet != ERROR_SUCCESS )
    {
        DBG(D_ERROR, (L"%s: CreateNetLink failed: %d\n", FULL_FUNC_NAME, dwRet));
        goto err;
    }    
#endif

    ret = pthread_create(&_hMonThread, NULL, MonThreadProc, this);
    if ( ret )
    {
        dwRet = GetLastError();
        DBG(D_ERROR, (L"%s: create thread(Mon) failed: %d\n", FULL_FUNC_NAME, dwRet));
        goto err;
    }

#ifdef _WIN32_
    ret = pthread_create(&_hWndThread, NULL, WndThreadProc, this);
    if ( ret )
    {
        dwRet = GetLastError();
        DBG(D_ERROR, (L"%s: create thread(Wnd) failed: %d\n", FULL_FUNC_NAME, dwRet));
        goto err;
    }
#endif

    return ERROR_SUCCESS;

err:
    Unload();

    return dwRet;
}

void UsbDeviceMon::Unload()
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

#ifdef _WIN32_
    if ( _hMainWnd )
    {
        PostMessage(_hMainWnd, WM_DO_CLOSE, 0, 0);
        _hMainWnd = NULL;
    }

    if ( IS_PTH_VALID(_hWndThread) )
    {
        pthread_join(_hWndThread, NULL);
        SET_PTH_INVALID(_hWndThread);
    }
#endif

    _bQuit = true;

#ifdef _WIN32_
    sem_post(&_hWakeEvent);
#endif

#ifdef _LINUX_
    if ( _nlfd != INVALID_SOCKET )
    {
        closesocket(_nlfd);
        _nlfd = INVALID_SOCKET;
    }
#endif

    if ( IS_PTH_VALID(_hMonThread) )
    {
        pthread_join(_hMonThread, NULL);
        SET_PTH_INVALID(_hMonThread);
    }
}

DWORD UsbDeviceMon::MonThread()
{
    DBG(D_INFO, (L"%s: enter\n", FULL_FUNC_NAME));

    while ( !_bQuit )
    {
        MtpDeviceMon::Instance()->OnChanged();
        AdbDeviceMon::Instance()->OnChanged();

#ifdef _WIN32_
        sem_wait(&_hWakeEvent);
#endif
#ifdef _LINUX_
        while ( !_bQuit )
        {
            if ( WaitUEvent() != WAIT_TIMEOUT )
            {
                break;
            }
        }
#endif
    }

    DBG(D_INFO, (L"%s: leave\n", FULL_FUNC_NAME));

    return 0;
}

void *UsbDeviceMon::MonThreadProc(void *arg)
{
    UsbDeviceMon  *pThis = static_cast<UsbDeviceMon*>(arg);

    pThis->MonThread();

    return NULL;
}

#ifdef _WIN32_
void UsbDeviceMon::OnWMCreate(HWND hWnd)
{
    DEV_BROADCAST_DEVICEINTERFACE  filter;

    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    memset(&filter, 0, sizeof(filter) );
    filter.dbcc_size = sizeof(filter);
    filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    filter.dbcc_classguid =  AdbDevIntfGuid;//UsbDevIntfGuid;
    _hDevNotify = RegisterDeviceNotification(hWnd, &filter, DEVICE_NOTIFY_WINDOW_HANDLE);
}

void UsbDeviceMon::OnWMDestroy(HWND hWnd)
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    if ( _hDevNotify )
    {
        UnregisterDeviceNotification(_hDevNotify);
        _hDevNotify = NULL;
    }
}

void UsbDeviceMon::OnWMDeviceChange(UINT nEventType, DWORD_PTR dwData)
{
    PDEV_BROADCAST_HDR  pHdr;
    PDEV_BROADCAST_DEVICEINTERFACE  pDevIntf;

    DBG(D_VERB, (L"%s: type(%x)\n", FULL_FUNC_NAME, nEventType));

    if ( (nEventType != DBT_DEVICEARRIVAL) && (nEventType != DBT_DEVICEREMOVECOMPLETE) )
    {
        return;
    }

    pHdr = (PDEV_BROADCAST_HDR)dwData;
    if ( pHdr->dbch_devicetype != DBT_DEVTYP_DEVICEINTERFACE )
    {
        return;
    }

    pDevIntf = (PDEV_BROADCAST_DEVICEINTERFACE)dwData;
    if ( pDevIntf->dbcc_classguid == AdbDevIntfGuid )
    {
        sem_post(&_hWakeEvent);
    }
}

HWND UsbDeviceMon::CreateMainWindow()
{
    WNDCLASSEX  wcex;
    ATOM  hClass;
    HWND  hWnd;

    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = MainWndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = NULL;
    wcex.hIcon          = NULL;
    wcex.hCursor        = NULL;
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = WND_CLASSNAME;
    wcex.hIconSm        = NULL;

    hClass = RegisterClassEx(&wcex);

    hWnd = CreateWindow(WND_CLASSNAME, L"UsbDeviceMon Window",
                            WS_POPUP,
                            0, 0,
                            0, 0,
                            NULL, NULL, NULL, this);

    ShowWindow(hWnd, SW_HIDE);
    UpdateWindow(hWnd);

    return hWnd;
}

LRESULT CALLBACK UsbDeviceMon::MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    UsbDeviceMon  *pThis;

    DBG(D_FUNC, (L"%s: win(%x), msg(%d)\n", FULL_FUNC_NAME, hWnd, message));

    pThis = (UsbDeviceMon*)GetWindowLong(hWnd, GWL_USERDATA);

    switch (message)
    {
        case WM_CREATE:
        {
            CREATESTRUCT  *pCS = (CREATESTRUCT*)lParam;
            pThis = (UsbDeviceMon*)pCS->lpCreateParams;
            SetWindowLong(hWnd, GWL_USERDATA, (LONG)pThis);
            pThis->OnWMCreate(hWnd);
            break;
        }

        case WM_DESTROY:
            pThis->OnWMDestroy(hWnd);
            PostQuitMessage(0);
            break;

        case WM_DEVICECHANGE:
            pThis->OnWMDeviceChange(wParam, lParam);
            break;

        case WM_DO_CLOSE:
            DestroyWindow(hWnd);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

DWORD UsbDeviceMon::WndThread()
{
    MSG  msg;

    DBG(D_INFO, (L"%s: enter\n", FULL_FUNC_NAME));

    _hMainWnd = CreateMainWindow();

    while ( GetMessage(&msg, NULL, 0, 0) )
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DBG(D_INFO, (L"%s: leave\n", FULL_FUNC_NAME));

    return 0;
}

void *UsbDeviceMon::WndThreadProc(void *arg)
{
    UsbDeviceMon  *pThis = static_cast<UsbDeviceMon*>(arg);

    pThis->WndThread();

    return NULL;
}
#endif

#ifdef _LINUX_
DWORD UsbDeviceMon::CreateNetLink()
{
    DWORD  dwRet;
    struct sockaddr_nl addr;

    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    _nlfd = socket(AF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);
    if ( _nlfd == INVALID_SOCKET )
    {
        dwRet = WSAGetLastError();
        DBG(D_WARN, (L"%s: create socket failed: %d\n", FULL_FUNC_NAME, dwRet));
        goto err1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_pid = 0;//getpid();//0;
    addr.nl_groups = NETLINK_KOBJECT_UEVENT;
    if ( bind(_nlfd, (struct sockaddr*)&addr, sizeof(addr)) )
    {
        dwRet = WSAGetLastError();
        DBG(D_WARN, (L"%s: bind socket failed: %d\n", FULL_FUNC_NAME, dwRet));
        goto err2;
    }

    _biUEvent.Alloc(8000);

    return ERROR_SUCCESS;

err2:
    closesocket(_nlfd);
    _nlfd = INVALID_SOCKET;
    
err1:
    return dwRet;
}

DWORD UsbDeviceMon::WaitUEvent()
{
    DWORD  dwRet;
    int  ret;
    ssize_t  len;
    fd_set  rd_fds;
    timeval  tv;

    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    _biUEvent.Reset();

    FD_ZERO(&rd_fds);
    FD_SET(_nlfd, &rd_fds);
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    ret = select(_nlfd + 1, &rd_fds, NULL, NULL, &tv);

    if ( ret == 0 )
    {
        return WAIT_TIMEOUT;
    }

    if ( ret == SOCKET_ERROR )
    {
        dwRet = WSAGetLastError();
        DBG(D_ERROR, (L"%s: select failed: %d\n", FULL_FUNC_NAME, dwRet));
        return dwRet;
    }

    len = recv(_nlfd, _biUEvent.tail(), _biUEvent.tail_size(), 0);
    if ( len < 0 )
    {
        dwRet = GetLastError();
        DBG(D_ERROR, (L"%s: recv failed: %d\n", FULL_FUNC_NAME, dwRet));
        return dwRet;
    }
    _biUEvent.Put(len);

    DBG(D_INFO, (L"%s: len(%d), msg(%hs)\n", FULL_FUNC_NAME, len, _biUEvent.data()));

    return ERROR_SUCCESS;
}
#endif
