#pragma once
#include "stdint.h"

enum ThreadState {
    INACTIVE
};

struct ThreadStack {
    uint64_t rbp, rbx, r12, r13, r14, r15, rbp2, ret;
};

struct Thread {
    uint64_t tid; // The thread's ID
    struct ThreadStack* stack_ptr; // The thread's stack pointer
    enum ThreadState state; // The thread's state
};