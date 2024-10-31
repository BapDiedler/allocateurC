#include "myalloc.h"
#include <time.h>
#include <stdlib.h> 
#include <string.h>

void my_alloc_test_small(void)
{
    int size = 10;

    /* TEST SUR UN MALLOC */
    init_memory();
    char* arr = my_alloc(size);

    assert(arr != NULL);
    assert(ptr_head(arr)->size == 10);
    assert(ptr_head(arr)->head % 2 == 1);
    for(int i=0; i<size; i++)
    {
        arr[i] = i;
    }
    for(int i=0; i<size; i++)
    {
        assert(ptr_head(arr)->body[i] == i);
    }

    /* TEST REMPLISSAGE ET DEPASSEMENT SMALL_TABLE */
    init_memory();
    char** arr2 = calloc((MAX_SMALL),sizeof(char*));    
    for(int i=0; i<MAX_SMALL+1; i++)
    {
        arr2[i] = my_alloc(size);
    }
    for(int i=0; i<MAX_SMALL; i++)
    {
        assert(arr2[i] != NULL);
        assert(ptr_head(arr2[i])->size == 10);
        assert(ptr_head(arr2[i])->head % 2 == 1);
    }
    assert((size_t)arr2[MAX_SMALL] + SIZE_BLK_SMALL == (size_t)sbrk(0));
    free(arr2);
}

void my_alloc_test_large(void)
{
    /* TEST SIMPLE BLOC LARGE */
    init_memory();
    size_t value_break_before = (size_t)sbrk(0);
    char* arr = my_alloc(2*SIZE_BLK_SMALL);
    assert(arr != NULL);
    assert(ptr_head(arr)->head % 2 == 1);
    assert(ptr_head(arr)->size >= 2*SIZE_BLK_SMALL);
    assert(ptr_head(arr)->size % sizeof(size_t) == 0);
    for(size_t i=0; i<ptr_head(arr)->size; i++)
    {
        arr[i] = (char)i;
    }
    for(size_t i=0; i<ptr_head(arr)->size; i++)
    {
        assert(arr[i] == (char)i);
    }
    size_t value_break_after = (size_t)sbrk(0);
    //printf("%ld, %ld\n",value_break_before,value_break_after);
    assert(value_break_after == value_break_before);

    /* TEST BLOC LARGE AVEC DEPASSEMENT*/
    init_memory();
    value_break_before = (size_t)sbrk(0);
    arr = my_alloc(2*SIZE_FIRST_BLK_LARGE);
    assert(arr != NULL);
    assert(ptr_head(arr)->head % 2 == 1);
    assert(ptr_head(arr)->size >= 2*SIZE_FIRST_BLK_LARGE);
    assert(ptr_head(arr)->size % sizeof(size_t) == 0);
    for(size_t i=0; i<ptr_head(arr)->size; i++)
    {
        arr[i] = (char)i;
    }
    for(size_t i=0; i<ptr_head(arr)->size; i++)
    {
        assert(arr[i] == (char)i);
    }
    value_break_after = (size_t)sbrk(0);
    assert(value_break_after-value_break_before == 2*SIZE_FIRST_BLK_LARGE + 2*sizeof(size_t));
}

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

    error_tab1 != NULL ? VALID : ERROR;
    error_tab2 != NULL ? VALID : ERROR;

    error_tab1 != error_tab2 ? VALID : ERROR;
    (size_t)error_tab1 - 1 >= 200 ? VALID : ERROR;
    (size_t)ptr_head(error_tab1)->head %2 == 1 ? VALID : ERROR;

    (size_t)error_tab2 - 1 >= sizeof(block_t) ? VALID : ERROR;
    (size_t)ptr_head(error_tab2)->head %2 == 1 ? VALID : ERROR;

    for(int i = 0; i < MAX_SMALL-3; i++)// -3 car déjà 3 blocs alloués
    {
        (char*)my_alloc(0);
    }

    char* error_tab3 = (char*)my_alloc(3);
    error_tab3 == NULL ? VALID : ERROR;

    char* arr1 = (char*)my_alloc(230);
    (size_t)arr1 - 1 >= 230 ? VALID : ERROR;
    (size_t)ptr_head(arr1)->head %2 == 1 ? VALID : ERROR;
    char* arr2 = (char*)my_alloc(1024);
    (size_t)arr2 - 1 >= 1024 ? VALID : ERROR;
    (size_t)ptr_head(arr2)->head %2 == 1 ? VALID : ERROR;
    char* arr3 = (char*)my_alloc(385);
    (size_t)arr3 - 1 >= 385 ? VALID : ERROR;
    (size_t)ptr_head(arr3)->head %2 == 1 ? VALID : ERROR;
    char* arr4 = (char*)my_alloc(1234);
    (size_t)arr4 - 1 >= 1234 ? VALID : ERROR;
    (size_t)ptr_head(arr4)->head %2 == 1 ? VALID : ERROR;
    print_large_memory();
}

void free_test_small(void)
{
    init_memory();
    char* arr = my_alloc(10);
    assert(arr != NULL);
    my_free(arr);
    assert(ptr_head(arr)->head % 2 == 0);

    init_memory();

    arr = my_alloc(10);
    assert(arr != NULL);
    arr++;

    FILE *temp = freopen("temp_output.txt", "w+", stdout);
    if (temp == NULL) {
        perror("Erreur lors de l'ouverture du fichier temporaire");
        exit(1);
    }

    my_free(arr);

    fseek(temp, 0, SEEK_SET);
    char buffer[1024];
    fread(buffer, sizeof(char), sizeof(buffer), temp);

    char* test = "The pointer isn't at the begining of small block.\n";
    assert(strncmp(buffer,test,30)==0);

    fclose(temp);
    remove("temp_output.txt");
    
    assert(ptr_head(--arr)->head % 2 == 1);

    init_memory();
    arr = my_alloc(10);
    assert(arr != NULL);
    arr+=MAX_SMALL*(2*sizeof(size_t)+SIZE_BLK_SMALL);
    temp = freopen("temp_output1.txt", "w+", stdout);
    if (temp == NULL) {
        perror("Erreur lors de l'ouverture du fichier temporaire");
        exit(1);
    }

    my_free(arr);

    fseek(temp, 0, SEEK_SET);
    buffer[0] = '\0';
    fread(buffer, sizeof(char), sizeof(buffer), temp);

    test = "The pointer is in an empty block.\n";
    printf("%d\n",strncmp(buffer,test,30));
    assert(strncmp(buffer,test,30)==0);

    fclose(temp);
    remove("temp_output1.txt");

    arr-=MAX_SMALL*(2*sizeof(size_t)+SIZE_BLK_SMALL);
    assert(ptr_head(arr)->head % 2 == 1);

    temp = freopen("temp_output1.txt", "w+", stdout);
    if (temp == NULL) {
        perror("Erreur lors de l'ouverture du fichier temporaire");
        exit(1);
    }

    my_free(arr);



    my_free(arr);

    fseek(temp, 0, SEEK_SET);
    buffer[0] = '\0';
    fread(buffer, sizeof(char), sizeof(buffer), temp);

    test = "The pointer is in an empty block.\n";
    printf("%d\n",strncmp(buffer,test,30));
    assert(strncmp(buffer,test,30)==0);

    fclose(temp);
    remove("temp_output1.txt");

}

void free_test_last_block(void)
{
    init_memory();
    char* arr[MAX_SMALL+10];
    printf("%ld\n",(size_t)sbrk(0));
    for(int i=0; i<MAX_SMALL; i++)
    {
        arr[i] = my_alloc(10);
    }
    printf("%ld\n",(size_t)sbrk(0));
    for(int i=MAX_SMALL; i<MAX_SMALL+11; i++)
    {
        arr[i] = my_alloc(10);
    }
    for(int i=MAX_SMALL+10; i>MAX_SMALL-1; i--)
    {
        my_free(arr[i]);
    }
    printf("%ld\n",(size_t)sbrk(0));
}

void free_test_large(void)
{
    init_memory();
    char*arr = my_alloc(SIZE_FIRST_BLK_LARGE);
    assert(arr != NULL);
    arr++;

    FILE *temp = freopen("temp_output.txt", "w+", stdout);
    if (temp == NULL) {
        perror("Erreur lors de l'ouverture du fichier temporaire");
        exit(1);
    }

    my_free(arr);

    fseek(temp, 0, SEEK_SET);
    char buffer[1024];
    fread(buffer, sizeof(char), sizeof(buffer), temp);

    char* test = "The pointer is in an empty block.\n";
    assert(strncmp(buffer,test,25)==0);

    fclose(temp);
    remove("temp_output.txt");
   
    assert(ptr_head(--arr)->head % 2 == 1);

    arr--;
    my_free(arr);

    assert(ptr_head(arr)->head % 2 == 0);
}

void my_free_test_fusion(void)
{
    // init_memory();
    // char* arr = my_alloc(200);
    // my_free(arr);
    // assert((size_t)ptr_head(arr) != (size_t)big_free);

    // init_memory();
    // print_large_memory();
    // char* arr1 = my_alloc(200);
    // my_alloc(200);
    // char* arr2 = my_alloc(200);
    // char* arr3 = my_alloc(200);
    // my_alloc(200);
    // printf("%ld\n",sizeof(size_t));

    // my_free(arr1);
    // printf("%ld\n",sizeof(size_t));

    // my_free(arr2);
    // //size_t size = ptr_head(arr2)->size;
    // print_large_memory();
    // my_free(arr3);
    // print_large_memory();
    // assert(arr3+ptr_head(arr3)->size == arr2);

    // printf("\n\n");

    // init_memory();
    // arr = my_alloc(200);
    // my_free(arr);
    // assert((size_t)ptr_head(arr) != (size_t)big_free);

    // printf("\n\n");

    init_memory();
    print_large_memory();
    char* arr1 = my_alloc(200);
    char* arr4 =  my_alloc(200);
    my_alloc(200);
    char* arr2 = my_alloc(200);
    char* arr3 = my_alloc(200);
    my_alloc(200);
    printf("%ld -> %ld, %ld\n",(size_t)ptr_head(arr4),ptr_head(arr4)->head,ptr_head(arr4)->size);
    printf("%ld -> %ld, %ld\n",(size_t)ptr_head(arr3),ptr_head(arr3)->head,ptr_head(arr3)->size);
    printf("%ld -> %ld, %ld\n",(size_t)ptr_head(arr2),ptr_head(arr2)->head,ptr_head(arr2)->size);
    printf("%ld -> %ld, %ld\n",(size_t)ptr_head(arr1),ptr_head(arr1)->head,ptr_head(arr1)->size);
    print_large_memory();
    my_free(arr1);
    print_large_memory();

    my_free(arr3);
    print_large_memory();
    my_free(arr2);
    print_large_memory();
    //assert(arr3+ptr_head(arr3)->size == arr2 + ptr_head(arr2)->size);

    my_free(arr4);
    print_large_memory();
}

void free_test(void)
{
    init_memory();
    //print_small_memory();
    char* tab = (char*)my_alloc(57);

    ptr_head(tab)->head % 2 == 1 ? VALID : ERROR;

    my_free(tab);

    //print_small_memory();

    ptr_head(tab)->head % 2 == 0? VALID : ERROR;

    char* error_tab1 = (char*)my_alloc(200);
    (size_t)ptr_head(error_tab1)->head %2 == 1 ? VALID : ERROR;

    my_free(error_tab1);
    assert(ptr_head(error_tab1)->head != 0);
    (size_t)ptr_head(error_tab1)->head %2 == 0 ? VALID : ERROR;

    my_free(error_tab1);

    char* error_tab2 = (char*)my_alloc(16);
    error_tab2 ++;
    my_free(error_tab2);

    char* error_tab3 = (char*)my_alloc(23);
    my_free(error_tab3);
    my_free(error_tab3);
}

void my_realloc_test_small(void)
{
    init_memory();
    char* arr = my_alloc(10);
    assert(my_realloc(arr,10) == arr);
    assert(ptr_head(arr)->head%2 == 1);
    void* c = my_realloc(arr,200);
    assert(c != NULL && ptr_head(c)->size >= 200 && c <= sbrk(0));
    assert(my_realloc(arr,10) == NULL);
    assert(my_realloc(NULL,10) == NULL);
}

void my_realloc_test_large(void)
{
    init_memory();
    char* arr = my_alloc(200);
    assert(my_realloc(arr,10) == arr);
    assert(my_realloc(arr,400) != arr);
    assert(is_free(arr));

    init_memory();
    int* arr1 = my_alloc(500*sizeof(int));
    for(int i=0; i<200; i++)
    {
        arr1[i] = i;
    }
    for(int i=0; i<50; i++)
    {
        assert(arr1[i] == i);
    }
    arr1 = my_realloc(arr1,25*sizeof(int));
    for(int i=0; i<25; i++)
    {
        assert(arr1[i] == i);
    }
    arr1 = my_realloc(arr1,50*sizeof(int));
    for(int i=0; i<25; i++)
    {
        assert(arr1[i] == i);
    }
}

void realloc_test(void)
{
    init_memory();
    char* tab1 = (char*)my_alloc(57);
    char* tab2 = (char*)my_realloc(tab1, 100);

    char* tab3 = (char*)my_alloc(57);
    print_large_memory();
    char* tab4 = (char*)my_realloc(tab3, sizeof(block_t));
    print_large_memory();
    tab1 != tab2 ? VALID : ERROR;
    tab4 != NULL ? VALID : ERROR;
    tab3 != NULL ? VALID : ERROR;

}

void add_value_test(void)
{
    init_memory();
    char* tab1 = (char*)my_alloc(57);
    tab1[0] == 0 ? VALID : ERROR;
    tab1[0] = 10;
    tab1[0] == 10 ? VALID : ERROR;
    char* tab2 = (char*)my_realloc(tab1,345);
    tab2[0] == 10 ? VALID : ERROR;
    tab2[1] = 11;
    tab2[1] == 11 ? VALID : ERROR;
}

void time_test(void)
{
    init_memory();
    clock_t start = clock();
    for(int i=0; i<MAX_SMALL; i++) my_alloc(10);
    clock_t end = clock();
    printf("my_alloc time: %ld s\n",(end-start)*100000/CLOCKS_PER_SEC);
    init_memory();
    start = clock();
    for(int i=0; i<MAX_SMALL; i++) malloc(10);
    end = clock();
    printf("malloc time: %ld s\n",(end-start)*100000/CLOCKS_PER_SEC);


    init_memory();
    void** arr = malloc(sizeof(void*)*MAX_SMALL);
    start = clock();
    for(int j=0; j<10; j++)
    {
        for(int i=0; i<MAX_SMALL; i++)arr[i] = my_alloc(200);
        for(int i=0; i<MAX_SMALL/2; i++)arr[i] = my_realloc(arr[i],700);
        for(int i=0; i<MAX_SMALL; i++)my_free(arr[i]);
        for(int i=0; i<MAX_SMALL/2; i++)arr[i] = my_alloc(450);
    }
    end = clock();
    printf("my_alloc time: %ld s\n",(end-start)*100000/CLOCKS_PER_SEC);
    free(arr);
    init_memory();
    arr = malloc(sizeof(void*)*MAX_SMALL);
    start = clock();
    for(int j=0; j<10; j++)
    {
        for(int i=0; i<MAX_SMALL; i++) arr[i] = malloc(200);
        for(int i=0; i<MAX_SMALL/2; i++) arr[i] = realloc(arr[i],560);
        for(int i=0; i<MAX_SMALL/2; i++) free(arr[i]);
        for(int i=0; i<MAX_SMALL/2; i++) arr[i] = malloc(450);
    }
    end = clock();
    printf("malloc time: %ld s\n",(end-start)*100000/CLOCKS_PER_SEC);
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
    tab2 = tab1+MAX_SMALL*sizeof(block_t);
    my_realloc(tab2,10)==NULL ? VALID : ERROR;
    my_free(tab2);
}

void random_test(void)
{
    init_memory();
    srand(time(NULL));
    
    int size_index = rand() % (SIZE_BLK_SMALL+2);
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
    printf("%ld\n",(size_t)tab_realloc);
}

int main(void)
{
    // my_alloc_test_small();
    // free_test_last_block();
    // my_alloc_test_large();
    // init_memory();
    // print_large_memory();
    // print_memory();
    // my_alloc_test();
    // print_memory();
    // free_test_small();
    // free_test_large();
    // my_free_test_fusion();
    // free_test();
    // my_realloc_test_small();
    // my_realloc_test_large();
    // realloc_test();
    // add_value_test();
    time_test();
    // out_of_tab_test();
    // random_test();
    return 0;
}