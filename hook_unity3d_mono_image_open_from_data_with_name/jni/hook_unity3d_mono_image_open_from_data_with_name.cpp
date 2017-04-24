#include <android/log.h>
#include "substrate.h"
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "linker.h"


// cxx header
#include <string>

#define LOG_TAG "substratetag"
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)


//这个方法来自 android inject 用于获取地址
void* get_module_base(int pid, const char* module_name)
{
    FILE *fp;
    long addr = 0;
    char *pch;
    char filename[32];
    char line[1024];
    if (pid < 0) {
        /* self process */
        snprintf(filename, sizeof(filename), "/proc/self/maps");
    }
    else {
        snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);
    }
    fp = fopen(filename, "r");
    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, module_name)) {
                pch = strtok(line, "-");
                addr = strtoul(pch, NULL, 16);

                if (addr == 0x8000)
                    addr = 0;

                break;
            }
        }
        fclose(fp);
    }
    return (void *)addr;
}
//这个方法来自 android inject 用于获取地址
void* get_remote_addr(int target_pid, const char* module_name, void* local_addr)
{
    void* local_handle, *remote_handle;

    local_handle = get_module_base(-1, module_name);
    remote_handle = get_module_base(target_pid, module_name);

    LOGI("[+] get_remote_addr: local[%x], remote[%x]\n", (int)local_handle, (int)remote_handle);

    void * ret_addr = (void *)((uint32_t)local_addr + (uint32_t)remote_handle - (uint32_t)local_handle);

#if defined(__i386__)
    if (!strcmp(module_name, "/system/lib/libc.so")) {
        ret_addr += 2;
    }
#endif
    return ret_addr;
}

MSConfig(MSFilterLibrary,"/system/lib/libdvm.so")

struct _MonoImage {
	int   ref_count;
	void *raw_data_handle;
	char *raw_data;
	int raw_data_len;
};

void* (* old_mono_image_open_from_data_with_name)(char *data, int data_len, bool need_copy, void *status, bool refonly, const char *name);
void* my_mono_image_open_from_data_with_name(char *data, int data_len, bool need_copy, void *status, bool refonly, const char *name)
{
	char* p = strrchr(name, '/')+1;
	mode_t old_mode = umask(0);
	char file_name[256];
	sprintf(file_name, "/data/local/tmp/%s", p);
	int fd = open(file_name, O_RDWR|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO);
	if(fd) {
		write(fd, data, data_len);
		close(fd);
	}
	umask(old_mode);
	
	void *ret = old_mono_image_open_from_data_with_name(data, data_len, need_copy, status, refonly, name);
	LOGD("Into : my_mono_image_open_from_data_with_name, name = %s", name);
	return ret;
}

//hook方法
void* (*olddlsym)(void*  handle, const char*  symbol);
void* newdlsym(void*  handle, const char*  symbol) {
    LOGD("the handle [0x%x] symbol name:%s", (int)handle, symbol);
    return olddlsym(handle, symbol);
}
void* (*olddlopen)(const char* filename, int myflags);
void* newdlopen(const char* filename, int myflags) {
    LOGD("the dlopen name :%s", filename);
    void *handle = olddlopen(filename, myflags);
	if(strstr(filename, "libmono.so")) {
		LOGI("Have load the libmono.so, then try to hook the func!!");
		MSImageRef image;
		image = MSGetImageByName(filename);
		if(image==NULL) {
			LOGI("Image %s not found", filename);
		}
		else {
			LOGI("Image %s found", filename);
		}
		void* mono_image_open_from_data_with_name = MSFindSymbol(image,"mono_image_open_from_data_with_name");
		if(mono_image_open_from_data_with_name==NULL) {
			LOGI("mono_image_open_from_data_with_name not found");
		}
		else {
			LOGI("mono_image_open_from_data_with_name found, try to hook");
			MSHookFunction(mono_image_open_from_data_with_name,(void*)&my_mono_image_open_from_data_with_name,(void**)&old_mono_image_open_from_data_with_name);
		}
	}
	return handle;
}
MSInitialize {
	//声明各个变量存放地址
	//void *mmap_addr, *dlopen_addr, *dlsym_addr, *dlclose_addr, *dlerror_addr;
	//获取dlopen地址
	void *dlopen_addr = get_remote_addr(getpid(), "/system/bin/linker", (void *)dlopen);
	LOGI("[+] dlopen_addr: [%x]", (int)dlopen_addr);
	//hook dlopen方法  下面方法类似
	MSHookFunction((void*)dlopen_addr, (void*)&newdlopen, (void**)&olddlopen);

	void *dlsym_addr = get_remote_addr(getpid(), "/system/bin/linker", (void *)dlsym);
	LOGI("[+] dlsym_addr: [%x]", (int)dlsym_addr);
	MSHookFunction((void*)dlsym_addr, (void*)&newdlsym, (void**)&olddlsym);
	void *dlclose_addr = get_remote_addr(getpid(), "/system/bin/linker", (void *)dlclose);
	void *dlerror_addr = get_remote_addr(getpid(), "/system/bin/linker", (void *)dlerror);
}