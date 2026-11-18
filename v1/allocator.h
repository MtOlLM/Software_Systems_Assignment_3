#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <sys/mman.h> 

#define MEM_SIZE 1024

//get memory from OS using mmap
void *get_mem_block(void *addr, size_t mem_size);

//allocate memory
void *smalloc(size_t num_bytes);

//free allocated memory
void sfree(void *mem_ptr);

//utility functions
void allocator_list_dump();
size_t allocator_free_mem_size();
//return -1 for oversized payload
size_t allocator_req_mem(size_t payload_size);

#endif