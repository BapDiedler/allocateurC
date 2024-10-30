#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#define VALID printf("valid\n"), fflush(stdout)
#define ERROR printf("ERROR\n"), fflush(stdout)

#define MAX_SMALL 1000                                  // max size of small_tab
#define SIZE_BLK_SMALL (128 - 2*sizeof(size_t))         // size of one block of small_tab
#define SIZE_FIRST_BLK_LARGE 1024                       // size of the first large block

typedef struct block_s block_t;
typedef struct large_block_s large_block_t;
struct large_block_s // struct of block (header + size + body)
{
    size_t head;                        // pointer to next free block
    size_t size;                        // body's block size
    __uint8_t body[];                   // body's block
};

struct block_s // struct of block (header + size + body)
{
    size_t head;                        // pointer to next free block
    size_t size;                        // body's block size
    __uint8_t body[SIZE_BLK_SMALL];     // body's block
};

/**
 * @brief change the location of the program break
 */
void* sbrk(__intptr_t increment);

/**
 * @brief       allocate size bytes and return a pointer to the allocated memory
 * @param size  type size_t : nomber of bytes
 * @return      pointer to empty memory area
 */
void* my_alloc(size_t size);

/**
 * @brief       frees the memory space pointed to by ptr
 * @param ptr   type void* : pointer to memory area
 */
void my_free(void* ptr);

/**
 * @brief       changes the size of the memory block pointed to by ptr to size bytes
 * @param ptr   type void* : pointer to memory area
 * @param size  type size_t : memory size
 * @return      pointer to memory area with sames values than ptr
 */
void* my_realloc(void* ptr, size_t size);

/**
 * @brief print smalls frees and unfrees blocks
 */
void print_small_memory(void);

/**
 * @brief print larges frees blocks
 */
void print_large_memory(void);

/**
 * @brief initialise memory
 */
void init_memory(void);

/**
 * @brief passe au prochain bloc libre
 */
void next(void);

/**
 * @brief test pour savoir si un bloc est libre
 * @param ptr : void* pointeur à tester
 * @return 0 si libre
 */
int is_free(void* ptr);

/**
 * @brief accès au pointeur vers la tête d'un bloc
 * @param ptr : void* poiteur du tableau d'un bloc
 * @return block_t* pointeur de la tête du bloc
 */
block_t* ptr_head(void* ptr);

size_t size_big_free(void);