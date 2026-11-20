#include "allocator.h"
#include "freelist.h"

//each zone has its own arena and its own freelist
//these can only be accessed in this file
static void *arena[ZONE_NUM] = {NULL, NULL, NULL};
static common_header_t *zone_head[ZONE_NUM] = {NULL, NULL, NULL};

#if ZONES
static const size_t zone_size[ZONE_NUM] = {TINY_SIZE, SMALL_SIZE, MEDIUM_SIZE};
#else
//single heap: equivalent to only one zone and it gets all the memory
static const size_t zone_size[ZONE_NUM] = {0, 0, MEM_SIZE};
#endif

static const char *zone_name[ZONE_NUM] = {"TINY", "SMALL", "MEDIUM"};

static int pick_zone(size_t required)
{
    //single heap case
    if (!ZONES) return MEDIUM;

    if (required <= TINY_MAX) return TINY;
    if (required <= SMALL_MAX) return SMALL;
    return MEDIUM;
}

//find which zone an address belongs to, -1 if it is in none of them
//char pointer is used for arithmetic convenience
static int find_zone(void *addr)
{
    for (int z = 0; z < ZONE_NUM; z++) {
        if (arena[z] != NULL){
            char *start = (char *)arena[z];
            char *end = start + zone_size[z];

            //the actual end is the address right before end
            if ((char *)addr >= start && (char *)addr < end) {
                return z;
            }
        }        
    }
    return -1;
}

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

    int zone = pick_zone(num_bytes + sizeof(common_header_t));
    common_header_t *block = NULL;

    //try the current zone, escalate to the next zone theres no enough space
    while (zone < ZONE_NUM && block == NULL) {
        //create the arena if havent yet
        if (arena[zone] == NULL) {
            arena[zone] = get_mem_block(NULL, zone_size[zone]);
            if (arena[zone] == NULL) {
                zone++;
                continue;
            }
            freelist_init(&zone_head[zone], arena[zone], zone_size[zone]);
        }

        if(FIT_TYPE) block = freelist_allocate_best(&zone_head[zone], num_bytes);
        else block = freelist_allocate_first(&zone_head[zone], num_bytes);

        zone++;
    }

    //if no zone can satisfy the size
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

    common_header_t *header = (common_header_t *)((char *)mem_ptr - sizeof(common_header_t));

    int zone = find_zone(header);

    //not a pointer inside the arenas
    if(zone < 0){
        return;
    }

    if(MERGE) freelist_insert_and_merge(&zone_head[zone],header);
    else freelist_insert(&zone_head[zone],header);
}

void allocator_list_dump()
{
    for (int i = 0; i < ZONE_NUM; i++) {
        if (arena[i] != NULL){
            printf("%s: ", zone_name[i]);
            freelist_dump(&zone_head[i]);
        }
    }
}

size_t allocator_free_mem_size()
{
    size_t total = 0;

    for (int i = 0; i < ZONE_NUM; i++) {
        total += freelist_free_mem_size(&zone_head[i]);
    }
    return total;
}

void allocator_stats(size_t* N, size_t* F, size_t* L)
{
    size_t n = 0;
    size_t f = 0;
    size_t l = 0;

    for (int i = 0; i < ZONE_NUM; i++) {
        size_t ni, fi, li;
        freelist_stats(&zone_head[i], &ni, &fi, &li);

        n += ni;
        f += fi;
        if (li > l) l = li;
    }

    *N = n;
    *F = f;
    *L = l;
}

void allocator_zone_stats(int zone, size_t* N, size_t* F, size_t* L)
{
    freelist_stats(&zone_head[zone], N, F, L);
}

size_t allocator_req_mem(size_t payload_size)
{
    //oversized payload: larger than the biggest zone
    if (payload_size > zone_size[MEDIUM] - sizeof(common_header_t)) {
        return -1;
    }
    return payload_size + sizeof(common_header_t);
}