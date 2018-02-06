//
// Created by Administrator on 2017/4/24 0024.
//

#include "linker.h"
#include "U3dHook.cy.h"


static string g_strDataPath;
static int g_nCount = 1;

inline
int32_t NewSeed(int32_t seed)
{
    seed = (seed * 5188 + 18) % 131076;
    return seed;
}

inline
void NewDec( uint32_t seedVal, unsigned char* buf, uint32_t len)
{
    uint32_t* seed=&seedVal;

    unsigned char* key = ( unsigned char* )seed;// &(*seed);
    uint32_t k = 0;
    unsigned char c = 0;
    uint32_t i;
    for ( i = 0; i < len; i++ )
    {
        k %= 4;
        unsigned char x = ( buf[i] - c )  ^ key[k];
        ++k;
        c = buf[i];
        buf[i] = x;
    }
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
_MonoImage* (* old_mono_image_open_from_data_with_name)(char *data, int data_len, gboolean need_copy, MonoImageOpenStatus *status, gboolean refonly,char *name);
_MonoImage* my_mono_image_open_from_data_with_name(char *data, int data_len, gboolean need_copy, MonoImageOpenStatus *status, gboolean refonly,char *name) // hook jielan
{
    LOGD("[U3DHook] Info: image name is %s, data offset is %s, data len is %d.\n", name, data, data_len); // hook jielan
    LOGD("[U3DHook] Info: data[0] is %d, data[1] is %d.\n", data[0], data[1]);

    _MonoImage *ret = old_mono_image_open_from_data_with_name(data, data_len, need_copy, status, refonly, name); //hook jielan

    LOGD("[U3DHook] after call old_mono_image_open_from_data_with_name datab[0] is %d.\n",data[0]);
    if(strstr(name, "Assembly-CSharp.dll"))
    {
        LOGD("[U3DHook] Assembly-CSharp.dll ");
        saveDllFile(ret->raw_data, ret->raw_data_len, getNextFilePath(".dll").c_str());
    }

	return ret;
}


// hook do_mono_image_load(MonoImage *image, MonoImageOpenStatus *status, gboolean care_about_cli, gboolean care_about_pecoff);
_MonoImage* (*old_do_mono_image_load)(_MonoImage *image, MonoImageOpenStatus *status, gboolean care_about_cli, gboolean care_about_pecoff);
_MonoImage* my_do_mono_image_load(_MonoImage *image, MonoImageOpenStatus *status, gboolean care_about_cli, gboolean care_about_pecoff)
{
    LOGD("[U3DHook] call my_do_mono_image_load. image->moudle_name is %s.\n", image->module_name);

    _MonoImage *ret = old_do_mono_image_load(image, status, care_about_cli, care_about_pecoff);

    if(strstr(image->assembly_name, "Assembly-CSharp.dll"))
    {
        saveFile(image->raw_data, image->raw_data_len, getNextFilePath(".dll").c_str());
    }

    LOGD("[U3DHook] do_mono_image_load ret is :%s.\n", ret);
    return ret;

}

_MonoImage* (*old_mono_image_init)(_MonoImage *image);
_MonoImage* my_mono_image_init(_MonoImage *image)
{
    LOGD("[U3DHook] call mono_image_init. image->assembly_name is %s.\n", image->assembly_name);

    _MonoImage *ret = old_mono_image_init(image);

    if(strstr(image->assembly_name, "Assembly-CSharp.dll"))
        saveFile(image->raw_data, image->raw_data_len, getNextFilePath(".dll").c_str());
    LOGD("[U3DHook] mono_image_init ret is :%s.\n", ret);
    return ret;

}


bool saveDllFile(char *data, int data_len, const char *outFileName)
{
    LOGD("[U3dHook] call saveFile.\n");
    bool bSuccess = false;
    FILE* file = fopen(outFileName, "wb+");
    if (file != NULL) {
        fwrite((void *)data, (int)data_len, 1, file);
        fflush(file);
        fclose(file);
        bSuccess = true;
        chmod(outFileName, S_IRWXU | S_IRWXG | S_IRWXO);
    } else {
        LOGE("[U3dHook] [%s] fopen failed, error: %s\n", __FUNCTION__, dlerror());
    }

    return bSuccess;
}

void U3DHook(const char* filename)
{
    LOGI("[U3DHook] Have load the libmono.so, then try to hook the func!!");
    MSImageRef image;
    image = MSGetImageByName(filename);
    if(image==NULL) {
        LOGI("[U3DHook] Image %s not found", filename);
    }
    else {
        LOGI("[U3DHook] Image %s found", filename);
    }

    void* mono_image_open_from_data_with_name = MSFindSymbol(image,"mono_image_open_from_data_with_name");
    LOGD("[U3DHook] Function mono_image_open_from_data_with_name address is %x.\n", mono_image_open_from_data_with_name);

    if (mono_image_open_from_data_with_name == NULL) {
        LOGI("[U3DHook] mono_image_open_from_data_with_name not found");
    } else {
        LOGI("[U3DHook] mono_image_open_from_data_with_name found, try to hook");
        MSHookFunction(mono_image_open_from_data_with_name,
                       (void *) &my_mono_image_open_from_data_with_name,
                       (void **) &old_mono_image_open_from_data_with_name);

    }

//    void* mono_image_init = MSFindSymbol(image,"mono_image_init");
//    LOGD("[U3DHook] Function mono_image_init address is %x.\n", mono_image_init);
//
//    if (mono_image_init == NULL) {
//        LOGI("[U3DHook] mono_image_init not found");
//    } else {
//        LOGI("[U3DHook] mono_image_init found, try to hook");
//        MSHookFunction(mono_image_init,
//                       (void *) &my_mono_image_init,
//                       (void **) &old_mono_image_init);
//
//    }




//    void* mono_base;
//    mono_base = get_module_base(-1, filename);
//
//
//    void* register_image = (void *)((uint32_t)mono_base + 0x111920);
////    //void* register_image = (void *)((uint32_t)mono_base + 0x196918); // hook bob
//    //void* register_image = MSFindSymbol(image,"register_image");
//    if(register_image == NULL){
//        LOGD("[U3DHook] register_image not found.\n");
//    } else {
//        LOGD("[U3DHook] register_image found.tring to hook\n");
//        MSHookFunction(register_image, (void*)&my_register_image, (void**)&old_register_image);
 //   }

//    void* mono_image_load_pe_data = (void *)((uint32_t)mono_base + 0x195f68);
//    if(mono_image_load_pe_data == NULL)
//    {
//        LOGE("[U3DHook] mono_image_load_pe_data not found.\n");
//    } else {
//        LOGD("[U3DHook] mono_image_load_pe_data found. trying hook.\n");
//        MSHookFunction(mono_image_load_pe_data, (void *)&my_mono_image_load_pe_data, (void **)&old_mono_image_load_pe_data);
//    }

}

// hook cocos2dx-lua
// L - lua_state
int (*luaL_loadbuffer_orig)(void *L, const char *buff, int size, const char *name) = NULL;

int luaL_loadbuffer_mod(void *L, const char *buff, int size, const char *name) {
    LOGD("[Cocos2DHook] luaL_loadbuffer name: %s lua: %s.\n", name, buff);
    int ret = luaL_loadbuffer_orig(L, buff, size, name);
    saveFile(buff, size, getNextFilePath(".lua").c_str());
    return ret;
}

int (*decryptUF_orig)(void *pInBuff, int len, int *n, int *poutlen, char *name) = NULL;

int decryptUF_mod(void *pInBuff, int len, int *n, int *poutlen, char *name)
{
    LOGD("[Cocos2DHook] decryptUF addr %s, len %d, xx %d, putlen %d, name %s.\n", pInBuff, len, n, poutlen, name);
    int ret = decryptUF_orig(pInBuff, len, n, poutlen, name);
    LOGD("[Cocos2DHook] save pngs to sdcard.\n");
    saveFile(pInBuff, *poutlen, getNextFilePath(".png").c_str());
    return ret;
}

string getNextFilePath(const char *fileExt) {

    LOGD("[Cocos2DHook] call getNextFilePath.\n");
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
    LOGD("[Cocos2DHook] buff is %s.\n", buff);
    return buff;
}

bool saveFile(const void *addr, int len,const char *outFileName)
{
    LOGD("[U3DHook] call void saveFile.\n");
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

bool saveFile(const char *addr, int len,const char *outFileName)
{
    LOGD("[U3DHook] call char saveFile.\n");
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
    LOGD("[Cocos2DHook] hook begin.\n");

    MSImageRef image;
    image = MSGetImageByName(filename);
    if (image == NULL) {
        LOGE("[Cocos2DHook] dlopen libgame error: %s.\n", dlerror());
        return;
    }else{
        LOGD("[Cocos2DHook] libgmae.so dlopen ok.\n");
    }

    // hook luaL_loadbuffer
    void *pluaL_loadbuffer = MSFindSymbol(image, "luaL_loadbuffer");
    if(pluaL_loadbuffer){
        LOGD("[Cocos2DHook] luaL_loadbuffer found!\n");
        MSHookFunction(pluaL_loadbuffer, (void *)&luaL_loadbuffer_mod, (void **)&luaL_loadbuffer_orig);
    }

    // hook decryptUF
    void *decryptUF = MSFindSymbol(image, "_ZN7cocos2d5extra8CCCrypto9decryptUFEPhiPiS3_");
    if (decryptUF == NULL)
    {
        LOGE("[Cocos2DHook] _ZN7cocos2d5extra8CCCrypto9decryptUFEPhiPiS3_ (decryptUF) not found!");
        LOGE("[Cocos2DHook] dlsym err: %s.", dlerror());
    }else{
        LOGD("[Cocos2DHook] _ZN7cocos2d5extra8CCCrypto9decryptUFEPhiPiS3_ (decryptUF) found!");
        MSHookFunction(decryptUF, (void *)&decryptUF_mod, (void **)&decryptUF_orig);
    }
}

//hook方法
void* (*olddlsym)(void*  handle, const char*  symbol);
void* newdlsym(void*  handle, const char*  symbol) {
    LOGD("[+]The handle [0x%x] symbol name:%s", (int)handle, symbol);
    return olddlsym(handle, symbol);
}
void* (*olddlopen)(const char *filename, int myflags);
void* newdlopen(const char *filename, int myflags) {
    if(!(strstr(filename, "/system/lib/")))
    {
        LOGD("[+]The dlopen name :%s", filename);
    }

    void *handle = olddlopen(filename, myflags);
	if(strstr(filename, "libmono.so")) {
        U3DHook(filename);
	} else if(strstr(filename, "libgame.so"))
    {
        //hookCocos(filename);
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