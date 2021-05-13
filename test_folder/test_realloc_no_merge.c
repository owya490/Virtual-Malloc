#include "../virtual_alloc.h"
#include <stdio.h>
#include <stdlib.h>
void * virtual_heap = NULL;
void * sbrk = NULL;

void * virtual_sbrk(int32_t increment) {
    sbrk += increment;
    return sbrk;
}

int main() {
    // Your own testing code here
    virtual_heap = malloc(4000000000);
    sbrk = virtual_heap;
    init_allocator(virtual_heap, 4, 2);
    void* ptr1 = virtual_malloc(virtual_heap, 4);
    void* ptr2 = virtual_malloc(virtual_heap, 4);
    void* ptr3 = virtual_malloc(virtual_heap, 4);
    void* ptr4 = virtual_malloc(virtual_heap, 4);

    virtual_free(virtual_heap, ptr3);

    void* result = virtual_realloc(virtual_heap, ptr2, 8);


    if (result != NULL) {
        printf("Hello!\n");
    }
    virtual_info(virtual_heap);

}