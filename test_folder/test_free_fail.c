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
    void * virtual_heap = malloc(1000000);
    sbrk = virtual_heap;
    init_allocator(virtual_heap, 4, 1);

    void* ptr1 = (void*) 2;

    void* ptr = virtual_malloc(virtual_heap, 4);
    int a = virtual_free(virtual_heap, NULL);
    if (a == 1) {
        printf("Free failed, ptr was NULL!\n");
    }

    int b = virtual_free(virtual_heap, ptr1);
    if (b == 1) {
        printf("Free failed, ptr was not in Memory!\n");
    }
    virtual_info(virtual_heap);

    virtual_free(virtual_heap, ptr);
    virtual_info(virtual_heap);

    int c = virtual_free(virtual_heap, ptr);
    if (c == 1) {
        printf("Free failed, ptr was already freed!\n");
    }
    return 0;
}