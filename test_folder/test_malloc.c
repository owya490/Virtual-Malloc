#include "../virtual_alloc.h"
#include <stdio.h>
#include <stdlib.h>
void * virtual_heap = NULL;
void* sbrk = NULL;
void * virtual_sbrk(int32_t increment) {
    sbrk += increment;
    return sbrk;
}

int main() {
    // Your own testing code here
    void * virtual_heap = malloc(1000000);
    sbrk = virtual_heap;
    init_allocator(virtual_heap, 4, 1);
    virtual_malloc(virtual_heap, 4);
    virtual_malloc(virtual_heap, 8);
    virtual_info(virtual_heap);
    return 0;
}