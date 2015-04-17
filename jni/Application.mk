#APP_ABI := all
#APP_ABI := armeabi armeabi-v7a x86 
APP_ABI := x86
APP_CFLAGS += -Wno-error=format-security
APP_PLATFORM := android-17
APP_STL := gnustl_static
APP_CPPFLAGS := -fexceptions