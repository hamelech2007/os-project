#include "thread.h"
#include "stdint.h"
#include "memory.h"
#include "kheap.h"

extern void switch_task(uint8_t* old_stack, uint8_t* new_stack);

static uint64_t tid = 0;

struct Thread* thread_create(void (*function)(void)) {
    struct Thread *thread = kmalloc(sizeof(struct Thread));

    if(!thread) return NULL;


    if(!(thread->stack_ptr = kmalloc(PAGE_SIZE))) {
        kfree(thread);
        return NULL;
    }

    thread->tid = tid++;
    

    thread->stack_ptr->rbp = (uint64_t) &thread->stack_ptr->rbp2;
    thread->stack_ptr->ret = (uint64_t) function;

    return thread;
}

void thread_destroy(struct Thread* thread) {
    kfree(thread->stack_ptr);
    kfree(thread);
}