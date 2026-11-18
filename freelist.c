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

    while (current != NULL) {
        //if the current block has enough space
        if (current->size >= (int)size) {
            int size_remained = current->size - (int)size - (int)sizeof(common_header_t);

            //if there is enough space for another header
            if (size_remained >= 0) {
                common_header_t *remainder;
                remainder = (common_header_t *)((char *)current + sizeof(common_header_t) + size);
                remainder->size = size_remained;
                remainder->next = current->next;

                if (previous == NULL) {
                    list_head = remainder;
                }
                else {
                    previous->next = remainder;
                }

                current->size = (int)size;
                current->next = NULL;
                return current;
            }

            //give away the whole block if there is no enough space for another header
            if (previous == NULL) {
                list_head = current->next;
            } else {
                previous->next = current->next;
            }
            current->next = NULL;
            //returns the pointer to the header
            return current;
        }

        previous = current;
        current = current->next;
    }

    return NULL;
}

void freelist_insert(common_header_t *block)
{
    common_header_t *current = list_head;
    common_header_t *previous = list_head;

    while (current != NULL) {
        if(block < current){
            if (current != list_head) previous->next = block;
            else list_head = block;
            block->next = current;
            return;
        }

        previous = current;
        current = current->next;
    }

    //getting here means the block should be added to the end of the list
    previous->next = block;
    block->next = NULL;
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
