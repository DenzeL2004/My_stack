#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdlib.h>

typedef int elem_t;

#define USE_LOG //<- logging errors

#define DEBUG   //<- use of protection

#ifdef DEBUG

    #define CANARY_PROTECT //<- use of canary protection

    #ifdef CANARY_PROTECT
        const unsigned long long CANARY_VAL = 0xDEADBABEDEADBABE;
    #endif

    #define HASH           //<- use of hash protection

#endif

const elem_t POISON_VAL = 228;      //<- poison value

const elem_t POISON_PTR = 4242564;  //<- poison ptr

const elem_t NOT_ALLOC_PTR = 666666;

#define PRINT_TYPE "d"

#define MAX_SIZE_BUF 50

#define MIN_SIZE_CAPACITY 10 

#endif