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
    init_allocator(virtual_heap, 15, 7);

    void* arr[200];
    for (int i = 0; i < 200; i ++) {
        arr[i] = virtual_malloc(virtual_heap, 7);
    }
    virtual_info(virtual_heap);
    return 0;
}