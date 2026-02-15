#include "../include/thread.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>



static thread* ready_queue = NULL;
static thread* current = NULL;
static ucontext_t scheduler_context;
static int thread_count = 0;

/* Circular queue */

void enqueue(thread* t) {
    if (!ready_queue) {
        ready_queue = t;
        t->next = NULL;
    } else {
        thread* temp = ready_queue;
        while (temp->next)
            temp = temp->next;
        temp->next = t;
        t->next = NULL;
    }
}

thread* dequeue() {
    if (!ready_queue)
        return NULL;

    thread* t = ready_queue;
    ready_queue = ready_queue->next;
    t->next = NULL;
    return t;
}

/* Wrapper function to mark thread finished */
void thread_wrapper(void (*func)()) {
    func();
    current->state = FINISHED;
    swapcontext(&current->context, &scheduler_context);
}

/* Thread creation */

void thread_create(void (*func)()) {

    thread* t = malloc(sizeof(thread));
    t->id = thread_count++;
    t->state = READY;
    t->next = NULL;

    getcontext(&t->context);

    t->context.uc_stack.ss_sp = malloc(STACK_SIZE);
    t->context.uc_stack.ss_size = STACK_SIZE;
    t->context.uc_link = &scheduler_context;

    makecontext(&t->context, (void (*)())thread_wrapper, 1, func);

    enqueue(t);
}

/* Yield */

void thread_yield() {
    current->state = READY;
    enqueue(current);
    swapcontext(&current->context, &scheduler_context);
}

/* Scheduler */

void scheduler_run() {

    while ((current = dequeue()) != NULL) {

        if (current->state == FINISHED) {
            free(current->context.uc_stack.ss_sp);
            free(current);
            continue;
        }

        current->state = RUNNING;
        swapcontext(&scheduler_context, &current->context);
    }
}

void timer_handler(int signum) {
    if (current && current->state == RUNNING) {
        thread_yield();
    }
}

void scheduler_start() {

    struct sigaction sa;
    sa.sa_handler = timer_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGALRM, &sa, NULL);

    struct itimerval timer;

    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 50000;   // 50 ms

    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 50000;

    setitimer(ITIMER_REAL, &timer, NULL);

    scheduler_run();
}