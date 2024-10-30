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
    if((small_tab = (block_t*)sbrk(MAX_SMALL*(MAX_SMALL+2*sizeof(size_t)))) == (size_t)-1)
    {
        ERROR;
        exit(1);
    }
    for(int i=0; i<MAX_SMALL-1; i++)
    {
        small_tab[i].head = (size_t)&small_tab[i+1];
        small_tab[i].size = SIZE_BLK_SMALL;
    }
    small_free = (size_t)small_tab;
    small_tab[MAX_SMALL-1].head = 0;

    init_mem = 0;
}

void* my_alloc(size_t size)
{
    if(init_mem)    // Initialisation de la mémoire
        init_memory();


    if(size > SIZE_BLK_SMALL)   //allocation de large bloc
    {   
        if(size % sizeof(size_t) != 0)
            size = sizeof(size_t) * ((size / sizeof(size_t)) + 1);

        large_block_t *prev_ptr = NULL;
        large_block_t *ptr = (large_block_t *)big_free;
        while(ptr != NULL) //recherche du premier bloc libre de bonne taille
        {
            if(ptr->size >= size)
			{
				// Here the block is small enough for keeping it intact

				if(ptr->size < size + 2*sizeof(size_t) +SIZE_BLK_SMALL)
				{
					if(prev_ptr != NULL)
					{
						prev_ptr->head = ptr->head;
					}
					else
					{
						big_free = (size_t)ptr->head;
					}
					ptr->head = 1;
					return (void*)ptr->body;
				}
				else
				{
					// Else, the block is split into two parts to avoid fragmentation
                    size_t k = size + 2*sizeof(size_t);
                    ptr->size -= k;
                    ptr->head = 0;
					large_block_t* newBlock = (large_block_t*)((char*)ptr + ptr->size);
					newBlock->head = 1;
					newBlock->size = size;
					return (void*)newBlock->body;
				}
			}
			prev_ptr = ptr;	
			ptr = (large_block_t*)ptr->head;
            // if(ptr->size > size)
            //     break;
            // *prev_ptr = ptr;
            // ptr = (large_block_t *)ptr->head;
        }

        // If no block large enough is found, memory is allocated on the heap
		large_block_t* newBlock = (large_block_t*)sbrk(size+2*sizeof(size_t));
		if(newBlock == (void*)-1)
		{
			printf("ERROR : no memory available on the heap.\n");
			return NULL;
		}
		newBlock->head = 1;
		newBlock->size = size;

		
		return (void*)newBlock->body;

        // if(ptr == NULL)    //ajout de mémoire
        // {
        //     if((ptr = (large_block_t *)sbrk(size+2*sizeof(size_t))) == (void*)-1)
        //     {
        //         ERROR;
        //         return NULL;
        //     }
        //     ptr->head = 1;
        //     ptr->size = size;
        // }else if(ptr->size < size + 2*sizeof(size_t) + SIZE_BLK_SMALL)  //le bloc trouvé est de bonne taille
        // {
        //     if(*prev_ptr == (large_block_t *)big_free){
        //         if(big_free == (size_t)ptr)
        //             big_free = ptr->head;
        //     }else
        //         (*prev_ptr)->head = ptr->head;
        //     ptr->head += 1;
        // }else   //le bloc trouvé est trop grand (découpage de la mémoire)
        // {
        //     (*prev_ptr)->head = (size_t)ptr->head;
        //     size_t k = size + 2*sizeof(size_t);
        //     ptr->size -= k;
        //     ptr = (large_block_t *)((__uint8_t *)ptr + ptr->size);
        //     ptr->head = 1;
        //     ptr->size = k-2*sizeof(size_t);
        // }
        // return &ptr->body;
    }
    if(size <= SIZE_BLK_SMALL && small_free != 0)   //bloc de petite taille
    {
        block_t* tmp = (block_t*)small_free;
        next();
        tmp->head += 1;
        tmp->size = size;
        return tmp->body;
    }else if(small_free == 0 && size <= SIZE_BLK_SMALL){
        if((small_free = (size_t)sbrk(SIZE_BLK_SMALL+2*sizeof(size_t))) == (size_t)-1)
        {
            ERROR;
            return NULL;
        }
        block_t* tmp = (block_t*)small_free;
        small_free = 0;
        tmp->head = 1;
        tmp->size = size;
        return tmp->body;
    }
    return NULL;

}

void my_free(void* ptr)
{
    size_t new_small_tab = (size_t)small_tab;
    size_t new_ptr = (size_t)ptr;
    if(new_ptr + SIZE_BLK_SMALL == (size_t)sbrk(0))
    {
        ptr_head(ptr)->size=0;
        ptr_head(ptr)->head=0;
        sbrk(-SIZE_BLK_SMALL - 2*sizeof(size_t));
    }else if(ptr_head(ptr)->size + new_ptr == (size_t)sbrk(0))
    {
        ptr_head(ptr)->size=0;
        ptr_head(ptr)->head=0;
        sbrk(-ptr_head(ptr)->size - 2*sizeof(size_t));
    }
    else if((new_ptr >= new_small_tab && new_ptr <= (new_small_tab + sizeof(block_t) * MAX_SMALL)))
    {
        if(new_small_tab > new_ptr || (new_ptr > (new_small_tab + sizeof(block_t) * MAX_SMALL)))
            printf("The pointer is outside the working memory area.\n");
        else if((new_ptr - new_small_tab) % sizeof(block_t) != 2*sizeof(size_t))
            printf("The pointer isn't at the begining of small block.\n");
        else if(is_free(ptr))
            printf("The pointer is in an empty block.\n");
        else
        {
            ptr_head(ptr)->head = (size_t)small_free;
            small_free = (size_t)ptr_head(ptr);
        }
    }else
    {
        if(is_free(ptr))
            printf("The pointer is in an empty block.\n");
        else
        {
            // large_block_t* tmp = (large_block_t*)big_free;
            // if(tmp == NULL)
            // {
            //     ptr_head(ptr)->head = big_free;
            //     big_free = (size_t)ptr_head(ptr);
            // }else if((size_t)tmp + tmp->size + 2*sizeof(size_t) == (size_t)ptr_head(ptr))
            // {
            //     tmp->size += 2*sizeof(size_t) + ptr_head(ptr)->size ;
            //     ptr_head(ptr)->head = 0;
            // }else if((size_t)ptr_head(ptr)->size + (size_t)ptr == (size_t)tmp)
            // {
            //     ptr_head(ptr)->size += 2*sizeof(size_t) + tmp->size;
            //     ptr_head(ptr)->head = big_free;
            //     tmp->head=0;
            //     big_free = (size_t)ptr_head(ptr);
            // }else
            // {
            //     tmp = (large_block_t*)tmp->head;
            //     large_block_t* before_tmp = (large_block_t*)big_free;
                
            //     while(tmp != NULL && (size_t)tmp + tmp->size + 2*sizeof(size_t) != (size_t)ptr_head(ptr) && ptr_head(ptr)->size + (size_t)ptr != (size_t)tmp)
            //     {
            //         before_tmp = tmp;
            //         tmp = (large_block_t*)tmp->head;
            //     }
            //     if(tmp == NULL)
            //     {
            //         ptr_head(ptr)->head = big_free;
            //         big_free = (size_t)ptr_head(ptr);
            //     }
            //     else if((size_t)tmp + tmp->size + 2*sizeof(size_t) == (size_t)ptr_head(ptr))
            //     {
            //         tmp->size += 2*sizeof(size_t) + ptr_head(ptr)->size ;
            //         ptr_head(ptr)->head = 0;
            //     }else if(ptr_head(ptr)->size + (size_t)ptr == (size_t)tmp)
            //     {
            //         ptr_head(ptr)->size += 2*sizeof(size_t) + tmp->size ;
            //         before_tmp->head = tmp->head;
            //         ptr_head(ptr)->head = big_free;
            //         big_free = (size_t)ptr_head(ptr);
            //     }
            //     // ptr_head(ptr)->head = big_free;
            //     // big_free = (size_t)ptr_head(ptr);
            // }
            // large_block_t* freeBlock = (large_block_t*)ptr_head(ptr);

            // ptr_head(ptr)->head = big_free;
            // big_free = (size_t)ptr_head(ptr);
            // // Here I don't check if the address points to the start of the block (With the current global variables, I don't think there is any ways of doing it)

            large_block_t* freeBlock = (large_block_t*)ptr_head(ptr);
            if(!is_free(ptr))
            {
                if(big_free == 0)
                {
                    big_free = (size_t)freeBlock;
                    ((large_block_t*)big_free)->head = 0;
                    return;
                }


                large_block_t* currentLargeBlock = (large_block_t*)big_free;
                large_block_t* prevLargeBlock = NULL;

                // I loop over every free block to check if it is adjacent to the block that need to be freed (avoid memory fragmentation)
                while(currentLargeBlock != NULL)
                {
                    if( ((char*)currentLargeBlock + currentLargeBlock->size + 2*sizeof(size_t)) == (char*)freeBlock )
                    {
                        currentLargeBlock->size += freeBlock->size + 2*sizeof(size_t);
                        freeBlock->head = 0;
                        return;
                    }


                    if( ((char*)freeBlock + freeBlock->size + 2*sizeof(size_t)) == (char*)currentLargeBlock )
                    {
                        freeBlock->size += currentLargeBlock->size + 2*sizeof(size_t);
                        freeBlock->head = currentLargeBlock->head;
                        if(prevLargeBlock != NULL)
                        {
                            prevLargeBlock->head = (size_t)freeBlock;
                        }
                        else
                        {
                            big_free = (size_t)freeBlock;
                        }
                        return;
                    }

                    prevLargeBlock = currentLargeBlock;	
                    currentLargeBlock = (large_block_t*)currentLargeBlock->head;
                }

                // If no adjacent block found, simply add the block that nedd to be freed to the big_free list
                freeBlock->head = (size_t)big_free;
                big_free = (size_t)freeBlock;
            }
        }
    }
}

size_t size_big_free(void)
{
    size_t res = 0;
    large_block_t* tmp = (large_block_t*) big_free;
    while(tmp != NULL)
    {
        res++;
        tmp = (large_block_t*)tmp->head;
    }
    return res;
}

void malloc_copy_free(char *ptr1, char *ptr2)
{
    for (size_t i = 0; i < ptr_head(ptr2)->size; i++)
    {
        ptr1[i] = ptr2[i];
    }
}


void* my_realloc(void* ptr, size_t size)
{
    size_t new_small_tab = (size_t)small_tab;
    size_t new_ptr = (size_t)ptr;
    if(new_small_tab > new_ptr || size >= SIZE_BLK_SMALL || (new_ptr > (new_small_tab + sizeof(block_t) * MAX_SMALL))) 
    {
        if (ptr == NULL || is_free(ptr))
        {
            return NULL;
        }else
        {
            if(size >= ptr_head(ptr)->size)
            {
                my_free(ptr);
                new_ptr = (size_t)my_alloc(size);
                malloc_copy_free((char*)new_ptr, (char*)ptr);
                return (void*)new_ptr;
            }
            ptr_head(ptr)->size = size;
            return ptr;
        }
    }
    if((new_ptr - new_small_tab) % sizeof(block_t) != 2*sizeof(size_t))
        return NULL;
    if(is_free(ptr))
        return NULL;
    return ptr;
}

void print_small_memory(void)
{
    printf("WARNING: THE NEXT PART IS A PRINT OF MEMORY !!!!!\n");
    for(int i = 0; i < MAX_SMALL-1; i++)
        printf("%p | ",(void*)small_tab[i].head);
    printf("%p\n",(void*)small_tab[MAX_SMALL-1].head);
    printf("END OF PRINT !!!!!\n");
}

void print_large_memory(void)
{
    printf("WARNING: THE NEXT PART IS A PRINT OF MEMORY !!!!!\n");
    large_block_t* tmp = (large_block_t*)big_free;

    while(tmp != NULL)
    {
        printf("(%ld -> %ld,%ld) |\n",(size_t)tmp,tmp->head,tmp->size);
        tmp = (large_block_t*)tmp->head;
    }
    printf("end \n");
    printf("END OF PRINT !!!!!\n");
}

block_t* head(void)
{
    return (block_t*)small_free;
}

large_block_t* head_big_free(void)
{
    return (large_block_t*)big_free;
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