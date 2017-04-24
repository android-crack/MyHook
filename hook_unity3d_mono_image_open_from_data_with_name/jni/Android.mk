LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CPPFLAGS += -std=c++0x -frtti -fexceptions
LOCAL_CPP_FEATURES += rtti exceptions

LOCAL_MODULE    := hook_unity3d_mono_image_open_from_data_with_name.cy
LOCAL_SRC_FILES := hook_unity3d_mono_image_open_from_data_with_name.cpp
LOCAL_LDLIBS:= -L$(LOCAL_PATH) -lsubstrate-dvm -llog -lsubstrate
include $(BUILD_SHARED_LIBRARY)