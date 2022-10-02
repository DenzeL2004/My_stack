#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>

typedef int elem_t;

#define USE_LOG  //<- connect when we use logs

#ifdef USE_LOG

    //#define USE_DUMP_JR  //<- Concludes that everything is fine, the stack is not corrupted

#endif

#define DEBUG    //<- Use of protection

#ifdef DEBUG

    #define CANARY_PROTECT  //<- use of canary protection

    #ifdef CANARY_PROTECT
        const uint64_t CANARY_VAL = 0xDEADBABEDEADBABE;
    #endif

    #define HASH           //<- use of hash protection

#endif

const elem_t POISON_VAL = 228;       //<- fill stack with poisonous value

const elem_t POISON_PTR = 4242564;   //<- fill pointer with poison value

const elem_t NOT_ALLOC_PTR = 666666; //<- fill in the value of the pointer to which we are going to allocate dynamic memory

#define PRINT_TYPE "d"               //<- specifier character to print elem_t

#endif