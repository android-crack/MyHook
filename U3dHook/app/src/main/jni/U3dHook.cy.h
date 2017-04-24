//
// Created by Administrator on 2017/4/24 0024.
//

#ifndef U3DHOOK_U3DHOOK_CY_H
#define U3DHOOK_U3DHOOK_CY_H

#include <android/log.h>
#include "substrate.h"
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string>

#define LOG_TAG "U3dHook"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

struct _MonoImage {
    int ret_count;
    void *raw_data_handle;
    char *raw_data;
    int raw_data_len;
};

void* get_module_base(int pid, const char* module_name);
void* get_remote_addr(int target_pid, const char* module_name, void* local_addr);

#endif //U3DHOOK_U3DHOOK_CY_H
