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

#define MAX 128

static void init() __attribute__(constructor);
bool loadEngine();
void unLoadEngine();
bool getSo();
void badData();

#endif //ANTICHEATER_LOADER_H
