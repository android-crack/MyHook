//
// Created by Administrator on 2017/4/27 0027.
//
#include "Loader.h"

void *handle = NULL;
bool mRet;

void init()
{
    LOGD("Call init().\n");
    run();
}

void run()
{
   mRet = loadEngine();

    /*
    pthread_t thread_id_1;
    pthread_create(&thread_id_1, NULL, &ThreadCheckSpeedHack, NULL);

    pthread_t thread_id_2;
    pthread_create(&thread_id_2, NULL, &ThreadCheckVirtualMachine, NULL);

    pthread_t thread_id_3;
    pthread_create(&pthread_id_3, NULL, &ThreadRootUserCheck, NULL);


    pthread_t thread_id_4;
    pthread_create(&thread_id_4, NULL, &ThreadCheckDebug, NULL);

    pthread_t thread_id_5;
    pthread_create(&thread_id_5, NULL, &ThreadModBinary, NULL);
    */
    Debug();

    // unLoadEngine();
}

/*
* Load Engine, Unload Engine
*/
bool loadEngine(){
    handle = dlopen("libLMDAnti.so", RTLD_NOW);

    if(!handle)
    {
        LOGE("Failed to dlopen : %s\n", dlerror());
        return false;
    }
    LOGI("True to dlopen\n");
    return true;
}

void unLoadEngine(){
    if(handle){
        dlclose(handle);
        handle = NULL;
    }
}

/*
* CheckDebug
*/

void Debug(){
    LOGD("Call Debug.\n");
    if(mRet){
        void* pCheckDebug;
        pCheckDebug = dlsym(handle, "CheckDbg");
        if(pCheckDebug)
        {
            pCheckDebug;
            LOGI("True to dlsym : CheckDebug.\n");
        } else {
            LOGE("CheckDebug : fail to dlsym %s.\n", strerror(errno));
        }
    }
}

void* ThreadCheckDebug(){
    Debug();
}

/*

//  CheckSppedHack
void SpeedHack() {
    if(mRet){
        void (*pCheckSpeedHack)();
        pCheckSpeedHack = dlsym(handle, "CheckSpeedHack");
        if(pCheckSppedHack)
        {
            (*pCheckSpeedHack)();
            LOGI("True to dlsym : CheckSpeedHack.\n");
        } else {
            LOGE("CheckSpeedHack : failed to dlsym %s\n", strerror(errno));
        }
    }
}

void* ThreadCheckSpeedHack() {
    SpeedHack();
}

// CheckVirtualMachine

void VirtualMachine() {
    if(mRet) {
        int (*pCheckVirtualMachine)();
        pCheckVirtualMachine = dlsym(handle, "CheckVirtualMachine");
        if(pCheckVirtualMachine)
        {
            (*pCheckVirtualMachine)();
             LOGI("True to dlsym : CheckVirtualMachine.\n");
        } else {
            LOGE("CheckVirtualMachine : fail to dlsym %s\n", sterror(errno));
        }
    }
}

void* ThreadCheckVirtualMachine()
{
    VirtualMachine();
}

void RootUser() {
    if(mRet){
        void (*pCheckRootUser)();
        pCheckRootUser = dlsym(handle, "CheckRootUser");
        if(pCheckRootUser)
        {
            (*pCheckRootUser)();
            LOGI("True to dlsym : CheckRootUser.\n");
        } else {
            LOGE("RootUserCheck : fail to dlsym %s.\n", strerror(errno));
        }
    }
}

void* ThreadRootUserCheck() {
    RootUser();
}


void ModBinary() {
    if(mRet)
    {
        void(*pCheckModBinary)();
        pCheckModBinary = dlsym(handle, "CheckModBinary");
        if(pCheckModBinary)
        {
            (*pCheckModBinary)();
            LOGI("True to dlsym : CheckModBinary.\n");
        } else {
            LOGE("CheckModBinary : fail to dlsym %s.\n", strerror(errno));
        }
    }
}

void* ThreadModBinary() {
    ModBinary();
}
*/