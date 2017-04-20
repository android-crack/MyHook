//
// Created by killer on 4/20/17.
//

#ifndef JLHOOK_HOOK_CY_H
#define JLHOOK_HOOK_CY_H

#include <stdlib.h>
#include <unistd.h>
#include <android/log.h>
#include <stdio.h>


#define TAG "Killer"

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

typedef enum {
    MONO_IMAGE_OK,
    MONO_IMAGE_ERROR_ERRNO,
    MONO_IMAGE_MISSING_ASSEMBLYREF,
    MONO_IMAGE_IMAGE_INVALID
} MonoImageOpenStatus;

void my_hook();
bool hookU3D();
void* get_module_base(int pid, const char* module_name);
void* get_remote_addr(int target_pid, const char* module_name, void* local_addr);

#endif //JLHOOK_HOOK_CY_H
