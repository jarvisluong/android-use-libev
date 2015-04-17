#include <StdAfx.h>
#include "AdbCmd.h"


AdbCmd::AdbCmd()
#ifdef _WIN32_
    : _hChildProcess(NULL)
    , _hParentRd(INVALID_HANDLE_VALUE)
    , _hParentWr(INVALID_HANDLE_VALUE)
#endif
#ifdef _LINUX_
    : _hChildProcess(-1)
    , _hParentRd(-1)
    , _hParentWr(-1)
#endif
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));
}

AdbCmd::~AdbCmd()
{
    DBG(D_FUNC, (L"%s\n", FULL_FUNC_NAME));

    Cleanup();
}

DWORD AdbCmd::Execute()
{
    DWORD  dwRet;

    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    dwRet = BuildCmdline();
    if ( dwRet != ERROR_SUCCESS )
    {
        DBG(D_WARN, (L"%s: BuildCmdline failed: %d\n", FULL_FUNC_NAME, dwRet));
        return dwRet;
    }

    dwRet = CreateChildProcess();
    if ( dwRet != ERROR_SUCCESS )
    {
        DBG(D_WARN, (L"%s: CreateChildProcess failed: %d\n", FULL_FUNC_NAME, dwRet));
        return dwRet;
    }

    ProcessChildOutput();

    OnChildExit();

    Cleanup();

    ProcessResult();

    return ERROR_SUCCESS;
}

DWORD AdbCmd::GetResult(PBYTE pBuf, DWORD &dwSize)
{
    DWORD  dwBytes;

    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    dwBytes = dwSize;
    dwSize = _biResult.data_size();

    if ( (!pBuf) || (dwBytes < dwSize) )
    {
        DBG(D_WARN, (L"%s: result need size(%d)\n", FULL_FUNC_NAME, dwSize));
        return ERROR_NOT_ENOUGH_MEMORY;
    }

    memcpy(pBuf, _biResult.data(), dwSize);

    return ERROR_SUCCESS;
}

void AdbCmd::OnChildOutput()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));
}

void AdbCmd::OnChildExit()
{
    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));
}

void AdbCmd::ProcessResult()
{
    const char  *szOutput, *p;
    PRET_COMM_HDR  pHdr;

    szOutput = (const char*)_biOutput.data();
    DBG(D_INFO, (L"%s: output(%hs)\n", FULL_FUNC_NAME, szOutput));

    assert(_biResult.data_size() >= sizeof(RET_COMM_HDR));
    pHdr = (PRET_COMM_HDR)_biResult.data();

    pHdr->dwSize = _biResult.data_size();

    pHdr->dwResult = ERROR_NOT_SUPPORTED;
    for ( UINT i = 0; i < _countof(ADB_CMD_RET_MAP); i++ )
    {
        if ( MatchStringAnywhere(szOutput, ADB_CMD_RET_MAP[i].str, p) )
        {
            pHdr->dwResult = ADB_CMD_RET_MAP[i].dwCode;
            break;
        }
    }

    DBG(D_INFO, (L"%s: result(%d)\n", FULL_FUNC_NAME, pHdr->dwResult));
}


#ifdef _WIN32_
DWORD AdbCmd::CreateChildProcess()
{
    DWORD  dwRet;
    HANDLE  hChildRd, hChildWr;
    SECURITY_ATTRIBUTES sa;
    STARTUPINFOA  si;
    PROCESS_INFORMATION pi;

    DBG(D_VERB, (L"%s: cmd(%hs)\n", FULL_FUNC_NAME, _Cmdline.c_str()));

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    if ( !CreatePipe(&_hParentRd, &hChildWr, &sa, 0) )
    {
        dwRet = GetLastError();
        DBG(D_WARN, (L"%s: CreatePipe failed: %d\n", FULL_FUNC_NAME, dwRet));
        goto err1;
    }
    SetHandleInformation(_hParentRd, HANDLE_FLAG_INHERIT, 0);

    if ( !CreatePipe(&hChildRd, &_hParentWr, &sa, 0) )
    {
        dwRet = GetLastError();
        DBG(D_WARN, (L"%s: CreatePipe failed: %d\n", FULL_FUNC_NAME, dwRet));
        goto err2;
    }
    SetHandleInformation(_hParentWr, HANDLE_FLAG_INHERIT, 0);

    memset(&si, 0, sizeof(si));
    si.cb = sizeof(STARTUPINFO); 
    si.hStdInput = hChildRd;
    si.hStdOutput = hChildWr;
    si.hStdError = hChildWr;
    si.dwFlags |= STARTF_USESTDHANDLES;

    if ( !CreateProcessA(NULL, (LPSTR)_Cmdline.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi) )
    {
        dwRet = GetLastError();
        DBG(D_WARN, (L"%s: CreateProcess(%hs) failed: %d\n", FULL_FUNC_NAME, _Cmdline.c_str(), dwRet));
        goto err3;
    }
    DBG(D_INFO, (L"%s: cmd(%hs) run successfully\n", FULL_FUNC_NAME, _Cmdline.c_str()));

    CloseHandle(hChildRd);
    CloseHandle(hChildWr);

    CloseHandle(pi.hThread);
    _hChildProcess = pi.hProcess;
 
    return ERROR_SUCCESS;

err3:
    CloseHandle(hChildRd);
    CloseHandle(_hParentWr);
    _hParentWr = INVALID_HANDLE_VALUE;

err2:
    CloseHandle(hChildWr);
    CloseHandle(_hParentRd);
    _hParentRd = INVALID_HANDLE_VALUE;

err1:
    return dwRet;
}

void AdbCmd::ProcessChildOutput()
{
    BOOL  bRet;
    BYTE  buf[500];
    DWORD  dwBytes;

    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    _biOutput.Alloc(sizeof(buf));

    while ( true )
    {
        // assure dwBytes <= (sizeof(buf) - sizeof(char))
        bRet = ReadFile(_hParentRd, buf, sizeof(buf) - sizeof(char), &dwBytes, NULL);
        if ( (!bRet) || (dwBytes == 0) )
        {
            break;
        }
        buf[dwBytes] = 0;

        if ( _biOutput.tail_size() <= dwBytes )
        {
            _biOutput.Expand(sizeof(buf));
        }

        memcpy(_biOutput.Put(dwBytes), buf, dwBytes + 1);

        OnChildOutput();
    }

    _biOutput.Put(sizeof(char));
}

void AdbCmd::Cleanup()
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    if ( _hParentRd != INVALID_HANDLE_VALUE )
    {
        CloseHandle(_hParentRd);
        _hParentRd = INVALID_HANDLE_VALUE;
    }

    if ( _hParentWr != INVALID_HANDLE_VALUE )
    {
        CloseHandle(_hParentWr);
        _hParentWr = INVALID_HANDLE_VALUE;
    }

    if ( _hChildProcess )
    {
        CloseHandle(_hChildProcess);
        _hChildProcess = NULL;
    }
}
#endif

#ifdef _LINUX_
DWORD AdbCmd::CreateChildProcess()
{
    DWORD  dwRet;
    int  fdIn[2], fdOut[2];

    DBG(D_INFO, (L"%s: cmd(%hs)\n", FULL_FUNC_NAME, _Cmdline.c_str()));

    if ( pipe(fdIn) )
    {
        dwRet = GetLastError();
        DBG(D_WARN, (L"%s: pipe failed: %d\n", FULL_FUNC_NAME, dwRet));
        goto err1;
    }

    if ( pipe(fdOut) )
    {
        dwRet = GetLastError();
        DBG(D_WARN, (L"%s: pipe failed: %d\n", FULL_FUNC_NAME, dwRet));
        goto err2;
    }

    _hChildProcess = fork();
    if ( _hChildProcess < 0 )
    {
        dwRet = GetLastError();
        DBG(D_WARN, (L"%s: fork failed: %d\n", FULL_FUNC_NAME, dwRet));
        goto err3;
    }

    if ( !_hChildProcess )
    {
        close(fdIn[1]);
        close(fdOut[0]);
        dup2(fdIn[0], STDIN_FILENO);
        dup2(fdOut[1], STDOUT_FILENO);
        dup2(fdOut[1], STDERR_FILENO);
        execl("/bin/sh", "sh", "-c", _Cmdline.c_str(), NULL);
        perror("exec failed");
        exit(0);
    }
    else
    {
        close(fdIn[0]);
        close(fdOut[1]);
        _hParentRd = fdOut[0];
        _hParentWr= fdIn[1];
    }

    return ERROR_SUCCESS;

err3:
    close(fdOut[0]);
    close(fdOut[1]);

err2:
    close(fdIn[0]);
    close(fdIn[1]);

err1:
    return dwRet;
}

void AdbCmd::ProcessChildOutput()
{
    BYTE  buf[500];
    ssize_t  nBytes;

    DBG(D_VERB, (L"%s\n", FULL_FUNC_NAME));

    _biOutput.Alloc(sizeof(buf));

    while ( true )
    {
        // assure dwBytes <= (sizeof(buf) - sizeof(char))
        nBytes = read(_hParentRd, buf, sizeof(buf) - sizeof(char));
        if ( nBytes <= 0 )
        {
            break;
        }
        buf[nBytes] = 0;

        if ( _biOutput.tail_size() <= (UINT)nBytes )
        {
            _biOutput.Expand(sizeof(buf));
        }

        memcpy(_biOutput.Put(nBytes), buf, nBytes + 1);

        OnChildOutput();
    }

    _biOutput.Put(sizeof(char));
}

void AdbCmd::Cleanup()
{
    DBG(D_INFO, (L"%s\n", FULL_FUNC_NAME));

    if ( _hParentRd != -1 )
    {
        close(_hParentRd);
        _hParentRd = -1;
    }

    if ( _hParentWr != -1 )
    {
        close(_hParentWr);
        _hParentWr = -1;
    }

    if ( _hChildProcess != -1 )
    {
        waitpid(_hChildProcess, NULL, 0);
        _hChildProcess = -1;
    }

}
#endif

