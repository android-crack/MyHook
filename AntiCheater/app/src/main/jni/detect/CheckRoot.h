//
// Created by killer on 5/5/17.
//

#ifndef ANTICHEATER_CHECKROOT_H
#define ANTICHEATER_CHECKROOT_H

#include <stdlib.h>
#include <stdio.h>

#include "Debug.h"
#include "linux.h"
#include "Hash.h"

bool getPpid(int pid);
bool CheckRootUser();



#endif //ANTICHEATER_CHECKROOT_H
