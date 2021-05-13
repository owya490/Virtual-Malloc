/*************************************************************************************
*    25 April 2021
*    COMP2017 Assignment 3 - VIRTUAL ALLOC
*    Owen Yang
*    500550430
*    DESCRIPTION: C FILE FOR VIRTUAL BUDDY ALLOCATOR
*************************************************************************************/

#include "virtual_alloc.h"

#ifndef bool
#define bool
#define True 1
#define False 0
#define END 4294967294
#endif
//Increases the base to the power or pow
int power(int base, int pow) {
    if (pow == 0) {
        return 1;
    }
    int ret = base;
    for (int i = 1; i < pow; i ++) {
        ret *= base;
    }
    return ret;
}

//Changes a integer to its closest greater power of 2
int size_to_power(uint32_t num) {
    
    int count = 0; 
    while (1) {
        if (power(2, count) >= num) {
            return count;
        }
        count ++;
    }
}

//Finds the closest unallocated block of greater than the size requirements
struct block* find_closest_block_unallocated(struct block*c, uint8_t size, void* heapstart) {
    uint8_t initial_size = *((uint8_t*) heapstart);
    while (c != NULL) {
        if (c->free == True && c->size == size) {
            return c;
        }
        if (c->next_num == END) {
            break;
        }
        c = (struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (c->next_num * sizeof(struct block)));
    }
    return NULL;
}

//Recursively splits the blocks down to the necessary size for malloc and creates associated meta data
struct block* split_block(struct block*c, uint8_t n, void* heapstart) {
    if (n == 0) {
        return c;
    }
    //Initiating variables needed at the start of the buddy allocator structure
    void* v  = virtual_sbrk(sizeof(struct block));
    if (v == (void*)(-1)) {
        return NULL;
    }
    uint8_t initial_size = *((uint8_t*) heapstart);
    uint16_t* structure_count = ((uint16_t *)heapstart + 1);
    int count;
    for (int i = 0 ; i < *structure_count; i ++) {
        if (((struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (i * sizeof(struct block)))) == c) {
            count = i;
            break;
        }
    }
    //Looping through the meta data strcutrues to check if any is of size 0, thus can over write the meta data
    for (int i = 0 ; i < *structure_count; i ++) {
        if (((struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (i * sizeof(struct block))))->size == 0) {
            struct block* new = ((struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (i * sizeof(struct block))));
            c->size = c->size-1;
            new->size = c->size;
            new->next_num = c->next_num;
            new->free = True;
            new->data_num = c->data_num + power(2, c->size);
            new->buddy_num = count;
            c->next_num = i;
            *((uint16_t*) heapstart + 1) = (*((uint16_t*) heapstart + 1)) + 1;
            return split_block(c,n-1, heapstart);
        }
    }

    //If there is no size 0, write new meta data at the end of the structure
    c->size = c->size-1; 

    // create new block
    struct block* new = ((struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (*structure_count * sizeof(struct block))));
    new->size = c->size;
    new->next_num = c->next_num;
    new->free = True;
    new->data_num = c->data_num + power(2, c->size);
    new->buddy_num = count;
    //Update c block
    c->next_num = *structure_count;
    *((uint16_t*) heapstart + 1) = (*((uint16_t*) heapstart + 1)) + 1;

    //Recurse split_block until necessary splits are completed
    return split_block(c, n -1, heapstart);
}

//Allocates the existing data structures and the heap space for the buddy allocator
void init_allocator(void * heapstart, uint8_t initial_size, uint8_t min_size) {
    //Converting the size from a power to a integer
    uint32_t final_size = 1 << initial_size;
    //Extening sbrk by the size of the data strcutures and heap
    void * v = virtual_sbrk(sizeof(struct block) + final_size + ((4*sizeof(uint8_t))));
    if (v == (void*)(-1)) {
        printf("Can't Initialise, gg go next...\n");
        return;
    }



    memcpy(heapstart, &initial_size, sizeof(uint8_t));
    memcpy(((char*)heapstart) + 1, &min_size, sizeof(uint8_t));
    uint16_t structure_count = 1;
    memcpy(((char*)heapstart) + 2, &structure_count, sizeof(uint16_t));
    //Creating the first data structure, base one, responsible for a whole contiguous size initial_size heap
    struct block head = {.next_num = END, .size = initial_size, .free = True};
    head.data_num = 4;
    head.buddy_num = END;
    memmove((((char*)heapstart) + ((int)final_size) + (4*sizeof(uint8_t))), &head, sizeof(struct block));
}

//Recursively frees the block and merges with the buddy if available
int my_free_new(struct block* to_be_free, void* heapstart, int n) {
    //Free the block requested to be freed
    to_be_free->free = True;
    uint8_t initial_size = *((uint8_t*) heapstart);
    //If the next block is free and is its buddy, merge it and recurse (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (to_be_free->next_num * sizeof(struct block)))
    if (to_be_free->next_num != END){
            struct block* to_be_free_next = (struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (to_be_free->next_num * sizeof(struct block)));
            struct block* free_next_buddy = ((struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + ((to_be_free_next->buddy_num) * sizeof(struct block))));
        if(free_next_buddy == to_be_free && to_be_free_next->size == to_be_free->size && to_be_free_next->free == True) {
            to_be_free_next->size = 0;
            to_be_free->next_num = to_be_free_next->next_num;
            to_be_free->size ++;
            return my_free_new(to_be_free, heapstart, n);
        }
    }

    if(to_be_free->buddy_num == END) {
        return 0;
    }
    struct block* free_buddy = ((struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + ((to_be_free->buddy_num) * sizeof(struct block))));


    //If its buddy is free and its the same size, merge and recurse
    struct block* free_buddy_next = (struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (free_buddy->next_num * sizeof(struct block)));
    if (free_buddy->free == True && free_buddy->size == to_be_free->size && free_buddy_next == to_be_free) {
        to_be_free->size = 0;
        free_buddy->next_num = free_buddy_next->next_num;
        free_buddy->size ++;
        return my_free_new(free_buddy, heapstart, n);
    }

    return 0;
}

//Allocates a block of memory greater than size and creates necessary meta data for it
void * virtual_malloc(void * heapstart, uint32_t size) {
    if (size == 0) {
        return NULL;
    }
    uint8_t initial_size = *((uint8_t*) heapstart);
    uint8_t min_size = *((uint8_t*) heapstart + 1);
    uint8_t requested_size = size_to_power(size);
    //if the requested size is smaller than the min_size, change it to the min size
    if (requested_size <= min_size) {
        requested_size = min_size;
    }
    struct block* c = (struct block*) (((char*)heapstart) + ((int) 4 + power(2, initial_size)));
    //If there is a free block and of the same size, allocate that block of meta data
    while(c != NULL) {
        if (c->free == True) {
            if (c->size == min_size) {
                if (min_size >= requested_size) {
                    c->free = False;
                    return (char*)heapstart + c->data_num;
                }
            }
            if (c->size == requested_size) {
                c->free = False;
                return (char*)heapstart + c->data_num;
            }
        }
        //c = c->next;
        if (c->next_num == END) {
            break;
        }
        c = (struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (c->next_num * sizeof(struct block)));
    }
    
    //If there isnt, find the closest block of free meta data with closest size and break it down into the correct size using split_block
    for (int i = requested_size + 1; i <= initial_size; i ++) {
        c = (struct block*) (((char*)heapstart) + ((int) 4 + power(2, initial_size)));
        c = find_closest_block_unallocated(c, i, heapstart);
        if (c) {
            c = split_block(c, i - requested_size, heapstart);
            if (c == NULL) {
                return NULL;
            }    

            //There should be correct size of meta data free now and should be allocated        
            if (c->free == True) {
                if (c->size == power(2, min_size)) {
                    if (min_size >= requested_size) {
                        c->free = False;
                        return (char*)heapstart + c->data_num;
                    }
                }
                if (c->size == requested_size) {
                    c->free = False;
                    return (char*)heapstart + c->data_num;
                }
            }
        }
    }   
    return NULL;
}

//Frees the given data, merging and clearing meta data associated with it
int virtual_free(void * heapstart, void * ptr) {
    if (ptr == NULL) {
        return 1;
    }
    uint8_t initial_size = *((uint8_t*) heapstart);
    uint16_t* structure_count = ((uint16_t *)heapstart + 1);
    //Loops through meta data blocks, if the meta data has the same data ptr given, it is the block needing to be freed
    for (int i = 0; i < *structure_count; i ++) {
        if ( (char*)heapstart + ((struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (i * sizeof(struct block))))->data_num == ptr && ((struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (i * sizeof(struct block))))->size != 0) {
            struct block* free = ((struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (i * sizeof(struct block))));
            // If it is already freed, return 1
            if(free->free == True) {
                return 1;
            }
            // Recursively free the block, and merge the meta data
            my_free_new(free, heapstart, 0);
            return 0;
        }
    }
    
    //If PTR is not allocated, return 1
    return 1;
}

//Reallocates previously allocated data to a new size requiring new piece of memory, updates meta data accordingly
void * virtual_realloc(void * heapstart, void * ptr, uint32_t size) {
    if (ptr == NULL) {
        return virtual_malloc(heapstart, size);
    }
    if (size == 0) {
        virtual_free(heapstart, ptr);
        return NULL;
    }
    uint8_t initial_size = *((uint8_t*) heapstart);
    uint16_t* structure_count = ((uint16_t *)heapstart + 1);
    struct block* reallocating;
    int can_fit = False;
    uint32_t size_count = 0;
    //Loops through meta data searching for the required reallocating block, possessing the same data ptr given
    for (int i = 0; i < *structure_count; i ++) {
        if ( (char*)heapstart + ((struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (i * sizeof(struct block))))->data_num ==  ptr) {
            //If the size is not zero, free it temporarily and assign it to reallocating
            if (((struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (i * sizeof(struct block))))->size != 0) {
                reallocating = ((struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (i * sizeof(struct block))));
                reallocating->free = True;
                break;
            }
        }
    }
    //Calculate the available space as if the reallocating block is free, adding the sizes of freed blocks spaces
    for (int i = 0; i < *structure_count; i ++) {
        if (((struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (i * sizeof(struct block))))->free == True) {
            if (((struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (i * sizeof(struct block))))->size != 0) {
                if (((struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (i * sizeof(struct block)))) == (struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (reallocating->next_num * sizeof(struct block)))) {
                    if(((struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + ((((struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (i * sizeof(struct block))))->buddy_num) * sizeof(struct block)))) != reallocating) {
                        continue;
                    }
                }
                size_count += power(2, ((struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (i * sizeof(struct block))))->size);
            }
            if (size_count >= size) {
                can_fit = True;
                break;
            }
        } 
        else {
            if (size_count >= size) {
                can_fit = True;
                break;
            }
            size_count = 0;
        }
    }

    
    //Revert back reallocting to not freed
    reallocating->free = False;
    //If there is enough space, reallocate the block, else return NULL
    if (can_fit == True) {
        virtual_free(heapstart, ptr);

        void* new_ptr = virtual_malloc(heapstart, size);

        memmove(new_ptr, ptr, size);

        return new_ptr;
    } 
    else if (can_fit == False) {
        return NULL;
    }

    return NULL;
}

//Prints out the current state of data and memory in the buddy allocator
void virtual_info(void * heapstart) {
    uint8_t initial_size = *((uint8_t*) heapstart);
    struct block* c = (struct block*) (((char*)heapstart) + ((int) 4 + power(2, initial_size)));
    //Loops through the buddy meta data structures and prints out whether it is free or allocated and the associated size
    while(c != NULL) {
        if (c->free == True) {
            printf("free %d\n", power(2, c->size));
        }
        else {
            printf("allocated %d\n", power(2, c->size));
        }
        //c = c->next;
        if (c->next_num == END) {
            break;
        }
        c = (struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (c->next_num * sizeof(struct block)));
    }
}
