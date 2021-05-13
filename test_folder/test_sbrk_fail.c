#include "../virtual_alloc.h"
#include <stdio.h>
#include <stdlib.h>
void * virtual_heap = NULL;
void * sbrk = NULL;

void * virtual_sbrk(int32_t increment) {
    // void* ptr = (void*) realloc(virtual_heap, increment);
    // return ptr;
    return (void*) (-1);


}

int main() {
    // Your own testing code here
    virtual_heap = malloc(4000000000);
    sbrk = virtual_heap;
    init_allocator(virtual_heap, 4, 2);


}