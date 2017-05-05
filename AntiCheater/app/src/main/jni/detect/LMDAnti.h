//
// Created by killer on 5/5/17.
//

#ifndef ANTICHEATER_LMDANTI_H
#define ANTICHEATER_LMDANTI_H


#include "detect/hash/md5.h"
#include "detect/hash/crc32.h"
#include "detect/hash/sha256.h"

#include "Hash.h"
#include "linux.h"
#include "CheckDebug.h"
#include "CheckRoot.h"
#include "CheckSpeedHack.h"
#include "CheckBinaryModification.h"

#include "Debug.h"

extern "C" void init() __attribute__((constructor));

void SpeedHack();
void* ThreadCheckSpeedHack();
void Debug();
void* ThreadCheckDebug();
void RootUser();
void* ThreadRootUserCheck();
void ModBinary();
void* ThreadModBinary();
void run();


#endif //ANTICHEATER_LMDANTI_H

