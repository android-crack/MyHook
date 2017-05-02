//
// Created by Administrator on 2017/4/27 0027.
//

#ifndef ANTICHEATER_CHECKDEBUG_H
#define ANTICHEATER_CHECKDEBUG_H

#include <string>
#include <sys/inotify.h>
#include <sys/ptrace.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <jni.h>
#include <elf.h>
#include "Debug.h"

#ifdef __cplusplus
extern "C" {

#else
extern
#endif

void CheckDbg();

static void *multTh(void *arg);
bool getSo();
unsigned long getSoBase();
void soData();
void badData();
void xxoo01();
void GetAn();
void GetT();

#ifdef __cplusplus

}

#endif

#endif //ANTICHEATER_CHECKDEBUG_H
