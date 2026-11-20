#include "allocator.h"
#include "freelist.h"

//the memory pointer can only be accessed in this file
static void *memory = NULL;

void *get_mem_block(void *addr, size_t mem_size) {
    void *h_ptr = mmap(
        addr,
        mem_size,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0
    );

    if (h_ptr == MAP_FAILED) {
        return NULL;
    }
    return h_ptr;
}

void *smalloc(size_t num_bytes)
{
    //reject zero-sized and oversized requests
    if (num_bytes == 0 || num_bytes > MEM_SIZE) {
        return NULL;
    }

    //initialise the memory
    if (memory == NULL) {
        memory = get_mem_block(NULL, MEM_SIZE);
        if (memory == NULL) {
            return NULL;
        }
        freelist_init(memory, MEM_SIZE);
    }

    common_header_t *block = freelist_allocate(num_bytes);

    if (block == NULL) {
        return NULL;
    }

    //cast the block pointer first to char * 
    //because the pointer arithmatics of char pointer add 1 byte after +1
    //sizeof() returns the size in bytes
    return (void *)((char *)block + sizeof(common_header_t));
}

void sfree(void *mem_ptr)
{
    if (mem_ptr == NULL) {
        return;
    }

    // Recover the metadata header preceding the payload
    common_header_t *header = (common_header_t *)((char *)mem_ptr - sizeof(common_header_t));

    // Insert the block to the free list
    freelist_insert_and_merge(header);
}

void allocator_list_dump()
{
    freelist_dump();
}

size_t allocator_free_mem_size()
{
    return freelist_free_mem_size();
}

void allocator_stats(size_t* N, size_t* F, size_t* L)
{
    freelist_stats(N,F,L);
}

size_t allocator_req_mem(size_t payload_size)
{
    //oversized payload
    if (payload_size > MEM_SIZE - sizeof(common_header_t)) {
        return -1;
    }
    return payload_size + sizeof(common_header_t);
}