#ifndef FREELIST_H
#define FREELIST_H

#include <stddef.h>

typedef struct __common_header_t common_header_t;

struct __common_header_t {
    int size;                     // size of the memory block
    common_header_t *next;   // pointer to the next node of the freelist 
};

//initialisation
void freelist_init(void *mem, size_t mem_size);

//allocate from the free list, returns the pointer to the header
//best-fit version
common_header_t *freelist_allocate(size_t size);

//check if the block can merge with the next block, merge if so
void freelist_check_and_merge(common_header_t *block);

//insert the block to the free list, merge if possible
void freelist_insert_and_merge(common_header_t *block);

//print the free list
void freelist_dump();

//calculate total free memory size
size_t freelist_free_mem_size();

#endif