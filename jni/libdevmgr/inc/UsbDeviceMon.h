#ifndef _USB_DEVICE_MON_H
#define _USB_DEVICE_MON_H


class UsbDeviceMon
{
public:
    static UsbDeviceMon *Instance();
    static void Destroy();

private:
    static UsbDeviceMon       * _instance;

protected:
    UsbDeviceMon();
    ~UsbDeviceMon();

public:
    DWORD Load();
    void Unload();

private:
    DWORD MonThread();
    static void *MonThreadProc(void *arg);

#ifdef _WIN32_
    void OnWMCreate(HWND hWnd);
    void OnWMDestroy(HWND hWnd);
    void OnWMDeviceChange(UINT nEventType, DWORD_PTR dwData);

    HWND CreateMainWindow();
    static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    DWORD WndThread();
    static void *WndThreadProc(void *arg);
#endif

#ifdef _LINUX_
    DWORD CreateNetLink();
    DWORD WaitUEvent();
#endif

private:
    volatile bool               _bQuit;
    pthread_t                   _hMonThread;

#ifdef _WIN32_
    sem_t                       _hWakeEvent;

    pthread_t                   _hWndThread;
    HWND                        _hMainWnd;
    HDEVNOTIFY                  _hDevNotify;
#endif

#ifdef _LINUX_
    int                         _nlfd;
    BufferItem                  _biUEvent;
#endif
};


#endif

