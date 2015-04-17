#ifndef _MISC_H_
#define _MISC_H_

#define	D_ASSERT	1
#define	D_ERROR		1
#define	D_WARN		1
#define	D_INFO		1
#define	D_DEBUG		1

#include <stdio.h>
#include <jni.h>
#include <android/log.h>

#if defined(_ANDROID_)
#define TAG     "Native"
#define DBG(cond,...)   \
    do {\
        if(cond){\
            __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__);\
        }\
    }while(0)
#else
#define DBG(cond,...) \
    do {\
        if(cond){\
            printf(__VA_ARGS__);\
        }\
    }while(0)
#endif
#define MAX_STRING_LEN  256

#define func_name() DBG(D_INFO, "%s\n", __func__)
#define enter_func() DBG(D_INFO, "enter func --> %s\n", __func__)
#define exit_func() DBG(D_INFO, "exit func  -->%s\n", __func__)

#endif
