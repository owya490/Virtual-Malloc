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
    void * virtual_heap = malloc(4000000000);
    sbrk = virtual_heap;
    init_allocator(virtual_heap, 8, 2);
    
    virtual_info(virtual_heap);
    printf("-----------\n");
    void* array[200];
    for (int i = 0; i < 25; i++){
        array[i] = virtual_malloc(virtual_heap, 4);
    }
    virtual_info(virtual_heap);
    printf("------------------\n");
    //virtual_info(virtual_heap);
    for (int i = 0; i < 25; i++){
        array[i] = virtual_realloc(virtual_heap, array[i], 8);
    }
    virtual_info(virtual_heap);
    for (int i = 0; i < 25; i++){
        virtual_free(virtual_heap, array[i]);
    }
    printf("--------------------------------\n");
    virtual_info(virtual_heap);
}