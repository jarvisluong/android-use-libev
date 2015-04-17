/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <string.h>
#include <jni.h>
#include <pthread.h>
#include <android/log.h>
#include "ev.h"
#include "misc.h"
#include "global.h"
#include "list.h"
#include "s_process.h"
#include "server.h"
#include "bcast_server.h"

/* This is a trivial JNI example where we use a native method
 * to return a new VM String. See the corresponding Java source
 * file located at:
 *
 *   apps/samples/hello-jni/project/src/com/example/hellojni/HelloJni.java
 */

JavaVM 			*gpJavaVM;
jobject 		gObject;
jmethodID       gOnEventID;

pthread_t		bcPID;
pthread_t 		mainPID;
pthread_t		sndPID;
pthread_t		rcvPID;

extern int main();
extern void exit_server_loop();

void init_server_thread()
{
	enter_func();

	create_bc_thread();

	create_main_thread();

	pthread_mutex_init(&rcv_list_mutex, NULL);
	INIT_LIST_HEAD(&rcv_list);

	pthread_mutex_init(&snd_list_mutex, NULL);
	INIT_LIST_HEAD(&snd_list);

	pthread_cond_init(&snd_cond, 0);
	pthread_cond_init(&rcv_cond, 0);

	create_rcv_thread();
	create_snd_thread();
}


void exit_server_thread()
{
	enter_func();
	exit_all_thread();

	DBG(D_INFO, "wait other threads to stop");
	pthread_join(sndPID, NULL);
	DBG(D_INFO, "snd thread exit");
	pthread_join(rcvPID, NULL);
	DBG(D_INFO, "rcv thread exit");

	pthread_mutex_destroy(&snd_list_mutex);
	pthread_mutex_destroy(&rcv_list_mutex);

	ev_async_send(loop, &main_async_watcher);

	pthread_join(mainPID, NULL);
	DBG(D_INFO, "main thread exit");
	pthread_mutex_destroy(&client_list_mutex);

	ev_async_send(bc_loop, &bc_async_watcher);
	pthread_join(bcPID, NULL);
	DBG(D_INFO, "main thread exit");

	DBG(D_INFO, "all 4 threads has exit");


}
void create_bc_thread()
{
	enter_func();
	int ret;

	ret = pthread_create(&bcPID, NULL, bc_main, NULL);
	if (ret) {
		perror("pthread_create");
		exit(1);
	}
}
void create_main_thread()
{
	enter_func();
	int ret;

	ret = pthread_create(&mainPID, NULL, main, NULL);
	if (ret) {
		perror("pthread_create");
		exit(1);
	}
}

void create_rcv_thread()
{
	enter_func();
	pthread_t pid;
	int ret;

	ret = pthread_create(&rcvPID, NULL, handle_rcv_pkt, NULL);
	if (ret) {
		perror("pthread_create");
		exit(1);
	}
}

void create_snd_thread()
{
	enter_func();
	pthread_t pid;
	int ret;

	ret = pthread_create(&sndPID, NULL, handle_snd_pkt, NULL);
	if (ret) {
		perror("pthread_create");
		exit(1);
	}
}

jint server_init(JNIEnv* env, jobject thiz)
{
	enter_func();
	gObject = (*env)->NewGlobalRef(env, thiz);

	init_server_thread();

}
jint server_exit(JNIEnv* env, jobject thiz)
{
	enter_func();
	exit_server_thread();

	(*env)->DeleteGlobalRef(env, gObject);

}
jint send_msg( JNIEnv* env, jobject thiz, jstring msg )
{

}

void on_msg_received(int code, char * msg) {
	JNIEnv *env;

	enter_func();

	(*gpJavaVM)->AttachCurrentThread(gpJavaVM, &env, NULL);

    jstring jmsg = (*env)->NewStringUTF(env, msg);
    (*env)->CallVoidMethod(env, gObject, gOnEventID, code, jmsg);
    (*env)->DeleteLocalRef(env, jmsg);
    (*gpJavaVM)->DetachCurrentThread(gpJavaVM);
}

static const  JNINativeMethod  gMethods[] = {
    {"server_init_native", "()I", (void*)server_init},
    {"server_exit_native", "()I", (void*)server_exit},
    {"send_msg_native", "(Ljava/lang/String;)I", (void*)send_msg},
//    {"getVal_native", "()I", (void*)hello_getVal},
   };

jint register_native_methods(JNIEnv *env, const char *clsname, const JNINativeMethod *gMethods, int nums)
{
	enter_func();
    jclass clazz = (*env)->FindClass(env, clsname);
    if (clazz == NULL) {
        return JNI_FALSE;
    }
    if ((*env)->RegisterNatives(env, clazz, gMethods, nums) < 0) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

const char *class_path = "com/example/zzz/NetService";

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
	JNIEnv *env = NULL;
	jint ret = -1;

    enter_func();
    gpJavaVM = vm;

    if ((*vm)->GetEnv(vm, (void**)&env, JNI_VERSION_1_4) != JNI_OK) {
    	DBG(D_INFO, "GetEvn Failed\n");
    	return ret;
    }

    jclass clazz = (*env)->FindClass(env, class_path);
    gOnEventID = (*env)->GetMethodID(env, clazz, "onEvent", "(ILjava/lang/String;)V");

    ret = register_native_methods(env, class_path, gMethods, sizeof(gMethods)/sizeof(gMethods[0]));

    return JNI_VERSION_1_4;
}
