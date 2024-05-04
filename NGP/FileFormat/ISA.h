#pragma once
#include "Header.h"

enum NGPInstructionClass : u8 {
    NGP_CALL,
    NGP_BRANCH,

    NGP_BRANCH_COND,
    NGP_LOAD_STORE,
    NGP_LOAD_STORES,
    NGP_LOAD_STORE_PC,
};


