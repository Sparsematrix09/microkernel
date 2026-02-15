#include <stdio.h>
#include "include/thread.h"

void task1() {
    
    for (int i = 0; i < 5; i++) {
        printf("Task 1: %d\n", i);
        for (volatile long j = 0; j < 10000000; j++);
    }

}

void task2() {
    for (int i = 0; i < 5; i++) {
        printf("Task 2: %d\n", i);
        thread_yield();
    }
}

int main() {

    thread_create(task1);
    thread_create(task2);

    scheduler_start();

    return 0;
}