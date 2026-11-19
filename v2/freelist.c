#include "freelist.h"
#include <stdio.h>

//the list head can only be accessed in this file
static common_header_t *list_head = NULL;

void freelist_init(void *mem, size_t mem_size)
{
    list_head = (common_header_t *)mem;
    list_head->size = (int)(mem_size - sizeof(common_header_t));
    list_head->next = NULL;
}

common_header_t *freelist_allocate(size_t size)
{
    common_header_t *current = list_head;
    common_header_t *previous = NULL;
    common_header_t *min_current = list_head;
    common_header_t *min_previous = NULL;
    int min = -1;
    int diff;

    while (current != NULL) {
        diff = current->size - (int)size;

        //if the current block has a smaller but enough space
        //or if the current block is the first block with enough space
        if (diff >= 0 && (diff < min || min == -1)) {
            min_current = current;
            min_previous = previous;
            min = diff;
        }

        previous = current;
        current = current->next;
    }

    //no big enough block found
    if (min == -1){
        return NULL;
    }

    int size_remained = min - (int)sizeof(common_header_t);

    //if there is enough space for another header
    if (size_remained >= 0) {
        common_header_t *remainder;
        remainder = (common_header_t *)((char *)min_current + sizeof(common_header_t) + size);
        remainder->size = size_remained;
        remainder->next = min_current->next;

        if (min_previous == NULL) {
            list_head = remainder;
        }
        else {
            min_previous->next = remainder;
        }

        min_current->size = (int)size;
        min_current->next = NULL;
        return min_current;
    }

    //give away the whole block if there is no enough space for another header
    if (min_previous == NULL) {
        list_head = min_current->next;
    } else {
        min_previous->next = min_current->next;
    }
    min_current->next = NULL;
    //returns the pointer to the header
    return min_current;

}

void freelist_check_and_merge(common_header_t *block)
{
    if (block->next == NULL){
        return;
    }

    int gap, length;
    //cast both pointers to char * to get the right pointer arithmetics
    gap = (char *)block->next - (char *)block;
    length = block->size + (int)sizeof(common_header_t);
    
    if (gap == length){
        block->size = block->size + (int)sizeof(common_header_t) + block->next->size;
        block->next = block->next->next;
    }
}

void freelist_insert_and_merge(common_header_t *block)
{
    common_header_t *current = list_head;
    common_header_t *previous = NULL;

    //if the memory is full already
    if(list_head == NULL) {
        list_head = block;
        block->next = NULL;
        return;
    }

    while (current != NULL) {
        if(block < current) {
            block->next = current;
            freelist_check_and_merge(block);
            
            if (current != list_head){
                previous->next = block;
                freelist_check_and_merge(previous);
            }
            else list_head = block;

            return;
        }

        previous = current;
        current = current->next;
    }

    //getting here means the block should be added to the end of the list
    previous->next = block;
    block->next = NULL;
    freelist_check_and_merge(previous);
}

void freelist_dump()
{
    common_header_t *current = list_head;

    while (current != NULL) {
        printf("[%d]", current->size);

        if (current->next != NULL) {
            printf(" -> ");
        }
        current = current->next;
    }
    printf("\n");
}

size_t freelist_free_mem_size()
{
    size_t total = 0;
    common_header_t *current = list_head;

    while (current != NULL) {
        //convert int to size_t
        total += (size_t)current->size;
        current = current->next;
    }

    return total;
}

size_t freelist_largest_free_block()
{
    size_t max = 0;
    common_header_t *current = list_head;

    while (current != NULL) {
        if(current->size > max) max = current->size;
        current = current->next;
    }

    return max;
}

size_t freelist_num_free_block()
{
    size_t num = 0;
    common_header_t *current = list_head;

    while (current != NULL) {
        num += 1;
        current = current->next;
    }

    return num;
}

void freelist_stats(size_t* N, size_t* F, size_t* L){
    size_t n = 0;
    size_t f = 0;
    size_t l = 0;

    common_header_t *current = list_head;
    
    while (current != NULL) {
        n += 1;
        if(current->size > l) l = current->size;
        f += (size_t)current->size;

        current = current->next;
    }

    *N = n;
    *F = f;
    *L = l;
}
