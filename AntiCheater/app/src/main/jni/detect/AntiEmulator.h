//
// Created by Administrator on 2017/4/27 0027.
//

#ifndef ANTICHEATER_ANTIEMULATOR_H
#define ANTICHEATER_ANTIEMULATOR_H

#include <stdlib.h> // avoid exit warning
#include <signal.h> // sigtrap stuff, duh
#include <sys/wait.h> // for waitpid
#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

void handler_sigtrap(int signo);
void handler_sigbus(int signo);
void setupSigTrap();

// This will cause a SIGSEGV on some QEMU or be properly respected
int tryBKPT();

int AntiEmulator();

#ifdef __cplusplus
}
#endif

#endif //ANTICHEATER_ANTIEMULATOR_H
