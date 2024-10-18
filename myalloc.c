#include "myalloc.h"

void* my_alloc(size_t size)
{
    if(size <= SIZE_BLK_SMALL && list_block != 0)
    {
        block_t* tmp = (block_t*)list_block;
        next();
        tmp->head = (tmp->head)+1;
        return tmp->body;
    }
    return NULL;
}

void my_free(void* ptr)
{
    size_t new_small_tab = (size_t)small_tab;
    size_t new_ptr = (size_t)ptr;
    if(new_small_tab > new_ptr || (new_ptr > (new_small_tab + sizeof(block_t) * MAX_SMALL)))
    {
        printf("The pointer is outside the working memory area.\n");
    }else if((new_ptr - new_small_tab) % sizeof(block_t) != sizeof(size_t))
    {
        printf("The pointer isn't at the begining of a block.\n");
    }else if(is_free(ptr))
    {

        printf("The pointer is in an empty block.\n");
    }else
    {
        ptr_head(ptr)->head = (size_t)list_block;
        list_block = (size_t)ptr_head(ptr);
    }
}

void* my_realloc(void* ptr, size_t size)
{
    size_t new_small_tab = (size_t)small_tab;
    size_t new_ptr = (size_t)ptr;

    if(new_small_tab > new_ptr || (new_ptr > (new_small_tab + sizeof(block_t) * MAX_SMALL)))
    {
        return NULL;
    }else if((new_ptr - new_small_tab) % sizeof(block_t) != sizeof(size_t))
    {
        return NULL;
    }else if(size > SIZE_BLK_SMALL || is_free(ptr))
    {
        return NULL;
    }
    return ptr;
}

void print_memory(void)
{
    printf("WARNING: THE NEXT PART IS A PRINT OF MEMORY !!!!!\n");
    for(int i = 0; i < MAX_SMALL-1; i++)
    {
        printf("%p | ",(void*)small_tab[i].head);
    }
    printf("%p\n",(void*)small_tab[MAX_SMALL-1].head);
    printf("END OF PRINT !!!!!\n");
}

void init_memory(void)
{
    assert(sizeof(block_t)%sizeof(size_t) == 0 && sizeof(block_t)> SIZE_BLK_SMALL);
    for(int i=0; i<MAX_SMALL-1; i++)
    {
        small_tab[i].head = (size_t)&small_tab[i+1];
    }
    list_block = (size_t)small_tab;
    small_tab[MAX_SMALL-1].head = 0;
}

block_t* head(void)
{
    return (block_t*)list_block;
}

void next(void)
{
    block_t* tmp = (block_t*)list_block;
    list_block = (size_t)tmp->head;
}

int is_free(void* ptr)
{
    return ((size_t)(ptr_head(ptr)->head) % 2 ) == 0;
}

block_t* ptr_head(void* ptr)
{
    return (block_t*)((size_t *)ptr - 1);
}