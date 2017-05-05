//
// Created by killer on 5/5/17.
//
#include "LMDAnti.h"

void *handle = NULL;
bool mRet;

void init()
{
    LOGD("Call init().\n");
    run();
}

void run()
{

    ThreadCheckSpeedHack();

    ThreadRootUserCheck();

    ThreadCheckDebug();

    ThreadModBinary();
}


/*
* CheckDebug
*/

void Debug(){
    LOGD("Call Debug.\n");
    CheckDbg();
}

void* ThreadCheckDebug(){
    Debug();
}


//  CheckSppedHack
void SpeedHack() {
    LOGD("Call SpeedHack.\n");
    CheckSpeedHack();
}

void* ThreadCheckSpeedHack() {
    SpeedHack();
}


void RootUser() {
    LOGD("Call RootUser().\n");
    CheckRootUser();
}

void* ThreadRootUserCheck() {
    RootUser();
}


void ModBinary() {
    LOGD("Call ModBinary.\n");
    CheckModBinary();
}

void* ThreadModBinary() {
    ModBinary();
}