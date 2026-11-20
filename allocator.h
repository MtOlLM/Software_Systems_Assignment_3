#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <sys/mman.h> 

#define MEM_SIZE 10*1024*1024 //10MB of memory

//merge on and off
#define MERGE 0
//0 for first-fit and 1 for best-fit
#define FIT_TYPE 1

//get memory from OS using mmap
void *get_mem_block(void *addr, size_t mem_size);

//allocate memory
void *smalloc(size_t num_bytes);

//free allocated memory, merge if possible
void sfree(void *mem_ptr);

//utility functions
void allocator_list_dump();
size_t allocator_free_mem_size();
void allocator_stats(size_t* N, size_t* F, size_t* L);
//return -1 for oversized payload
size_t allocator_req_mem(size_t payload_size);

#endif