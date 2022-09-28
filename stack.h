#ifndef _STACK_H_
#define _STACK_H_

#include <math.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifndef elem_t
    #include "stack_type.h"
#endif

#include "config.h"
#include "log_def.h"

struct  Stack_info
{
    char origin_file[MAX_SIZE_BUF] = {};
    char origin_func[MAX_SIZE_BUF] = {};
    int origin_line = 0;
};

struct Stack 
{
    #ifdef CANARY_PROTECT
        uint64_t canary_vall_begin = CANARY_VALL;
    #endif

    elem_t *data = nullptr;

    long size_data = 0;
    long capacity  = 0;

    Stack_info stack_info = {};

    #ifdef HASH
        uint64_t hash_struct = 0;
        uint64_t hash_data   = 0;
    #endif

    #ifdef CANARY_PROTECT
        uint64_t canary_vall_end   = CANARY_VALL;
    #endif
};

enum Change_stack_capacity
{
    DECREASE = -1,
    INCREASE =  1
};

enum Stack_err
{
    DATA_IS_NULLPTR      = 1,
    DATA_IS_POISON       = 2,
    SIZE_LOWER_ZERO      = 4,
    CAPACITY_LOWER_ZERO  = 8,
    CAPACITY_LOWER_SIZE  = 16,
    CANARY_CURUPTED      = 32,
    HASH_DATA_CURUPTED   = 64,
    HASH_STRUCT_CURUPTED = 128
};

enum Stack_func_err{
    STACK_CTOR_ERR       = -1,
    INIT_STACK_VALLS_ERR = -2,
    
    REALLOC_STACK_ERR    = -3,
    INCREASE_STACK_ERR   = -4,
    DECREASE_STACK_ERR   = -5,
    
    STACK_PUSH_ERR       = -6,
    STACK_POP_ERR        = -7,
    
    STACK_DTOR_ERR       = -8,

    STACK_SAVE_HASH_ERR  = -9,
};

#define Stack_ctor(stack, size)             \
        Stack_ctor_ (stack, size, LOG_ARGS)

int Stack_ctor_ (Stack *stack, int size, LOG_PARAMETS);

int Stack_dtor (Stack *stack);

int Stack_push (Stack *stack, elem_t  vall);

int Stack_pop  (Stack *stack, elem_t *vall);

int Check_hash_data   (Stack *stack);

int Check_hash_struct (Stack *stack);

#endif