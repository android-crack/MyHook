//
// Created by Administrator on 2017/4/27 0027.
//

#include "AntiEmulator.h"
#include "Debug.h"

void handler_sigtrap(int signo){
    exit(-1);
}

void handler_sigbus(int signo)
{
    exit(-1);
}

int setupsigTrap() {
    // BKPT throws SIGTRAP on nexus 5 / oneplus one (and most devices)
    signal(SIGTRAP, handler_sigtrap);

    // BKPT throws SIGBUS on nexus4
    signal(SIGBUS, handler_sigbus);
}

//  This will cause a SIGSEGV on some QEMU or be properly respected
int tryBKPT() {
    __asm__ __volative__("bkpt 255");
}

int AntiEmulator(){

    pid_t child = fork();
    int child_status, status = 0;

    if (child_status == 0)
    {
        setupSigTrap();
        tryBKPT();
    } else if(child == -1) {
        status = -1;
    } else {
        int timeout = 0;
        int i = 0;
        while( waitpid(child, &child_status, WNOHANG) == 0){
            sleep(1);

            if(i++ == 1) {
                timeout = 1;
                break;
            }
        }

        if (timeout == 1)
        {
            // Process time out - likely an emualted device and child is frozen
            status = 1;
        }

        if (WIFEXITED(child_status)) {
            // Likely a real device
            status = 0;
        } else {
            // Didn't exit properly - very likely an emulator
            status = 2;
        }

        // Ensure child is dead
        kill(child, SIGKILL);
    }

    return status;
}
