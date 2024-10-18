#include "myalloc.h"
#include <time.h>
#include <stdlib.h> 

void my_alloc_test(void)
{
    init_memory();

    char* error_tab1 = (char*)my_alloc(200);
    char* error_tab2 = (char*)my_alloc(sizeof(block_t));

    char* valid_tab1 = (char*)my_alloc(SIZE_BLK_SMALL);
    char* valid_tab2 = (char*)my_alloc(0);
    char* valid_tab3 = (char*)my_alloc(57);

    valid_tab1 != NULL ? VALID : ERROR;
    valid_tab2 != NULL ? VALID : ERROR;
    valid_tab3 != NULL ? VALID : ERROR;

    (size_t)ptr_head(valid_tab1)->head %2 == 1 ? VALID : ERROR;
    (size_t)ptr_head(valid_tab2)->head %2 == 1 ? VALID : ERROR;
    (size_t)ptr_head(valid_tab3)->head %2 == 1 ? VALID : ERROR;

    error_tab1 == NULL ? VALID : ERROR;
    error_tab2 == NULL ? VALID : ERROR;

    for(int i = 0; i < MAX_SMALL-3; i++)// -3 car déjà 3 blocs alloués
    {
        (char*)my_alloc(0);
    }

    char* error_tab3 = (char*)my_alloc(3);
    error_tab3 == NULL ? VALID : ERROR;
}

void free_test(void)
{
    init_memory();
    print_memory();
    char* tab = (char*)my_alloc(57);

    ptr_head(tab)->head % 2 == 1 ? VALID : ERROR;

    print_memory();

    my_free(tab);

    print_memory();

    ptr_head(tab)->head % 2 == 0 && head() == ptr_head(tab) ? VALID : ERROR;

    char* error_tab1 = (char*)my_alloc(200);
    my_free(error_tab1);

    char* error_tab2 = (char*)my_alloc(16);
    error_tab2 ++;
    my_free(error_tab2);

    char* error_tab3 = (char*)my_alloc(23);
    my_free(error_tab3);
    my_free(error_tab3);
}

void realloc_test(void)
{
    init_memory();
    char* tab1 = (char*)my_alloc(57);
    char* tab2 = (char*)my_realloc(tab1, 100);

    char* tab3 = (char*)my_alloc(57);
    print_memory();
    char* tab4 = (char*)my_realloc(tab3, sizeof(block_t));
    print_memory();
    tab1 == tab2 ? VALID : ERROR;
    tab4 == NULL ? VALID : ERROR;
    tab3 != NULL ? VALID : ERROR;

}

void add_value_test(void)
{
    init_memory();
    char* tab1 = (char*)my_alloc(57);
    tab1[0] == 0 ? VALID : ERROR;
    tab1[0] = 10;
    tab1[0] == 10 ? VALID : ERROR;
}

void time_test(void)
{
    init_memory();
    clock_t start = clock();
    for(int i=0; i<MAX_SMALL; i++) my_alloc(100);
    clock_t end = clock();
    printf("my_alloc time: %ld s\n",(end-start)*1000/CLOCKS_PER_SEC);
    init_memory();
    start = clock();
    for(int i=0; i<MAX_SMALL; i++) malloc(100);
    end = clock();
    printf("malloc time: %ld s\n",(end-start)*1000/CLOCKS_PER_SEC);
}

void out_of_tab_test()
{
    init_memory();
    char* tab1 = (char*)my_alloc(57);
    char* tab2 = tab1-1;
    my_realloc(tab2,10)==NULL ? VALID : ERROR;
    my_free(tab2);
    tab2 = tab1-sizeof(block_t);
    my_realloc(tab2,10)==NULL ? VALID : ERROR;
    my_free(tab2);
    tab2 = tab1+128*sizeof(block_t);
    my_realloc(tab2,10)==NULL ? VALID : ERROR;
    my_free(tab2);
}

void random_test(void)
{
    init_memory();
    srand(time(NULL));
    
    int size_index = rand() % SIZE_BLK_SMALL;
    char* tab = my_alloc(size_index);
    for(int i=0; i<size_index; i++)
    {
        int value = rand() % 10;
        tab[i] = value;
        printf("%d | ",value);
    }
    printf("\n");
    for(int i=0; i<size_index; i++)
    {
        printf("%d | ",tab[i]);
    }
    printf("\n\n");
    int new_size = (size_index==0?0:size_index-1);
    char* tab_realloc = my_realloc(tab, new_size);
    for(int i=0; i<new_size; i++)
    {
        printf("%d | ",tab_realloc[i]);fflush(stdout);
    }
    printf("\n\n");
}

int main(void)
{
    // init_memory();
    // print_memory();
    // my_alloc_test();
    // print_memory();
    // free_test();
    // realloc_test();
    // add_value_test();
    // time_test();
    // out_of_tab_test();
     random_test();
    return 0;
}