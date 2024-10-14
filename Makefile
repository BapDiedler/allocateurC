CC = gcc
COPT = -Wall -Wextra -Werror -Wshadow -ansi -pedantic -std=c11

myalloc_test : test_myalloc.c myalloc.c
	$(CC) $(COPT) myalloc.c test_myalloc.c -o myalloc_test

clear :
	rm -f myalloc_test
