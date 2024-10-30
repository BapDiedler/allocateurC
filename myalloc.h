#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#define VALID printf("valid\n"), fflush(stdout)
#define ERROR printf("ERROR\n"), fflush(stdout)

#define MAX_SMALL 1000                                  // max size of small_tab
#define SIZE_BLK_SMALL (128 - 2*sizeof(size_t))         // size of one block of small_tab
#define SIZE_FIRST_BLK_LARGE 1024                       // size of the first large block

typedef struct large_block_s large_block_t;
struct large_block_s // struct of block (header + size + body)
{
    size_t head;                        // pointeur vers le prochain bloc libre
    size_t size;                        // taille du bloc en nombre d'octet
    __uint8_t body[];     // corps de la mémoire du bloc
};
typedef struct block_s // structure d'un bloc (entête + corps)
{
    size_t head;                        // pointeur vers le prochain bloc libre
    size_t size;                        // taille du bloc en nombre d'octet
    __uint8_t body[SIZE_BLK_SMALL];     // corps de la mémoire du bloc
} block_t;

block_t small_tab[MAX_SMALL];   // tableau de bloc (char small_tab[MAX_SMALL * 128] est une autre otpion que avec la structure)
size_t small_free;            // pointeur vers le premier bloc libre
size_t big_free;
size_t init_mem = 1;


void* sbrk(__intptr_t increment);

/**
 * @brief allocation de mémoire de taille size
 * @param size type size_t : la taille en mémoire
 * @return pointeur vers la case mémoire
 */
void* my_alloc(size_t size);

/**
 * @brief libération de la mémoire
 * @param ptr type void* : pointeur de la zone mémoire
 */
void my_free(void* ptr);

/**
 * @brief allocation de mémoire déjà alloué de taille size
 * @param ptr type void* : pointeur donnant sa mémoire
 * @param size type size_t : la taille en mémoire
 * @return pointeur vers la case mémoire de ptr
 */
void* my_realloc(void* ptr, size_t size);

/**
 * @brief affiche les blocs libres et occupés
 */
void print_small_memory(void);

/**
 * @brief affiche les blocs libres et occupés
 */
void print_large_memory(void);
/**
 * @brief initialisation de la mémoire
 */
void init_memory(void);

/**
 * @brief premier bloc vide
 * @return bloc libre
 */
block_t* head(void);

/**
 * @brief premier bloc large vide
 * @return bloc libre
 */
large_block_t* head_big_free(void);

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