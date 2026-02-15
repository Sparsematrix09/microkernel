#include "../include/allocator.h"
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>



typedef struct block_meta {
    size_t size;
    int free;
    struct block_meta* next;
} block_meta;

#define META_SIZE sizeof(block_meta)

static block_meta* global_base = NULL;

void split_block(block_meta* block, size_t size);
void coalesce();
block_meta* find_free_block(size_t size);
block_meta* request_space(block_meta* last, size_t size);

block_meta* find_free_block(size_t size) {

    block_meta* current = global_base;

    while (current) {
        if (current->free && current->size >= size)
            return current;

        current = current->next;
    }

    return NULL;
}

block_meta* request_space(block_meta* last, size_t size) {

    size_t total_size = size + META_SIZE;

    void* block = mmap(NULL,
                       total_size,
                       PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS,
                       -1,
                       0);

    if (block == MAP_FAILED)
        return NULL;

    block_meta* meta = (block_meta*)block;
    meta->size = size;
    meta->free = 0;
    meta->next = NULL;

    if (last)
        last->next = meta;

    return meta;
}

void* custom_malloc(size_t size) {

    if (size == 0)
        return NULL;

    block_meta* block;

    if (!global_base) {
        block = request_space(NULL, size);
        if (!block)
            return NULL;
        global_base = block;
    } else {
        block_meta* last = global_base;
        block = find_free_block(size);

        if (!block) {
            while (last->next)
                last = last->next;

            block = request_space(last, size);
            if (!block)
                return NULL;
        } else {
            split_block(block, size);
            block->free = 0;
        }
    }

    return (block + 1);
}

void custom_free(void* ptr) {

    if (!ptr)
        return;

    block_meta* meta = (block_meta*)ptr - 1;
    meta->free = 1;
    coalesce();
}

void split_block(block_meta* block, size_t size) {

    if (block->size <= size + META_SIZE)
        return;

    block_meta* new_block =
        (block_meta*)((char*)(block + 1) + size);

    new_block->size = block->size - size - META_SIZE;
    new_block->free = 1;
    new_block->next = block->next;

    block->size = size;
    block->next = new_block;
}
void coalesce() {

    block_meta* current = global_base;

    while (current && current->next) {

        if (current->free && current->next->free) {

            current->size += META_SIZE + current->next->size;
            current->next = current->next->next;

        } else {
            current = current->next;
        }
    }
}