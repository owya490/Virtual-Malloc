#include "virtual_alloc.h"
#include <stdio.h>
#include <stdlib.h>
void * virtual_heap = NULL;

void * virtual_sbrk(int32_t increment) {
    void* ptr = (void*) realloc(virtual_heap, increment);
    return ptr;


}

// int main() {
//     // Your own testing code here
//     void * virtual_heap = virtual_sbrk(0);
//     init_allocator(virtual_heap, 15, 12);

//     //printf("%d\n", ((struct block*)virtual_heap)->size);
//     virtual_info(virtual_heap);
//     return 0;
// }
