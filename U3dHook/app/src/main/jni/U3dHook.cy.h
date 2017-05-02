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
#include "xxtea.h"

using namespace std;

#define LOG_TAG "U3dHook"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

struct _MonoImage {
    /*
     * The number of assemblies which reference this MonoImage though their 'image'
     * field plus the number of images which reference this MonoImage through their
     * 'modules' field, plus the number of threads holding temporary references to
     * this image between calls of mono_image_open () and mono_image_close ().
     */
    int   ref_count;
    void *raw_data_handle;
    char *raw_data;
    int raw_data_len;
};

typedef enum {
    MONO_IMAGE_OK,
    MONO_IMAGE_ERROR_ERRNO,
    MONO_IMAGE_MISSING_ASSEMBLYREF,
    MONO_IMAGE_IMAGE_INVALID
} MonoImageOpenStatus;

typedef int gboolean;

unsigned long getSoBase();

void* get_module_base(int pid, const char* module_name);
void* get_remote_addr(int target_pid, const char* module_name, void* local_addr);
bool saveFile(const void* addr, int len, const char *outFileName);
string getNextFilePath(const char *fileExt);
bool saveDllFile(int8_t *offset, int32_t *data_len, const char *outFileName);
bool saveAniDllFile(void *buf, size_t len, const char *outFileName);


#endif //U3DHOOK_U3DHOOK_CY_H
