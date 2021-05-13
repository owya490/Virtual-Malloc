#include "virtual_alloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// typedef enum {
//     True = 1,
//     False = 0
// } bool;

// struct block{
//     struct block* next;
//     struct block* prev;
//     struct block* merge_buddy[20];
//     int size;
//     void* data;
//     bool free;

// };

#define True 1
#define False 0

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

int size_to_power(uint32_t num) {
    
    int count = 0; 
    while (1) {
        if (power(2, count) >= num) {
            return count;
        }
        count ++;
    }
}

struct block* find_closest_block_unallocated(struct block*c, uint8_t size) {
    while (c) {
        if (c->free == True && c->size == size) {
            return c;
        }
        c = c->next;
    }
    return NULL;
}

struct block* split_block(struct block*c, uint8_t n, void* heapstart) {
    //printf("n: %d\n", n);
    if (n == 0) {
        return c;
    }
    virtual_sbrk(sizeof(struct block));
    uint8_t initial_size = *((uint8_t*) heapstart);
    //uint8_t min_size = *((uint8_t*) heapstart + 1);
    uint16_t* structure_count = ((uint16_t *)heapstart + 1);

    //printf("sc: %d\n", *structure_count);

    for (int i = 0 ; i < *structure_count; i ++) {
        //if (    ((struct block*) (((char*)heapstart) + ((int) i + 3 + power(2, initial_size))))->size == 0 ) {
        if (    ((struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (i * sizeof(struct block))))->size == 0 ) {
            //printf("hey\n");
            // place structure in this location
            struct block* new = ((struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (i * sizeof(struct block))));
            c->size = c->size-1;
            new->size = c->size;
            new->next = c->next;
            //new->prev = c;
            new->free = True;
            new->data = (void*) (((char*) c->data) + ((int) power(2, c->size)));
            //new->name = 'b';
            new->merge_buddy_count = 0;
            new->merge_buddy[new->merge_buddy_count] = c;
            new->merge_buddy_count++;

            c->next = new;
            c->merge_buddy[c->merge_buddy_count] = new;
            c->merge_buddy_count++;
            *((uint16_t*) heapstart + 1) = (*((uint16_t*) heapstart + 1)) + 1;

            return split_block(c,n-1, heapstart);

        }
    }


    c->size = c->size-1;

    struct block* new = ((struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (*structure_count * sizeof(struct block))));

    new->size = c->size;
    new->next = c->next;
    //new->prev = c;
    new->free = True;
    new->data = (void*) (((char*) c->data) + ((int) power(2, c->size)));
    //new->name = 'b';
    new->merge_buddy_count = 0;
    new->merge_buddy[new->merge_buddy_count] = c;
    new->merge_buddy_count++;


    //update c
    c->next = new;
    c->merge_buddy[c->merge_buddy_count] = new;
    c->merge_buddy_count++;

    *((uint16_t*) heapstart + 1) = (*((uint16_t*) heapstart + 1)) + 1;
    //printf("strucutre: %d\n", *structure_count);

    return split_block(c, n -1, heapstart);


}

void init_allocator(void * heapstart, uint8_t initial_size, uint8_t min_size) {
    // Your code here
    uint32_t final_size = 1 << initial_size;



    
    virtual_sbrk(sizeof(struct block) + final_size + ((4*sizeof(uint8_t))));

    //m(heapstart, &initial_size, sizeof(uint8_t));
    //memcpy(heapstart+sizeof(uint8_t), &min_size, sizeof(uint8_t));
    memcpy(heapstart, &initial_size, sizeof(uint8_t));
    memcpy(((char*)heapstart) + 1, &min_size, sizeof(uint8_t));
    uint16_t structure_count = 1;
    memcpy(((char*)heapstart) + 2, &structure_count, sizeof(uint16_t));

    //printf("%d\n", *(uint16_t*)((char*)heapstart+2));
    //printf("%d\n", (*(uint16_t*)((char*)heapstart + 2)));

    struct block head = {.next = NULL, .size = initial_size, .free = True};
    head.data = ((char*) heapstart) + 4;
    //head.name = 'a';
    head.merge_buddy_count = 0;

    //printf("s: %d\n", head.size);
    //memmove(  (((char*)heapstart) + 20), &head, sizeof(struct block)              );
    //memmove(((char*)heapstart) + ((int)final_size) + (2*sizeof(uint8_t)) + sizeof(uint16_t), &head, sizeof(struct block));
    memmove((((char*)heapstart) + ((int)final_size) + (4*sizeof(uint8_t))), &head, sizeof(struct block));

    //printf("%d\n", ((struct block*) (((char*)heapstart) + 20))->size                       );

    //printf("%d\n", ((struct block*)(((char*)heapstart) + ((int)final_size) + (2*sizeof(uint8_t)) + sizeof(uint16_t)))->size  );





}

int my_free(struct block* to_be_freed, void* heapstart, int n) {
    to_be_freed->free = True;
    //printf("--------------\n");
    //virtual_info(heapstart);
    //printf("--------------\n");
    if (to_be_freed->merge_buddy_count == 0) {
        if (n > 0) {
            return 0;
        }
        else if (n <= 0) {
            return 1;
        }
    }
    //to_be_freed->free = True;
    struct block* free_buddy = to_be_freed->merge_buddy[to_be_freed->merge_buddy_count-1];

    if (free_buddy->merge_buddy_count == 0) {
        if (n > 0) {
            return 0;
        }
        else if (n <= 0) {
            return 1;
        }
    }
    //to_be_freed->free = True;
    if (free_buddy->free == True && free_buddy->merge_buddy[free_buddy->merge_buddy_count-1] == to_be_freed) {
        //printf("HEY\n");
        n ++;

        //to_be_freed->next = free_buddy->next;

        if (to_be_freed->next == free_buddy) {
            to_be_freed->next = free_buddy->next;
            to_be_freed->merge_buddy_count --;

            to_be_freed->size += 1;
            to_be_freed->free = True;

            free_buddy->size = 0;
            return my_free(to_be_freed, heapstart, n);
        }
        else if (free_buddy->next == to_be_freed) {
            free_buddy->next = to_be_freed->next;
            free_buddy->merge_buddy_count --;

            free_buddy->size += 1;
            free_buddy->free = True;

            to_be_freed->size = 0;
            return my_free(free_buddy, heapstart, n);
        }
        //return my_free(to_be_freed, heapstart, n);
    }
    if (n > 0) {
        return 0;
    }
    else if (n <= 0) {
        return 1;
    }
    return 0;

}

void * virtual_malloc(void * heapstart, uint32_t size) {
    // Your code here

    if (size == 0) {
        return NULL;
    }

    uint8_t initial_size = *((uint8_t*) heapstart);
    uint8_t min_size = *((uint8_t*) heapstart + 1);
    //printf("min size: %d\n", min_size);

    //uint8_t* structure_count = ((uint8_t *)heapstart + 2);

    uint8_t requested_size = size_to_power(size);
    if (requested_size <= min_size) {
        requested_size = min_size;
    }
    //printf("hey\n");

    struct block* c = (struct block*) (((char*)heapstart) + ((int) 4 + power(2, initial_size)));
    //printf("hey: %d\n", c->size);
    //printf("hey\n");
    //int j = 0;
    while(c != NULL) {
        if (c->free == True) {
            //printf("%d\n", min_size);
            if (c->size == min_size) {
                if (min_size >= requested_size) {
                    c->free = False;
                    return c->data;
                }
            }
            //printf("%d\n", requested_size);
            if (c->size == requested_size) {
                c->free = False;
                return c->data;
            }
        }
        //printf("%d\n", j);
        //j ++;
        c = c->next;
    }
    //printf("Owen");
    
    for (int i = requested_size + 1; i <= initial_size; i ++) {
        c = (struct block*) (((char*)heapstart) + ((int) 4 + power(2, initial_size)));
        c = find_closest_block_unallocated(c, i);
        //printf("size %d\n", c->size);
        if (c) {
            c = split_block(c, i - requested_size, heapstart);

            //struct block* b = (struct block*) (((char*)heapstart) + ((int) 3 + power(2, initial_size)));
            
            
            if (c->free == True) {
                if (c->size == power(2, min_size)) {
                    if (min_size >= requested_size) {
                        c->free = False;
                        //printf("%p\n", c->data);
                        return c->data;
                    }
                }

                if (c->size == requested_size) {
                    c->free = False;
                    //printf("%p\n", c->data);
                    return c->data;
                }
            }
        }
    }   


    return NULL;
}

int virtual_free(void * heapstart, void * ptr) {
    // Your code here

    //int ret; 
    if (ptr == NULL) {
        return 1;
    }


    uint8_t initial_size = *((uint8_t*) heapstart);
    //uint8_t min_size = *((uint8_t*) heapstart + 1);
    uint16_t* structure_count = ((uint16_t *)heapstart + 1);
    //struct block* c = (struct block*) (((char*)heapstart) + ((int) 3 + power(2, initial_size)));

    for (int i = 0; i < *structure_count; i ++) {
        if (((struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (i * sizeof(struct block))))->data == (struct block*) ptr) {
            struct block* free = ((struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (i * sizeof(struct block))));
            //struct block* free_buddy
            //printf("lol\n");
            if(free->free == True) {
                return 1;
            }

            //ret = my_free(free, heapstart, 0);
            my_free(free, heapstart, 0);
            // if (free->merge_buddy[free->merge_buddy_count-1]->free == True && free->merge_buddy[free->merge_buddy_count-1]->merge_buddy[free->merge_buddy[free->merge_buddy_count-1]->merge_buddy_count-1] == free) {
            //     printf("LOL\n");
            // }

            //printf("%c\n", ((struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (3*sizeof(uint8_t)) + (i * sizeof(struct block))))->name);
            return 0;
        }
    }
    

    return 1;
}

void * virtual_realloc(void * heapstart, void * ptr, uint32_t size) {
    // Your code here
    //printf("%d\n", size);

    if (ptr == NULL) {
        return virtual_malloc(heapstart, size);
    }

    if (size == 0) {
        virtual_free(heapstart, ptr);
        return NULL;
    }
    uint8_t initial_size = *((uint8_t*) heapstart);
    //uint8_t min_size = *((uint8_t*) heapstart + 1);
    uint16_t* structure_count = ((uint16_t *)heapstart + 1);
    struct block* reallocating;

    int can_fit = False;
    uint32_t size_count = 0;
    
    for (int i = 0; i < *structure_count; i ++) {
        if (((struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (i * sizeof(struct block))))->data == (struct block*) ptr) {
            reallocating = ((struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (i * sizeof(struct block))));
            reallocating->free = True;
            break;
        }
    }

    //printf("size: %d\n", size);
    //virtual_info(heapstart);
    for (int i = 0; i < *structure_count; i ++) {
        if (((struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (i * sizeof(struct block))))->free == True) {
            size_count += power(2, ((struct block*) (((char*)heapstart) + ((int)power(2, initial_size)) + (4*sizeof(uint8_t)) + (i * sizeof(struct block))))->size);
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

    reallocating->free = False;
    //uint8_t size_converted_power = size_to_power(size);
    if (can_fit == True) {
        virtual_free(heapstart, ptr);
        //virtual_info(heapstart);
        //printf("-----------\n");
        void* new_ptr = virtual_malloc(heapstart, size);
        //virtual_info(heapstart);
        //printf("hey\n");
        memmove(new_ptr, ptr, size);
        //printf("hey\n");
        return new_ptr;
    } 
    else if (can_fit == False) {
        return NULL;
    }



    return NULL;
}

void virtual_info(void * heapstart) {
    // Your code here
    //printf("size: %d\n", ((struct block*)((char*) heapstart + 18))->size);
    uint8_t initial_size = *((uint8_t*) heapstart);
    //uint32_t initial_size = power(2, );
    //printf("hey %d\n", initial_size);
    //printf("hey\n");
    struct block* c = (struct block*) (    (   (char*)   heapstart) + ((int) 4 + power(2, initial_size)));
    while(c != NULL) {
        if (c->free == True) {
            printf("free %d\n", power(2, c->size));
        }
        else {
            printf("allocated %d\n", power(2, c->size));
        }
        c = c->next;
    }
}
 
// int main() {
//     void* virtual_heap = malloc(400000000);
//     init_allocator(virtual_heap, 8, 1);
//     //void* ptr = virtual_malloc(virtual_heap, 4);

//     //virtual_info(virtual_heap);

//     void* arr[600];

//     for (int i = 1; i < 5; i ++) {
//         arr[i] = virtual_malloc(virtual_heap, 1);
//         //printf("%d\n", i);
//     }
//     virtual_info(virtual_heap);

//     for (int i = 0; i < 5; i ++) {
//         virtual_free(virtual_heap, arr[i]);
//         //virtual_info(virtual_heap);
//     }

//     //virtual_info(virtual_heap);
//     // printf("----------\n");
//     // //void* ptr1 = virtual_realloc(virtual_heap, ptr, 256);
//     // virtual_info(virtual_heap);
// }


