LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
APP_PLATFORM := android-19
LOCAL_MODULE    := LMDLoader
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_SRC_FILES := Loader.c
LOCAL_CFLAGS    := -fvisibility=hidden
LOCAL_LDLIBS := -llog -lz
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
TARGET_PLATFORM := android-3
LOCAL_MODULE    := LMDAnti
LOCAL_C_INCLUDES := $(LOCAL_PATH)
#LOCAL_C_INCLUDES += /opt/android/ndk/sources/cxx-stl/stlport/stlport

LOCAL_C_INCLUDES += $(LOCAL_PATH)/detect

LOCAL_SRC_FILES := detect/LMDAnti.cpp \
                   detect/CheckBinaryModification.c \
                   detect/CheckDebug.cpp \
                   detect/CheckRoot.c \
                   detect/CheckSpeedHack.c

LOCAL_CFLAGS := -D_STLP_USE_NO_IOSTREAMS -D_STLP_USE_MALLOC #-fvisibility=hidden
LOCAL_LDLIBS    := -llog -Wl,-init=init
APP_PLATFORM    := android-19
APP_ABI         := armeabi
LOCAL_SHARED_LIBRARIES := liblog libcutils
include $(BUILD_SHARED_LIBRARY)