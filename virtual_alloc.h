/*************************************************************************************
*    25 April 2021
*    COMP2017 Assignment 3 - VIRTUAL ALLOC
*    Owen Yang
*    500550430
*    DESCRIPTION: H FILE FOR VIRTUAL BUDDY ALLOCATOR
*************************************************************************************/

#ifndef alloc
#define alloc

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*************************************************************************************
* STRUCT DECLARATIONS.
*************************************************************************************/

struct __attribute__((__packed__)) block{
    /*************************************************************************************
     * STORES ALL THE INFORMATION - THE META DATA OF THE ALLOCATED BLOCK OF MEMORY
     * 
     * next: the next block of meta data for the next contiguous block of memory
     * size: the size of the block of memory (power of two)
     * buddy: the buddy it was broken off from, to help with freeing memory and merging
     * data: the data pointer where the actual data is stored in the heap
     * free: if the block is free
     * 
     *************************************************************************************/

    uint32_t next_num;
    uint8_t size;
    uint32_t buddy_num;
    uint32_t data_num;
    uint8_t free;
};


/*************************************************************************************
* FUNCTION DECLARATIONS.
*************************************************************************************/

/*
* Increases the base to the power or pow
*/
int power(int base, int pow);

/*
* Changes a integer to its closest greater power of 2
*/
int size_to_power(uint32_t num);

/*
* Finds the closest unallocated block of greater than the size requirements
*/
struct block* find_closest_block_unallocated(struct block* c, uint8_t size, void* heapstart);

/*
* Recursively splits the blocks down to the necessary size for malloc and creates associated meta data
*/
struct block* split_block(struct block* c, uint8_t n, void* heapstart);

/*
* recursively frees the block and merges with the buddy if available
*/
int my_free_new(struct block* to_be_free, void* heapstart, int n);

/*
* Allocates more heap space for the meta datastructure to expand
*/
void * virtual_sbrk(int32_t increment); 

/*
* Allocates the existing data structures and the heap space for the buddy allocator
*/
void init_allocator(void * heapstart, uint8_t initial_size, uint8_t min_size);

/*
* Allocates a block of memory greater than size and creates necessary meta data for it
*/
void * virtual_malloc(void * heapstart, uint32_t size);

/*
* Frees the given data, merging and clearing meta data associated with it
*/
int virtual_free(void * heapstart, void * ptr);

/*
* Reallocates previously allocated data to a new size requiring new piece of memory, updates meta data accordingly
*/
void * virtual_realloc(void * heapstart, void * ptr, uint32_t size);

/*
* Prints out the current state of data and memory in the buddy allocator
*/
void virtual_info(void * heapstart);

#endif