//
// Created by Administrator on 2017/3/31 0031.
//

#include "substrate.h"
#include "hook.cy.h"

#define LIB "/data/app-lib/com.ztgame.jielan-1/libmono.so"

//指明要hook的lib
//MSConfig(MSFilterLibrary, "/data/app-lib/com.ztgame.jielan-1/libmono.so")
MSConfig(MSFilterExecutable, "/system/bin/app_process")

void *g_handlelibMonoSo;
//void *g_handlelibGameSo;
bool g_bU3dHooked = false;
//bool g_bCocosHooked;


//hook方法
//void* (*olddlsym)(void*  handle, const char*  symbol);
//void* newdlsym(void*  handle, const char*  symbol) {
//    LOGD("the handle [0x%x] symbol name:%s",handle,symbol);
//    return olddlsym(handle, symbol);
//}


void *(*mono_image_open_from_data_with_name_orig)(int offset, char *data_len, int a3, int a4, char refonly,const char *name);

void *mono_image_open_from_data_with_name_mod(int offset, char *data_len, int a3, int a4, char refonly,const char *name){

    LOGD("Call my image open function.\n");

    if(name != NULL){
        LOGD("Open dll name is %s, data offset is %d, data len %d\n", name, offset, data_len);
//        char *tmp = strstr(name,".dll");
//        if (tmp != NULL && strcmp(tmp,".dll")== 0){
//            if(data[0] == 'M' && data[1] == 'Z'){
//                LOGD("Open dll %s, data len %d\n", data, data_len);
//            }
//        }
    }

    void *res = mono_image_open_from_data_with_name_orig(offset, data_len, a3, a4, refonly, name);
    return res;
}

void* (*olddlopen)(const char* filename, int myflags) = NULL;

//void* newdlopen(const char* filename, int myflags) {
//    LOGD("The dlopen name: %s\n", filename);
//
//    return olddlopen(filename, myflags);
//}

void* newdlopen(const char* filename, int myflags) {
    LOGD("the dlopen name =: %s",filename);
    void *handle = olddlopen(filename, myflags);
    LOGD("Call newdlopen.\n");

    if ( strcmp(filename, "/data/app-lib/com.ztgame.jielan-1/libmono.so") == 0){
        if (g_bU3dHooked == false) {
            // hook libmono.so
            g_handlelibMonoSo = handle;
            LOGD("[xxoo] hook libmono.so\n");
            g_bU3dHooked = hookU3D();
        }
    }

//    if ( strcmp(filename, "libmono.so")==0 ) {
//        if ( g_bU3dHooked==false ) {
//            //libmono.so加载了，但是发现之前并没有HOOK成功
//            g_handlelibMonoSo = handle;
//            LOGD("[%s] hook libmono.so", __FUNCTION__);
//            g_bU3dHooked = hookU3D();
//        }
//    }else if ( strcmp(filename, "libgame.so")==0 ) {
//        if ( g_bCocosHooked==false ) {
//            g_handlelibGameSo = handle;
//            LOGD("[%s] hook libgame.so", __FUNCTION__);
//            g_bCocosHooked = hookCocos();
//        }
//    }
    return handle;
}

void* loadLib(char* libraryName, char* funcName)
{
    void *handle = dlopen(libraryName, RTLD_NOW | RTLD_GLOBAL);

    if(handle != NULL)
    {

        LOGD("dlopen %s successfully!\n", LIB);
        void *sym = dlsym(handle, funcName);
        if(sym){
            return sym;
        } else {
            LOGE("find symbol error!\n");
            return NULL;
        }
    } else{
        LOGE("dlopen %s error!\n", LIB);
        return NULL;
    }
}


bool hookU3D() {
    void *handle = NULL;
//    if ( g_handlelibMonoSo==NULL ) {
//        if ( olddlopen==NULL ) {
//            handle = dlopen("libmono.so", RTLD_NOW);
//        }else{
//            handle = olddlopen("libmono.so", RTLD_NOW);
//        }
//        if (handle == NULL) {
//            LOGE("[dumplua]dlopen err: %s.", dlerror());
//            return false;
//        }
//    }else{
//        handle = g_handlelibMonoSo;
//        LOGD("[dumplua] libmono.so handle: %p", handle);
//    }

    handle = dlopen(LIB, RTLD_NOW);

    void *mono_image_open_from_data_with_name = dlsym(handle, "mono_image_open_from_data_with_name");
    if (mono_image_open_from_data_with_name == NULL){
        LOGE("[dumplua] mono_image_open_from_data_with_name not found!");
        LOGE("[dumplua] dlsym err: %s.", dlerror());
    }else{
        LOGD("[dumplua] mono_image_open_from_data_with_name found: %p", mono_image_open_from_data_with_name);
        MSHookFunction(mono_image_open_from_data_with_name, (void *)&mono_image_open_from_data_with_name_mod, (void **)&mono_image_open_from_data_with_name_orig);
    }

    return true;
}


//用于获取地址
void* get_module_base(int pid, const char* module_name)
{
    FILE *fp;
    long addr = 0;
    char *pch;
    char filename[32];
    char line[1024];
    if (pid < 0) {
        /* self process */
        snprintf(filename, sizeof(filename), "/proc/self/maps", pid);
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

    LOGI("[+] get_remote_addr: local[%x], remote[%x]\n", local_handle, remote_handle);

    void * ret_addr = (void *)((uint32_t)local_addr + (uint32_t)remote_handle - (uint32_t)local_handle);

#if defined(__i386__)
    if (!strcmp(module_name, "/system/lib/libc.so")) {
        ret_addr += 2;
    }
#endif
    return ret_addr;
}

void my_hook() {

    LOGD("Call my_hook function!\n");

    //声明各个变量存放的地址
    void *dlopen_addr;

    //获取dlopen的地址
    dlopen_addr = get_remote_addr(getpid(), "/system/bin/linker", (void *)dlopen);
    //Hook dlopen
    MSHookFunction((void*)dlopen_addr, (void *)newdlopen, (void **)&olddlopen);


}

/*
 * Substrate entry point
 */
//初始化时进行hook

MSInitialize
{
    // Let the user know that the extension has been
    // extension has been registered
    LOGI( "MyHook Substrate initialized.\n");

    void *getSym = loadLib(LIB, "mono_jit_stats");

    if(getSym){
        //LOGD("Call my_hook function.\n");
        //my_hook();
        hookU3D();
    }
    else {
        LOGE("Get sym error!\n");
    }


//    MSImageRef image;
//    image = MSGetImageByName("/system/lib/libdvm.so");
//    if(image != NULL)
//    {
//        LOGD("dvm image: 0x%08x", (void *)image);
//        void * dvmload = MSFindSymbol(image, "_Z17dvmLoadNativeCodePKcP6ObjectPPc");
//        if(dvmload == NULL)
//        {
//            LOGD("error find dvmLoadNativeCode.");
//        }
//        else
//        {
//            MSHookFunction(dvmload,(void*)&fake_dvmLoadNativeCode,(void **)&_dvmLoadNativeCode);
//            LOGD("hook dvmLoadNativeCode sucess.");
//        }
//    }
//    else{
//        LOGD("can not find libdvm.");
//    }

}