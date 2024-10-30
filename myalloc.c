#include "myalloc.h"

block_t small_tab[MAX_SMALL];   // array with smalls blocks (char small_tab[MAX_SMALL * 128] was an other option for this one)
size_t small_free;              // first free small block
size_t big_free;                // first free large block
size_t init_mem = 1;            // memory state

/**
 * @brief initialise memory
 */
void init_memory(void)
{
    // initialise first large block
    if((big_free = (size_t)sbrk(SIZE_FIRST_BLK_LARGE)) == (size_t)-1)
    {
        ERROR;
        exit(1);
    }
    block_t* tmp = (block_t*)big_free;
    tmp->head = 0;
    tmp->size = SIZE_FIRST_BLK_LARGE - 2*sizeof(size_t);

    // initialise smalls blocks
    for(int i=0; i<MAX_SMALL-1; i++)
    {
        small_tab[i].head = (size_t)&small_tab[i+1];
        small_tab[i].size = SIZE_BLK_SMALL;
    }
    small_free = (size_t)small_tab;
    small_tab[MAX_SMALL-1].head = 0;

    init_mem = 0;
}

/**
 * @brief       allocate size bytes and return a pointer to the allocated memory
 * @param size  type size_t : nomber of bytes
 * @return      pointer to empty memory area
 */
void* my_alloc(size_t size)
{
    if(init_mem) init_memory(); // Initialise memory area


    if(size > SIZE_BLK_SMALL)   // allocation de large bloc
    {   
        if(size % sizeof(size_t) != 0)  // change size to be div by sizeof(size_t)
            size = sizeof(size_t) * ((size / sizeof(size_t)) + 1);

        block_t* prev_ptr = NULL;
        block_t* ptr = (block_t* )big_free;
        while(ptr != NULL)  // search the first free block which can countain size bytes
        {
            if(ptr->size >= size)   // ptr can countain size bytes
			{
				if(ptr->size < size + 2*sizeof(size_t) +SIZE_BLK_SMALL) // no fragmentation
				{
					if(prev_ptr != NULL)
						prev_ptr->head = ptr->head;
					else
						big_free = (size_t)ptr->head;
					ptr->head = 1;
					return (void*)ptr->body;
				}
				else    // fragmentation of memory
				{
                    size_t k = size + 2*sizeof(size_t);
                    ptr->size -= k;
                    ptr->head = 0;
					ptr = (block_t*)((__uint8_t*)ptr + ptr->size);
					ptr->head = 1;
					ptr->size = size;
					return (void*)ptr->body;
				}
			}
			prev_ptr = ptr;	
			ptr = (block_t*)ptr->head;
        }

        // if big_free doesn't have blocks to countain size bytes
		if((ptr = (block_t*)sbrk(size+2*sizeof(size_t))) == (void*)-1)
		{
            ERROR;
			return NULL;
		}
		ptr->head = 1;
		ptr->size = size;
		return (void*)ptr->body;
    }

    // if small block can countain size bytes
    block_t* tmp;
    if(small_free != 0) // small_free isn't an empty list
    {
        tmp = (block_t*)small_free;
        small_free = (size_t)tmp->head;
    }else if((tmp = (block_t*)sbrk(SIZE_BLK_SMALL+2*sizeof(size_t))) == (void*)-1)  // we don't have small block
    {
        ERROR;
        return NULL;
    }
    tmp->head = 1;
    tmp->size = SIZE_BLK_SMALL;
    return tmp->body;
}

/**
 * @brief       frees the memory space pointed to by ptr
 * @param ptr   type void* : pointer to memory area
 */
void my_free(void* ptr)
{
    if(init_mem) init_memory();   // Initialise memory area

    size_t new_ptr = (size_t)ptr_head(ptr);

    // if ptr is at the end of memory area
    if(ptr_head(ptr)->size + new_ptr == (size_t)sbrk(0))
    {
        ptr_head(ptr)->head = 0;
        sbrk(-ptr_head(ptr)->size - 2*sizeof(size_t));
        return;
    }

    if(is_free(ptr))    // unfree block
    {
        printf("The pointer is in an empty block.\n");
        return;
    }

    // ptr is out of small_tab
    size_t new_small_tab = (size_t)small_tab;
    if((new_ptr >= new_small_tab && new_ptr <= (new_small_tab + sizeof(block_t) * MAX_SMALL)))
    {
        if((new_ptr - new_small_tab) % sizeof(block_t) != 2*sizeof(size_t)) // begin of small block
            printf("The pointer isn't at the begining of small block.\n");
        else
        {   // ptr could be free
            ptr_head(ptr)->head = (size_t)small_free;
            small_free = (size_t)ptr_head(ptr);
        }
        return;
    }

    // we don't have free large block
    if(big_free == 0)
    {
        ptr_head(ptr)->head = 0;
        big_free = (size_t)ptr_head(ptr);
        return;
    }

    // defragmentation of memory area
    block_t* large_ptr = (block_t*)big_free;
    block_t* prev_large_ptr = NULL;
    while(large_ptr != NULL)    // while we're not at the end of list
    {
        if(((__uint8_t*)large_ptr + large_ptr->size + 2*sizeof(size_t)) == (__uint8_t*)ptr_head(ptr))   // before ptr
        {
            large_ptr->size += ptr_head(ptr)->size + 2*sizeof(size_t);
            ptr_head(ptr)->head = 0;
            return;
        }
        if(((__uint8_t*)ptr_head(ptr) + ptr_head(ptr)->size + 2*sizeof(size_t)) == (__uint8_t*)large_ptr)  // after ptr
        {
            ptr_head(ptr)->size += large_ptr->size + 2*sizeof(size_t);
            ptr_head(ptr)->head = large_ptr->head;
            if(prev_large_ptr != NULL)
                prev_large_ptr->head = (size_t)ptr_head(ptr);
            else
                big_free = (size_t)ptr_head(ptr);
            return;
        }

        // next block
        prev_large_ptr = large_ptr;	
        large_ptr = (block_t*)large_ptr->head;
    }

    // no defragmentation with ptr
    ptr_head(ptr)->head = (size_t)big_free;
    big_free = (size_t)ptr_head(ptr);
}

/**
 * @brief       changes the size of the memory block pointed to by ptr to size bytes
 * @param ptr   type void* : pointer to memory area
 * @param size  type size_t : memory size
 * @return      pointer to memory area with sames values than ptr
 */
void* my_realloc(void* ptr, size_t size)
{
    if(init_mem) init_memory();   // Initialise memory area

    if (ptr == NULL || is_free(ptr))    // invalid pointer or free block
        return NULL;

    size_t new_small_tab = (size_t)small_tab;
    size_t new_ptr = (size_t)ptr;
    if(new_small_tab > new_ptr || size >= SIZE_BLK_SMALL || (new_ptr > (new_small_tab + sizeof(block_t) * MAX_SMALL)))  // out of small tab memory
    {
        if(size >= ptr_head(ptr)->size) // size bigger than size's ptr
        {
            my_free(ptr);
            new_ptr = (size_t)my_alloc(size);
            malloc_copy_free((__uint8_t*)new_ptr, (__uint8_t*)ptr);
            return (void*)new_ptr;
        }
        ptr_head(ptr)->size = size;
        return ptr;
    }
    if((new_ptr - new_small_tab) % sizeof(block_t) != 2*sizeof(size_t)) // ptr isn't a small block
        return NULL;
    return ptr;
}

/**
 * @brief copy ptr2 in ptr1
 */
void malloc_copy_free(__uint8_t* ptr1, __uint8_t* ptr2)
{
    for (size_t i = 0; i < ptr_head(ptr2)->size; i++)
    {
        ptr1[i] = ptr2[i];
    }
}

/**
 * @brief print smalls frees and unfrees blocks
 */
void print_small_memory(void)
{
    printf("WARNING: THE NEXT PART IS A PRINT OF MEMORY !!!!!\n");
    for(int i = 0; i < MAX_SMALL-1; i++)
        printf("%p | ",(void*)small_tab[i].head);
    printf("%p\n",(void*)small_tab[MAX_SMALL-1].head);
    printf("END OF PRINT !!!!!\n");
}

/**
 * @brief print larges frees blocks
 */
void print_large_memory(void)
{
    printf("WARNING: THE NEXT PART IS A PRINT OF MEMORY !!!!!\n");
    block_t* tmp = (block_t*)big_free;

    while(tmp != NULL)
    {
        printf("%ld: (%ld,%ld)\n",(size_t)tmp,tmp->head,tmp->size);
        tmp = (block_t*)tmp->head;
    }
    printf("END OF PRINT !!!!!\n");
}

/**
 * @brief       test pour savoir si un bloc est libre
 * @param ptr   void* : pointeur à tester
 * @return      0 si libre
 */
int is_free(void* ptr)
{
    return ((size_t)(ptr_head(ptr)->head) % 2 ) == 0;
}

/**
 * @brief accès au pointeur vers la tête d'un bloc
 * @param ptr : void* poiteur du tableau d'un bloc
 * @return block_t* pointeur de la tête du bloc
 */
block_t* ptr_head(void* ptr)
{
    return (block_t*)((size_t *)ptr - 2);
}

/**
 * @brief size of the list big_free
 */
size_t size_big_free(void)
{
    size_t res = 0;
    block_t* tmp = (block_t*) big_free;
    while(tmp != NULL)
    {
        res++;
        tmp = (block_t*)tmp->head;
    }
    return res;
}