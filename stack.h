#ifndef _STACK_H_
#define _STACK_H_

#include <math.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "Generals_func\generals.h"
#include "log_info.h"
#include "config.h"

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

static int Stack_check_ (Stack *stack, Stack_err *stack_errors);

int Stack_dump (FILE *fpout, Stack *stack, Stack_err *stack_errors);

int Stack_ctor (Stack *stack, int size, LOG_PARAMETS);

int Stack_dtor (Stack *stack);

static int Read_stack_info_ (Stack_info *stack_info, LOG_PARAMETS);

static int Init_stack_valls_ (Stack *stack);

static int Init_stack_info_  (Stack_info *stack_info);

static int Realloc_stack_           (Stack *stack, int mode);

static int Increase_stack_capacity_ (Stack *stack);

static int Decrease_stack_capacity_ (Stack *stack);

int Stack_push (Stack *stack, elem_t  vall);

int Stack_pop  (Stack *stack, elem_t *vall);

//=======================================================================================================

static int Stack_check_ (Stack *stack, Stack_err *stack_errors)
{
    if (stack->data == nullptr)
    {
        stack_errors->data_is_nullptr = true;
        stack_errors->count_err++;
    }

    if (stack->data == (elem_t*) POISON_PTR)
    {
        stack_errors->data_is_poison = true;
        stack_errors->count_err++;
    }

    if (stack->size_data < 0)
    {
        stack_errors->size_data_lower_zero = true;
        stack_errors->count_err++;
    }

    if (stack->capacity < 0)
    {
        stack_errors->capacity_lower_zero = true;
        stack_errors->count_err++;
    }

    if (stack->capacity < stack->size_data)
    {
        stack_errors->capacity_lower_size = true;
        stack_errors->count_err++;
    }

    if (stack->canary_vall_begin != CANARY_VALL)
    {
        stack_errors->canary_begin_curupted = true;
        stack_errors->count_err++;
    }

    if (stack->canary_vall_end != CANARY_VALL)
    {
        stack_errors->canary_end_curupted = true;
        stack_errors->count_err++;
    }

    return stack_errors->count_err;    
}

//=======================================================================================================

int Stack_dump (FILE *fpout, Stack *stack, Stack_err *stack_errors)
{
    fprintf (fpout, "COUNT ERR: %ld\n\n", stack_errors->count_err);

    fprintf (fpout, "Stack pointer to data is |%p|\n\n", (char*)stack->data);

    fprintf (fpout, "Stack size_data = %ld\n", stack->size_data);
    fprintf (fpout, "Stack capacity = %ld\n",  stack->capacity);

    fprintf (fpout, "Stack canary_vall_begin = %ld\n", stack->canary_vall_begin);
    fprintf (fpout, "Stack canary_vall_end   = %ld\n", stack->canary_vall_end);

    fprintf (fpout, "\n");

    if (stack_errors->data_is_nullptr) fprintf (fpout, "stack pointer data is nullptr.\n");
    if (stack_errors->data_is_poison ) fprintf (fpout, "stack pointer data is poison vallue.\n");

    fprintf (fpout, "\n");

    if (stack_errors->size_data_lower_zero) fprintf (fpout, "stack size_data is a negative number.\n");
    if (stack_errors->capacity_lower_zero)  fprintf (fpout, "stack capacity is a negative number.\n");
    if (stack_errors->capacity_lower_size)  fprintf (fpout, "stack capacity is lower size_data:\n");

    fprintf (fpout, "\n");
    
    if (stack_errors->canary_begin_curupted) fprintf (fpout, "stack canary_begin is currupted.\n");
    if (stack_errors->canary_end_curupted  ) fprintf (fpout, "stack canary_end is currupted.\n");

    if (!stack_errors->data_is_nullptr && !stack_errors->capacity_lower_zero && stack_errors->size_data_lower_zero)
    {
        for (int id_elem = 0; id_elem < stack->capacity; id_elem)
        {
            if (id_elem < stack->size_data)
                fprintf (fpout, "%5ld. *[%5lg]", id_elem, stack->data[id_elem]);
            else
                fprintf (fpout, "%5ld.  [%5lg]", id_elem, stack->data[id_elem]);
        }
    }

    return 0;
}

//=======================================================================================================

int Stack_ctor (Stack *stack, int size, LOG_PARAMETS)
{
    if (stack->data == (elem_t*) POISON_PTR)
    {
        printf ("Sorry, but the memory stack is already free.\n");
        return 0;
    }

    Stack_err stack_errors = {0};

    if (Stack_check_ (stack, &stack_errors))
    { 
        Print_err (stack, stack_errors, STACK_CTOR_ERR);
        return STACK_CTOR_ERR;
    }

    stack->stack_info = {};
    
    Init_stack_info_ (&stack->stack_info);
    Read_stack_info_ (&(stack->stack_info), file_name, func_name, line);

    if (size <= 0)
    {
        printf ("YOU'RE SERIOUS!!!\nNON-POSITIVE SIZE?. "
                "Memory will be allocated, but please don't do that again. -_- \n");
    }

    int _size = size;
    if (_size < MIN_SIZE_CAPACITY) _size = MIN_SIZE_CAPACITY;

    stack->data = (elem_t*) calloc (size, sizeof(elem_t));

    stack->size_data = 0;
    stack->capacity  = _size; 

    Init_stack_valls_ (stack);
    
    if (Stack_check_ (stack, &stack_errors))
    { 
        Print_err (stack, stack_errors, STACK_CTOR_ERR);
        return STACK_CTOR_ERR;
    }
    
    return 0;
}

//=======================================================================================================

int Stack_dtor (Stack *stack)
{   
    if (stack->data == (elem_t*) POISON_PTR)
    {
        printf ("Sorry, but the memory stack is already free.\n");
        return 0;
    }

    Stack_err stack_errors = {0};

    if (Stack_check_ (stack, &stack_errors))
    { 
        Print_err (stack, stack_errors, STACK_DTOR_ERR);
        return STACK_DTOR_ERR;
    }

    Init_stack_valls_ (stack);

    free (stack->data);
    stack->data = (elem_t*) POISON_PTR;

    stack->size_data = 0;
    stack->capacity  = 0;

    stack->canary_vall_begin = 0;
    stack->canary_vall_end   = 0;

    Init_stack_info_ (&stack->stack_info);
    
    return 0;
}

//=======================================================================================================

static int Read_stack_info_ (Stack_info *stack_info, LOG_PARAMETS)
{
    assert (stack_info != nullptr && "stack_info in function Get_stack_info is nullptr");

    stack_info->origin_line = line;

    strncpy (stack_info->origin_file, file_name, MAX_SIZE_BUF);
    strncpy (stack_info->origin_func, func_name, MAX_SIZE_BUF);

    return 0;
}

//=======================================================================================================

static int Init_stack_info_ (Stack_info *stack_info)
{
    assert (stack_info != nullptr && "stack_info in function Get_stack_info is nullptr");

    stack_info->origin_line = 0;
    for (int id_char = 0; id_char < MAX_SIZE_BUF; id_char++){
        stack_info->origin_file[id_char] = '\0';
        stack_info->origin_func[id_char] = '\0';
    }

    return 0;
}

//=======================================================================================================

static int Init_stack_valls_ (Stack *stack)
{
    Stack_err stack_errors = {0};

    if (Stack_check_ (stack, &stack_errors))
    { 
        Print_err (stack, stack_errors, INIT_STACK_VALLS_ERR);
        return INIT_STACK_VALLS_ERR;
    }

    for (int id_elem = stack->size_data; id_elem < stack->capacity; id_elem++)
        stack->data[id_elem] = POISON_VALL;

    return 0;
}

//=======================================================================================================

static int Realloc_stack_ (Stack *stack, int mode)
{
    Stack_err stack_errors = {0};

    if (Stack_check_ (stack, &stack_errors))
    { 
        Print_err (stack, stack_errors, REALLOC_STACK_ERR);
        return REALLOC_STACK_ERR;
    } 

    if (mode == INCREASE)
        return Increase_stack_capacity_ (stack);

    if (mode == DECREASE)
        return Decrease_stack_capacity_ (stack);

    return 0;
}

//=======================================================================================================

static int Increase_stack_capacity_ (Stack *stack)
{
    Stack_err stack_errors = {0};

    if (Stack_check_ (stack, &stack_errors))
    { 
        Print_err (stack, stack_errors, INCREASE_STACK_ERR);
        return INCREASE_STACK_ERR;
    } 

    if (stack->size_data != stack->capacity) return 0;

    stack->capacity *= 2;

    stack->data = (elem_t*) realloc (stack->data, stack->capacity);

    if (Stack_check_ (stack, &stack_errors))
    { 
        Print_err (stack, stack_errors, INCREASE_STACK_ERR);
        return INCREASE_STACK_ERR;
    } 

    Init_stack_valls_ (stack);

    return 0;    
}

//=======================================================================================================

static int Decrease_stack_capacity_ (Stack *stack)
{
    Stack_err stack_errors = {0};

    if (Stack_check_ (stack, &stack_errors))
    { 
        Print_err (stack, stack_errors, DECREASE_STACK_ERR);
        return DECREASE_STACK_ERR;
    }

    if (stack->capacity     <= MIN_SIZE_CAPACITY) return 0;

    if (stack->capacity / 4 <= stack->size_data ) return 0;
    
    Init_stack_valls_ (stack);
    
    stack->capacity /= 2;

    stack->data = (elem_t*) realloc (stack->data, stack->capacity);

    if (Stack_check_ (stack, &stack_errors))
    { 
        Print_err (stack, stack_errors, DECREASE_STACK_ERR);
        return DECREASE_STACK_ERR;
    }

    return 0;
}

//=======================================================================================================

int Stack_push (Stack *stack, elem_t vall)
{
    if (stack->data == (elem_t*) POISON_PTR)
    {
        printf ("The memory has been freed, you cannot use this stack.\n");
        return 0;
    }

    Stack_err stack_errors = {0};

    if (Stack_check_ (stack, &stack_errors))
    { 
        Print_err (stack, stack_errors, STACK_PUSH_ERR);
        return STACK_PUSH_ERR;
    }

    if (Realloc_stack_ (stack, INCREASE)) 
        stack->data[stack->size_data++] = vall;
    else
    {
        Stack_check_ (stack, &stack_errors)
        Print_err (stack, stack_errors, STACK_PUSH_ERR);
        return STACK_PUSH_ERR;
    }

    return 0;
}

//=======================================================================================================

int Stack_pop (Stack *stack, elem_t *vall)
{
    if (stack->data == (elem_t*) POISON_PTR){
        printf ("The memory has been freed, you cannot use this stack.\n");
        return 0;
    }

    Stack_err stack_errors = {0};

    if (Stack_check_ (stack, &stack_errors))
    { 
        Print_err (stack, stack_errors, STACK_POP_ERR);
        return STACK_POP_ERR;
    }

    if (stack->size_data == 0)
    {
        printf ("Sorry baby stack is empty, next time. (o^▽^o) \n");
        return 0;
    }

    if (Realloc_stack_ (stack, DECREASE))
    {
        *vall = stack->data[stack->size_data - 1];

        stack->data[stack->size_data - 1] = POISON_VALL;

        stack->size_data -= 1;
    }
    else
    {
        Stack_check_ (stack, &stack_errors);
        Print_err (stack, stack_errors, STACK_POP_ERR);
        return STACK_POP_ERR;
    }

    return 0;
}

//=======================================================================================================

#endif