# Microkernel-Style Threading and Memory Subsystem

A lightweight user-level microkernel-inspired system implementing cooperative + preemptive threading and a custom dynamic memory allocator in C/C++ on Linux.

This project demonstrates core operating system concepts including scheduling, context switching, memory management, and concurrency control.

---

## 🚀 Features

### 🧵 User-Level Threading System
- Implemented using `ucontext` API for manual context switching
- Cooperative multitasking using `thread_yield()`
- Preemptive scheduling using `SIGALRM` + `setitimer`
- Round-robin scheduler with ready queue management
- Thread lifecycle states: `READY`, `RUNNING`, `FINISHED`

---

### ⏱ Scheduler Design
- Timer-driven preemption (50ms quantum)
- Signal handler-based interrupt mechanism
- Scheduler loop with dequeue–execute–requeue model
- Lightweight context switching without kernel thread dependency

---

### 🧠 Custom Memory Allocator
- Built using `mmap` for direct virtual memory allocation
- Free-list based allocation strategy
- Block splitting to reduce internal fragmentation
- Coalescing of adjacent free blocks to reduce external fragmentation
- `custom_malloc()` and `custom_free()` API

---

## 🧩 System Architecture
User Program
↓
Thread Library (ucontext)
↓
Scheduler (SIGALRM-based preemption)
↓
Memory Allocator (mmap + free list)
↓
Linux Kernel (system calls)

---

## ⚙️ Build & Run

### Compile
```bash
g++ main.c threading/thread.c memory/allocator.c -o microkernel -Wall
./microkernel
```
