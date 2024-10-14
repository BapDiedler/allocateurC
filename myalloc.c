#include "myalloc.h"

// void* my_alloc(size_t size)
// {
//     if(size <= SIZE_BLK_SMALL)
//     {
//         for(int i = 0; i < MAX_SMALL; i++)
//         {
//             if(IS_FREE(small_tab[i].body))
//             {
//                 small_tab[i].head += 1;
//                 return small_tab[i].body;
//             }
//         }
//     }
//     return NULL;
// }

void* my_alloc(size_t size)
{
    if(size <= SIZE_BLK_SMALL && list_block != NULL)
    {
        block_t* tmp = list_block;
        next();
        tmp->head = ALLOC_BLOCK;
        return tmp->body;
    }
    return NULL;
}

// void my_free(void* ptr)
// {
//     size_t new_small_tab = (size_t)small_tab;
//     size_t new_ptr = (size_t)ptr;

//     if(new_small_tab > new_ptr || (new_ptr > (new_small_tab + sizeof(block_t) * MAX_SMALL)))
//     {
//         printf("The pointer is outside the working memory area.\n");
//     }else if((new_ptr - new_small_tab) % sizeof(block_t) != sizeof(size_t))
//     {
//         printf("The pointer isn't at the begining of a block.\n");
//     }else if(IS_FREE(ptr))
//     {
//         printf("The pointer is in an empty block.\n");
//     }else
//     {
//         HEAD(ptr) -= 1;
//     }
// }

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
        ptr_head(ptr)->head = list_block;
        list_block = ptr_head(ptr);
    }
}

// void* my_realloc(void* ptr, size_t size)
// {
//     size_t new_small_tab = (size_t)small_tab;
//     size_t new_ptr = (size_t)ptr;

//     if(new_small_tab > new_ptr || (new_ptr > (new_small_tab + sizeof(block_t) * MAX_SMALL)))
//     {
//         return NULL;
//     }else if((new_ptr - new_small_tab) % sizeof(block_t) != sizeof(size_t))
//     {
//         return NULL;
//     }else if(size > SIZE_BLK_SMALL || IS_FREE(ptr))
//     {
//         return NULL;
//     }
//     return ptr;
// }

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
    for(int i=0; i<MAX_SMALL-1; i++)
    {
        small_tab[i].head = &small_tab[i+1];
    }
    list_block = small_tab;
    small_tab[MAX_SMALL-1].head = NULL;
}

block_t* head(void)
{
    return list_block;
}

void next(void)
{
    list_block = list_block->head;
}

int is_free(void* ptr)
{
    return ptr_head(ptr)->head != ALLOC_BLOCK;
}

block_t* ptr_head(void* ptr)
{
    return (block_t*)((size_t *)ptr - 1);
}