#ifndef THREAD_H
#define THREAD_H

#include <ucontext.h>

#define STACK_SIZE 1024 * 64

typedef enum {
    READY,
    RUNNING,
    FINISHED
} thread_state;

typedef struct thread {
    int id;
    ucontext_t context;
    thread_state state;
    struct thread* next;
} thread;

void thread_create(void (*func)());
void thread_yield();
void scheduler_run();
void scheduler_start();

#endif