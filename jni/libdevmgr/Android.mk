LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := devmgr

LOCAL_EXPORT_CFLAGS := -D_ANDROID_
LOCAL_CFLAGS := $(LOCAL_EXPORT_CFLAGS)

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/inc
LOCAL_C_INCLUDES := $(LOCAL_EXPORT_C_INCLUDES)

LOCAL_EXPORT_LDLIBS := -llog
LOCAL_LDLIBS := $(LOCAL_EXPORT_LDLIBS)

LOCAL_SRC_FILES :=                  \
    comm/BufferItem.cpp             \
    comm/RingBuffer.cpp             \
    comm/MiscUtils.cpp              \
    net/TransInst.cpp               \
    net/TransSockInst.cpp           \
    net/TransCenter.cpp             \
    net/TransHandler.cpp            \
    net/TransStreamHandler.cpp      \
    net/CommonServer.cpp            \
    net/MsgClient.cpp               \

include $(BUILD_STATIC_LIBRARY)

