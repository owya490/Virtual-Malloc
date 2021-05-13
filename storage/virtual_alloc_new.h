#include <stddef.h>
#include <stdint.h>


// typedef enum {
//     True = 1,
//     False = 0
// } bool;
struct block{
    struct block* next;
    //struct block* prev;
    struct block* merge_buddy[12];
    uint8_t merge_buddy_count;
    uint8_t size;
    struct block* buddy;
    void* data;
    //bool free;

    uint8_t free;
    
    //debugging
    //char name;
    

};

void * virtual_sbrk(int32_t increment); // i added check if its right

void init_allocator(void * heapstart, uint8_t initial_size, uint8_t min_size);

void * virtual_malloc(void * heapstart, uint32_t size);

int virtual_free(void * heapstart, void * ptr);

void * virtual_realloc(void * heapstart, void * ptr, uint32_t size);

void virtual_info(void * heapstart);
