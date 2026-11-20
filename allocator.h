#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <sys/mman.h> 

//memory sizes
#define MEM_SIZE    10*1024*1024 //10MB of memory
#define TINY_SIZE   1*1024*1024
#define SMALL_SIZE  3*1024*1024
#define MEDIUM_SIZE 6*1024*1024

//max sizes
#define TINY_MAX  256
#define SMALL_MAX 8*1024

//merge on and off
#ifndef MERGE
#define MERGE 1
#endif
//0 for first-fit and 1 for best-fit
#ifndef FIT_TYPE
#define FIT_TYPE 1
#endif
//1 for three size-class zones, 0 for a single heap of MEM_SIZE
#ifndef ZONES
#define ZONES 1
#endif

#define ZONE_NUM 3
enum Zone{
    TINY,
    SMALL,
    MEDIUM
};

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
void allocator_zone_stats(int zone, size_t* N, size_t* F, size_t* L);
//return -1 for oversized payload
size_t allocator_req_mem(size_t payload_size);

#endif