#if defined(_WINCE_)
#define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA
#include <windows.h>
#endif

#if defined(_WIN32_)
#define _CRTDBG_MAP_ALLOC
#define _CRT_SECURE_NO_WARNINGS
#define STRSAFE_NO_DEPRECATE
#include <windows.h>
#include <io.h>
#endif

#if defined(_ANDROID_)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

#if defined(_LINUX_)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

#include <pthread.h>
#include <semaphore.h>
#include <assert.h>
#include <string>
#include <list>
#include <queue>
#include <map>
#include <algorithm>

#if defined(_WINCE_) || defined(_WIN32_)
#include "win32_dep.h"
#endif

#if defined(_ANDROID_) || defined(_LINUX_)
#include "linux_dep.h"
#endif

#include "GlobalDef.h"
#include "MiscUtils.h"

using namespace std;
