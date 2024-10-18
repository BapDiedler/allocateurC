#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#define VALID printf("valid\n"), fflush(stdout)
#define ERROR printf("ERROR\n"), fflush(stdout)

#define MAX_SMALL 5                                 // taille maximale du petit tableau
#define SIZE_BLK_SMALL (128 - sizeof(size_t))       // taille d'un bloc sans l'entête
#define SIZE_FIRST_BLK_LARGE 1024

typedef struct block_s block_t;
struct block_s // structure d'un bloc (entête + corps)
{
    size_t head;                        // pointeur vers le prochain bloc libre
    size_t size;                        // taille du bloc en nombre d'octet
    __uint8_t* body;     // corps de la mémoire du bloc
};

block_t small_tab[MAX_SMALL];   // tableau de bloc (char small_tab[MAX_SMALL * 128] est une autre otpion que avec la structure)
size_t small_free;            // pointeur vers le premier bloc libre
size_t big_free;


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
void print_memory(void);

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