//
// Created by Administrator on 2017/4/24 0024.
//

#include "linker.h"
#include "U3dHook.cy.h"

static string g_strDataPath;
static int g_nCount = 1;


unsigned long getSoBase() {
    unsigned long ret = 0;
    char libName[] = "libmono.so";
    char buf[4096], *tmp;
    //int pid;
    FILE *fp;
    //pid = getpid();
    sprintf(buf, "/proc/self/maps");
    fp = fopen(buf, "r");
    if(fp == NULL) {
        puts("Open failed!\n");
        goto _error;
    }

    while(fgets(buf, sizeof(buf), fp)) {
        if(strstr(buf, libName)) {
            tmp = strtok(buf, "-");
            ret = strtoul(tmp, NULL, 16);
            break;
        }
    }
    _error: fclose(fp);

    return ret;
}

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

MSConfig(MSFilterLibrary, "/system/lib/libdvm.so")


// hook u3d
//void* (* old_mono_image_open_from_data_with_name)(void *arg0, size_t arg1, int arg2, double *arg3, char arg4, int arg5); // hook animal

//void* (* old_mono_image_open_from_data_with_name)(int *offset, char *data_len, int a3, int a4, char refonly, const char *name); // hook jielan
void* (* old_mono_image_open_from_data_with_name)(int8_t *data, int32_t *data_len, gboolean need_copy, MonoImageOpenStatus *status, gboolean refonly,int8_t *name);

//void* my_mono_image_open_from_data_with_name(void *arg0, size_t arg1, int arg2, double *arg3, char arg4, int arg5) // hook animal
void* my_mono_image_open_from_data_with_name(int8_t *data, int32_t *data_len, gboolean need_copy, MonoImageOpenStatus *status, gboolean refonly,int8_t *name) // hook jielan
{
//	char* p = strrchr(name, '/')+1;
//	mode_t old_mode = umask(0);
//	char file_name[256];
//	sprintf(file_name, "/data/local/tmp/%s", p);
//	int fd = open(file_name, O_RDWR|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO);
//	if(fd) {
//		write(fd, data, data_len);
//		close(fd);
//	}
//	umask(old_mode);


//  void *ret = old_mono_image_open_from_data_with_name(data, data_len, need_copy, status, refonly, name);

//	void *ret = old_mono_image_open_from_data_with_name(arg0, arg1, arg2, arg3, arg4, arg5); //hook animal
//	LOGD("Into : my_mono_image_open_from_data_with_name, name = %s, arg1 = %d, arg2 = %d, arg3 = %d, arg4 = %s, arg5 = %d\n", arg0, arg1, arg2, arg3, arg4, arg5);
//  saveAniDllFile(arg0, arg1, getNextFilePath(".dll").c_str());

    LOGD("[hookU3d] Info: image name is %s, data offset is %d, data len is %d.\n", name, data, data_len); // hook jielan
    void *ret = old_mono_image_open_from_data_with_name(data, data_len, need_copy, status, refonly, name); //hook jielan
    saveDllFile(data, data_len, getNextFilePath(".dll").c_str());

	return ret;
}

// hook load_modules
void* (*old_load_modules)(_MonoImage *image);

void* my_load_modules(_MonoImage *image)
{
    LOGD("offset = %s, len is %d.\n", image->raw_data, image->raw_data_len);
    void *ret = old_load_modules(image);
    return ret;
}

bool saveAniDllFile(void *buf, size_t len, const char *outFileName)
{
    LOGD("[U3dHook] call saveFile.\n");
    bool bSuccess = false;
    FILE* file = fopen(outFileName, "wb+");
    if (file != NULL) {
        fwrite(buf, len, 1, file);
        fflush(file);
        fclose(file);
        bSuccess = true;
        chmod(outFileName, S_IRWXU | S_IRWXG | S_IRWXO);
    } else {
        LOGE("[U3dHook] [%s] fopen failed, error: %s\n", __FUNCTION__, dlerror());
    }

    return bSuccess;
}

bool saveDllFile(int8_t *offset, int32_t *data_len, const char *outFileName)
{
    LOGD("[U3dHook] call saveFile.\n");
    bool bSuccess = false;
    FILE* file = fopen(outFileName, "wb+");
    if (file != NULL) {
        fwrite((void *)offset, (int)data_len, 1, file);
        fflush(file);
        fclose(file);
        bSuccess = true;
        chmod(outFileName, S_IRWXU | S_IRWXG | S_IRWXO);
    } else {
        LOGE("[U3dHook] [%s] fopen failed, error: %s\n", __FUNCTION__, dlerror());
    }

    return bSuccess;
}

void hookU3D(const char* filename)
{
    LOGI("[hookU3d] Have load the libmono.so, then try to hook the func!!");
    MSImageRef image;
    image = MSGetImageByName(filename);
    if(image==NULL) {
        LOGI("[hookU3d] Image %s not found", filename);
    }
    else {
        LOGI("[hookU3d] Image %s found", filename);
    }
//    void* mono_image_open_from_data_with_name = MSFindSymbol(image,"mono_image_open_from_data_with_name");
//    if(mono_image_open_from_data_with_name==NULL) {
//        LOGI("[hookU3d] mono_image_open_from_data_with_name not found");
//    }
//    else {
//        LOGI("[hookU3d] mono_image_open_from_data_with_name found, try to hook");
//        MSHookFunction(mono_image_open_from_data_with_name,(void*)&my_mono_image_open_from_data_with_name,(void**)&old_mono_image_open_from_data_with_name);
//    }

    // hook load_modules
    // void* load_modules = MSFindSymbol(image, "load_modules");
    unsigned long load_modules = getSoBase() + 0x18F305;
    if(load_modules == NULL){
        LOGD("[hookU3d] load_modules not found.\n");
    } else {
        LOGD("[hookU3d] load_modules found.tring to hook\n");
        MSHookFunction((void *)load_modules, (void*)&my_load_modules, (void**)&old_load_modules);
    }

}

// hook cocos2dx-lua
// L - lua_state
int (*luaL_loadbuffer_orig)(void *L, const char *buff, int size, const char *name) = NULL;

int luaL_loadbuffer_mod(void *L, const char *buff, int size, const char *name) {
    LOGD("[hookCocos-2dx] luaL_loadbuffer name: %s lua: %s.\n", name, buff);
    int ret = luaL_loadbuffer_orig(L, buff, size, name);
    saveFile(buff, size, getNextFilePath(".lua").c_str());
    return ret;
}

int (*decryptUF_orig)(void *pInBuff, int len, int *n, int *poutlen, char *name) = NULL;

int decryptUF_mod(void *pInBuff, int len, int *n, int *poutlen, char *name)
{
    LOGD("[hookCocos-2dx] decryptUF addr %s, len %d, xx %d, putlen %d, name %s.\n", pInBuff, len, n, poutlen, name);
    int ret = decryptUF_orig(pInBuff, len, n, poutlen, name);
    LOGD("[hookCocos-2dx] save pngs to sdcard.\n");
    saveFile(pInBuff, *poutlen, getNextFilePath(".png").c_str());
    return ret;
}

string getNextFilePath(const char *fileExt) {

    LOGD("[hookCocos-2dx] call getNextFilePath.\n");
    char buff[100] = {0};

    ++g_nCount;

    if(fileExt == ".png")
    {
        g_strDataPath = "/sdcard/img_cache";
    }
    else if(fileExt == ".lua")
    {
        g_strDataPath = "/sdcard/lua_cache";

    }
    else if(fileExt == ".dll")
    {
        g_strDataPath = "/sdcard/dll_cache";

    }

    sprintf(buff, "%s/%d%s",g_strDataPath.c_str(),g_nCount, fileExt);
    LOGD("[hookCocos-2dx] buff is %s.\n", buff);
    return buff;
}

bool saveFile(const void* addr, int len,const char *outFileName)
{
    LOGD("[U3dHook] call saveFile.\n");
    bool bSuccess = false;
    FILE* file = fopen(outFileName, "wb+");
    if (file != NULL) {
        fwrite(addr, len, 1, file);
        fflush(file);
        fclose(file);
        bSuccess = true;
        chmod(outFileName, S_IRWXU | S_IRWXG | S_IRWXO);
    } else {
        LOGE("[U3dHook] [%s] fopen failed, error: %s\n", __FUNCTION__, dlerror());
    }

    return bSuccess;
}

void hookCocos(const char* filename){
    LOGD("[hookCocos-2dx] hook begin.\n");

    MSImageRef image;
    image = MSGetImageByName(filename);
    if (image == NULL) {
        LOGE("[hookCocos-2dx] dlopen libgame error: %s.\n", dlerror());
        return;
    }else{
        LOGD("[hookCocos-2dx] libgmae.so dlopen ok.\n");
    }

    // hook luaL_loadbuffer
    void *pluaL_loadbuffer = MSFindSymbol(image, "luaL_loadbuffer");
    if(pluaL_loadbuffer){
        LOGD("[hookCocos-2dx] luaL_loadbuffer found!\n");
        MSHookFunction(pluaL_loadbuffer, (void *)&luaL_loadbuffer_mod, (void **)&luaL_loadbuffer_orig);
    }

    // hook decryptUF
    void *decryptUF = MSFindSymbol(image, "_ZN7cocos2d5extra8CCCrypto9decryptUFEPhiPiS3_");
    if (decryptUF == NULL)
    {
        LOGE("[hookCocos-2dx] _ZN7cocos2d5extra8CCCrypto9decryptUFEPhiPiS3_ (decryptUF) not found!");
        LOGE("[hookCocos-2dx] dlsym err: %s.", dlerror());
    }else{
        LOGD("[hookCocos-2dx] _ZN7cocos2d5extra8CCCrypto9decryptUFEPhiPiS3_ (decryptUF) found!");
        MSHookFunction(decryptUF, (void *)&decryptUF_mod, (void **)&decryptUF_orig);
    }
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
        hookU3D(filename);
	} else if(strstr(filename, "libgame.so"))
    {
        hookCocos(filename);
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