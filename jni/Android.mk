# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := hello
LOCAL_SRC_FILES := hello-jni.c src/server.c src/s_process.c \
					src/bcast_server.c
					
ifeq ($(TARGET_ARCH) ,x86) 
	LOCAL_SHARED_LIBRARIES := ev-x86
else
	LOCAL_SHARED_LIBRARIES := ev-arm
endif
LOCAL_LDLIBS+= -L$(SYSROOT)/usr/lib -llog
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_CFLAGS := -D_ANDROID_
include $(BUILD_SHARED_LIBRARY)

ifeq ($(TARGET_ARCH) ,x86) 
	include $(CLEAR_VARS)
	LOCAL_MODULE := ev-x86
	LOCAL_SRC_FILES := $(LOCAL_PATH)/prebuild/libev-x86.so
	include $(PREBUILT_SHARED_LIBRARY)
else
	include $(CLEAR_VARS) 
	LOCAL_MODULE := ev-arm
	LOCAL_SRC_FILES := $(LOCAL_PATH)/prebuild/libev-arm.so
	include $(PREBUILT_SHARED_LIBRARY)
endif


#---------------------tablet cart

include $(CLEAR_VARS)

ifeq ($(TC), 1)
	LOCAL_MODULE    := tabletcar
	LOCAL_LDLIBS    := -landroid
	LOCAL_SHARED_LIBRARIES :=
	LOCAL_STATIC_LIBRARIES := devmgr
	LOCAL_SRC_FILES := main-jni.cpp
	
	include $(BUILD_SHARED_LIBRARY)
	include $(LOCAL_PATH)/libdevmgr/Android.mk
endif