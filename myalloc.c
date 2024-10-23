#include "myalloc.h"

void init_memory(void)
{
    if((big_free = (size_t)sbrk(SIZE_FIRST_BLK_LARGE)) == (size_t)-1)
    {
        ERROR;
        exit(1);
    }
    large_block_t* tmp = (large_block_t*)big_free;
    tmp->head = 0;
    tmp->size = SIZE_FIRST_BLK_LARGE - 2*sizeof(size_t);
    for(int i=0; i<MAX_SMALL-1; i++)
    {
        small_tab[i].head = (size_t)&small_tab[i+1];
        small_tab[i].size = SIZE_BLK_SMALL;
    }
    small_free = (size_t)small_tab;
    small_tab[MAX_SMALL-1].head = 0;
}

void* my_alloc(size_t size)
{
    if(size > SIZE_BLK_SMALL)   //allocation de large bloc
    {
        if(size % sizeof(size_t) != 0)  //taille multiple de size_t
            size = sizeof(size_t) * ((size / sizeof(size_t)) + 1);

        large_block_t *prev_ptr = (large_block_t *)big_free;
        large_block_t *ptr = (large_block_t *)big_free;
        while(ptr != 0) //recherche du premier bloc libre de bonne taille
        {
            if(ptr->size > size + 2 * sizeof(size_t))
                break;
            prev_ptr = ptr;
            ptr = (large_block_t *)ptr->head;
        }

        if(ptr == 0)    //ajout de mémoire
        {
            if((ptr = (large_block_t *)sbrk(size+2*sizeof(size_t))) == (void*)-1)
            {
                ERROR;
                return NULL;
            }
            ptr->head += 1;
            ptr->size = size + 2 * sizeof(size_t);
            ptr->body = (__uint8_t*)((size_t)ptr->size + 1);
        }else if(ptr->size < size + 2*sizeof(size_t) + SIZE_BLK_SMALL)  //le bloc trouvé est de bonne taille
        {
            if(prev_ptr == (large_block_t *)big_free)
                big_free = (size_t)ptr;
            prev_ptr = ptr;
            ptr->head += 1;
            ptr->body = (__uint8_t*)((size_t)ptr->size + 1);
        }else   //le bloc trouvé est ttrop grand (découpage de la mémoire)
        {
            size_t k = size + 2*sizeof(size_t);
            ptr->size -= k;
            ptr = (large_block_t *)((__uint8_t *)ptr + ptr->size);
            ptr->head = 1;
            ptr->size = k;
        }
        return &ptr->body;
    }
    if(size <= SIZE_BLK_SMALL && small_free != 0)   //bloc de petite taille
    {
        block_t* tmp = (block_t*)small_free;
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
    if(new_ptr >= new_small_tab && new_ptr <= (new_small_tab + sizeof(block_t) * MAX_SMALL))
    {
        if(new_small_tab > new_ptr || (new_ptr > (new_small_tab + sizeof(block_t) * MAX_SMALL)))
        {
            printf("The pointer is outside the working memory area.\n");
        }else if((new_ptr - new_small_tab) % sizeof(block_t) != 2*sizeof(size_t))
        {
            printf("The pointer isn't at the begining of a block.\n");
        }else if(is_free(ptr))
        {
            printf("The pointer is in an empty block.\n");
        }else
        {
            ptr_head(ptr)->head = (size_t)small_free;
            small_free = (size_t)ptr_head(ptr);
        }
    }else
    {
        if (ptr_head(ptr)->head % 2 == 0)
        {
            printf("The pointer is in an large empty block.\n");
        }
        else
        {
            ptr_head(ptr)->head = big_free;
            big_free = (size_t)ptr_head(ptr);
        }
    }
}

void* my_realloc(void* ptr, size_t size)
{
    size_t new_small_tab = (size_t)small_tab;
    size_t new_ptr = (size_t)ptr;

    if(new_small_tab > new_ptr || (new_ptr > (new_small_tab + sizeof(block_t) * MAX_SMALL)))
    {
        if (ptr_head(ptr)->head % 2 == 0)
        {
            return NULL;
        }
        else
        {
            ptr_head(ptr)->size = size;
            return ptr;
        }
    }else if((new_ptr - new_small_tab) % sizeof(block_t) != sizeof(size_t))
    {
        return NULL;
    }else if(size > SIZE_BLK_SMALL || is_free(ptr))
    {
        return NULL;
    }
    return ptr;
}

void print_small_memory(void)
{
    printf("WARNING: THE NEXT PART IS A PRINT OF MEMORY !!!!!\n");
    for(int i = 0; i < MAX_SMALL-1; i++)
    {
        printf("%p | ",(void*)small_tab[i].head);
    }
    printf("%p\n",(void*)small_tab[MAX_SMALL-1].head);
    printf("END OF PRINT !!!!!\n");
}

void print_large_memory(void)
{
    printf("WARNING: THE NEXT PART IS A PRINT OF MEMORY !!!!!\n");
    large_block_t* tmp = (large_block_t*)big_free;

    while(tmp != 0)
    {
        printf("(%ld,%ld) |",tmp->head,tmp->size);
        tmp = (large_block_t*)tmp->head;
    }
    printf("end \n");
    printf("END OF PRINT !!!!!\n");
}

block_t* head(void)
{
    return (block_t*)small_free;
}

void next(void)
{
    block_t* tmp = (block_t*)small_free;
    small_free = (size_t)tmp->head;
}

int is_free(void* ptr)
{
    return ((size_t)(ptr_head(ptr)->head) % 2 ) == 0;
}

block_t* ptr_head(void* ptr)
{
    return (block_t*)((size_t *)ptr - 2);
}