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
    uint64_t canary_vall_begin = CANARY_VALL;

    elem_t *data = nullptr;

    long size_data = 0;
    long capacity  = 0;

    Stack_info stack_info = {};

    uint64_t canary_vall_end   = CANARY_VALL;

};

struct Stack_err
{
    bool data_is_nullptr = 0;
    bool data_is_poison  = 0;
    
    bool size_data_lower_zero = 0;
    bool capacity_lower_zero = 0;
    bool capacity_lower_size = 0;

    bool canary_begin_curupted = 0;
    bool canary_end_curupted   = 0;

    int count_err = 0;
};

enum Change_stack_capacity
{
    DECREASE = -1,
    INCREASE =  1
};

enum Stack_func_err{
    STACK_CTOR_ERR       = -1,
    INIT_STACK_VALLS_ERR = -2,
    REALLOC_STACK_ERR    = -3,
    INCREASE_STACK_ERR   = -4,
    DECREASE_STACK_ERR   = -5,
    STACK_PUSH_ERR       = -6,
    STACK_POP_ERR        = -7,
    STACK_DTOR_ERR       = -8
    
};

#define Stack_ctor(stack, size)             \
        Stack_ctor_ (stack, size, LOG_ARGS)

int Stack_dump (FILE *fpout, Stack *stack, Stack_err *stack_errors);

int Stack_ctor_ (Stack *stack, int size, LOG_PARAMETS);

int Stack_dtor (Stack *stack);

int Stack_push (Stack *stack, elem_t  vall);

int Stack_pop  (Stack *stack, elem_t *vall);

#endif