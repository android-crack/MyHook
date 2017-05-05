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
    while(true){
        mRet = getSo();
        if(!mRet)
        {
            badData();
        }
    }
}

void run()
{
   mRet = loadEngine();
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

bool getSo() {
    char libName[] = "libLMDAnti.so";
    char buffer[4096] = {0};
    char *temp;
    int pid;
    FILE *fp;

    pid = getpid();
    sprintf(buffer, "/proc/%d/maps", pid);
    fp = fopen(buffer, "r");
    if (fp == NULL) {
        LOGD("Open maps file failed.\n");
        fclose(fp);
        return false;
    }

    while(fgets(buffer, sizeof(buffer), fp))
    {
        if(strstr(buffer, libName))
        {
            fclose(fp);
            return true;
        }
    }
}

void badData()
{
    int  pid;
    FILE *fd;
    char fileName[MAX];
    char line[MAX];
    pid = getpid();
    sprintf(fileName, "/proc/%d/status", pid);

    if(fork() == 0)
    {
        fd = fopen(fileName, "r");
        while(fgets(line, MAX, fd)) {
            if(strncmp(line, "TracerPid", 9) == 0)
            {
                LOGD("Line is %s\n", line);
                int statue = atoi(&line[10]);
                LOGD("@@@@ status = %d, %s \n", statue, line);
                fclose(fd);
                if (statue != 0) {
                    LOGD(" @@@@@ junk data here.\n");
                    int ret = kill(pid, SIGKILL);
                    LOGD(" @@@@@@@@@@@ kill-self..\n");
                    return;
                }
                break;
            }
        }
    }
}