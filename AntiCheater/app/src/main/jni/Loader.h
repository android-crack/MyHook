//
// Created by Administrator on 2017/4/27 0027.
//

#ifndef ANTICHEATER_LOADER_H
#define ANTICHEATER_LOADER_H


#include <jni.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <sys/ptrace.h>
#include <unistd.h>
#include "detect/Debug.h"

#ifdef __cplusplus
extern "C" {

#else
extern
#endif

extern "C"  void init();

bool loadEngine();
void unLoadEngine();
void SpeedHack();
void* ThreadCheckSpeedHack();
void* VirtualMachine();
void* ThreadCheckVirtualMachine();
void Debug();
void* ThreadCheckDebug();
void RootUser();
void* ThreadRootUserCheck();
void ModBinary();
void* ThreadModiBinary();
void run();

#ifdef __cplusplus

}

#endif

#endif //ANTICHEATER_LOADER_H
