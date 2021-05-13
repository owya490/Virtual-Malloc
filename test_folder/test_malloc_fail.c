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

    void* ptr = virtual_malloc(virtual_heap, 20);
    if (ptr == NULL) {
        printf("Malloc failed!\n");
    }

    virtual_info(virtual_heap);

    void* ptr1 = virtual_malloc(virtual_heap, 0);
    if (ptr1 == NULL) {
        printf("Size was 0, malloc failed!\n");
    }
    virtual_info(virtual_heap);
    return 0;
}