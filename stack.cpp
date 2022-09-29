#include <math.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>

#include "Generals_func\generals.h"
#include "stack_log_errors.h"
#include "log_errors.h"
#include "log_def.h"
#include "config.h"
#include "stack.h"

#ifdef HASH

    #define Stack_hash_save(stack)         \
        Stack_hash_save_ (stack)
#else

    #define Stack_hash_save(stack)         \

#endif

static int Stack_info_ctor_  (Stack_info *stack_info, LOG_PARAMETS);

static int Stack_info_dtor_  (Stack_info *stack_info);

static int Stack_hash_save_  (Stack *stack);

static int Stack_vals_poison_set_ (Stack *stack);

static int Recalloc_stack_          (Stack *stack, int mode);

static int Increase_stack_capacity_ (Stack *stack);

static int Decrease_stack_capacity_ (Stack *stack);

//=======================================================================================================

int Stack_ctor_ (Stack *stack, unsigned long capacity, 
                 const char* file_name, const char* func_name, int line)
{ 
    stack->stack_info = {};
    Stack_info_ctor_ (&stack->stack_info, LOG_VAR);

    stack->data = (elem_t*) NOT_ALLOC_PTR;

    stack->size_data = 0;
    stack->capacity  = capacity; 

    #ifdef CANARY_PROTECT
        stack->canary_val_begin = CANARY_VAL;
        stack->canary_val_end   = CANARY_VAL;
    #endif

    #ifdef HASH
        Stack_hash_save (stack);
    #endif
    
    if (Stack_check (stack))
    {
        Stack_dump (stack);
        return STACK_CTOR_ERR;
    }

    return 0;
}

//=======================================================================================================

int Stack_dtor (Stack *stack)
{   
    if (Stack_data_ptr_check (stack, LOG_ARGS)) return STACK_CTOR_ERR; 

    if (Stack_check (stack))
    {
        Stack_dump (stack);
        return STACK_DTOR_ERR;
    }

    Stack_vals_poison_set_ (stack);

    free (stack->data);
    stack->data = (elem_t*) POISON_PTR;

    stack->size_data = POISON_VAL;
    stack->capacity  = POISON_VAL;

    #ifdef CANARY_PROTECT
        stack->canary_val_begin = POISON_VAL;
        stack->canary_val_end   = POISON_VAL;
    #endif

    #ifdef HASH
        stack->hash_data   = POISON_VAL;
        stack->hash_struct = POISON_VAL;
    #endif

    Stack_info_dtor_ (&stack->stack_info);
    
    return 0;
}

//=======================================================================================================

static int Stack_info_ctor_ (Stack_info *stack_info, 
                             const char* file_name, const char* func_name, int line) 
{
    if (stack_info == nullptr)
    {
        Log_report (LOG_ARGS, "Ptr on struct stack_info is nullptr\n");
        return STACK_INFO_CTOR_ERR;
    }

    stack_info->origin_line = line;
    stack_info->origin_file = strdup (file_name);
    stack_info->origin_func = strdup (func_name);
    
    return 0;
}

//=======================================================================================================

static int Stack_info_dtor_ (Stack_info *stack_info) 
{
    if (stack_info == nullptr)
    {
        Log_report (LOG_ARGS, "Ptr on struct stack_info is nullptr\n");
        return STACK_INFO_CTOR_ERR;
    }

    stack_info->origin_line = -1;
    
    free(stack_info->origin_file);
    stack_info->origin_file = (char*) POISON_PTR;

    free(stack_info->origin_func);
    stack_info->origin_func = (char*) POISON_PTR;
    
    return 0;
}

//=======================================================================================================

#ifdef HASH

static int Stack_hash_save_ (Stack *stack)
{
    size_t size_struct = sizeof (*stack) - 2*sizeof (uint64_t);

    #ifdef CANARY_PROTECT
        size_struct -= sizeof (uint64_t);
    #endif

    stack->hash_struct = Get_hash ((char*) stack, size_struct);

    size_t size_data = stack->capacity * sizeof (elem_t);

    stack->hash_data = Get_hash ((char*) stack->data, size_data);

    return 0;
}

#endif

//=======================================================================================================

#ifdef HASH

int Check_hash_data (const Stack *stack)
{
    if (stack->data != nullptr && stack->data != (elem_t*) POISON_PTR) 
    {
        int size_data = stack->capacity * sizeof (elem_t);

        uint64_t hash = Get_hash ((char*) stack->data, (unsigned int) size_data);

        if (hash == stack->hash_data) //
            return 0;
        else
            return 1;
    }

    return 1;
}

#endif

//=======================================================================================================

#ifdef HASH

int Check_hash_struct (const Stack *stack)
{
    int size_struct = sizeof (*stack) - 2 * sizeof (uint64_t);

    #ifdef CANARY_PROTECT
        size_struct -= sizeof (uint64_t);
    #endif

    uint64_t hash = Get_hash ((char*) stack, (unsigned int) size_struct);

    if (hash == stack->hash_struct) 
        return 0;   
    else
        return 1;
}

#endif

//=======================================================================================================

static int Stack_vals_poison_set_ (Stack *stack)
{
    if (Stack_data_ptr_check (stack, LOG_ARGS))
    {
        Log_report (LOG_ARGS, "Stack vals don't assigned\n");
        Stack_dump (stack);
        return INIT_STACK_VALLS_ERR;
    }

    for (int id_elem = stack->size_data; id_elem < stack->capacity; id_elem++)
        stack->data[id_elem] = POISON_VAL;

    return 0;
}

//=======================================================================================================

static int Recalloc_stack_(Stack *stack, int mode) // resize(.., option)
{
    if (Stack_check (stack))
    {
        Stack_dump (stack);
        return REALLOC_STACK_ERR;
    }

    if (mode == INCREASE)
        return Increase_stack_capacity_ (stack);      //recalloc

    if (mode == DECREASE)
        return Decrease_stack_capacity_ (stack);     // recalloc

    //

    return 0;
}

//=======================================================================================================

static int Increase_stack_capacity_ (Stack *stack)
{
    if (Stack_check (stack))
    {
        Stack_dump (stack);
        return INCREASE_STACK_ERR;
    }

    if (stack->size_data != stack->capacity) return 0;

    stack->capacity *= 2;

    stack->data = (elem_t*) realloc (stack->data, sizeof(elem_t)*stack->capacity); 

    Stack_vals_poison_set_ (stack);

    Stack_hash_save (stack);

    if (Stack_check (stack))
    {
        Stack_dump (stack);
        return INCREASE;
    }

    return 0;    
}

//=======================================================================================================

static int Decrease_stack_capacity_ (Stack *stack)
{
    if (Stack_check (stack))
    {
        Stack_dump (stack);
        return DECREASE_STACK_ERR;
    }

    if (stack->capacity     <= MIN_SIZE_CAPACITY) return 0;

    if (stack->capacity / 4 <= stack->size_data ) return 0;
    
    stack->capacity /= 2; //-> increase_step

    stack->data = (elem_t*) realloc (stack->data, sizeof(elem_t)*stack->capacity);
    
    Stack_vals_poison_set_ (stack);

    Stack_hash_save (stack);

    if (Stack_check (stack))
    {
        Stack_dump (stack);
        return DECREASE_STACK_ERR;
    }

    return 0;
}

//=======================================================================================================

int Stack_push (Stack *stack, elem_t vall)
{
    if (stack->data == (elem_t*) NOT_ALLOC_PTR)
    {
        stack->data = (elem_t*) calloc (stack->capacity, sizeof (elem_t));
        
        if (Stack_data_ptr_check (stack, LOG_ARGS)) return STACK_PUSH_ERR;

        Stack_vals_poison_set_ (stack);

        Stack_hash_save (stack);
    }

    if (Stack_data_ptr_check (stack, LOG_ARGS)) return STACK_PUSH_ERR;

    if (Stack_check (stack))
    {
        Stack_dump (stack);
        return STACK_PUSH_ERR;
    }

    if (!Recalloc_stack_(stack, INCREASE)){
        stack->data[stack->size_data++] = vall;
    }
    else
    {
        Log_report (LOG_ARGS, "In push recalloc didn't work correctly\n");
        Stack_dump (stack);
        return STACK_PUSH_ERR;
    }

    Stack_hash_save (stack);

    if (Stack_check (stack))
    {
        Stack_dump (stack);
        return STACK_PUSH_ERR;
    }

    return 0;
}

//=======================================================================================================

int Stack_pop (Stack *stack, elem_t *vall)
{
    if (Stack_data_ptr_check (stack, LOG_ARGS)) return STACK_PUSH_ERR;

    if (Stack_check (stack))
    {
        Stack_dump (stack);
        return STACK_POP_ERR;
    }

    if (stack->size_data == 0)
    {
        printf ("Sorry baby stack is empty, next time. (o^_^o) \n");
        return 0;
    }

    if (!Recalloc_stack_(stack, DECREASE))
    {
        *vall = stack->data[stack->size_data - 1];

        stack->data[stack->size_data - 1] = POISON_VAL;

        stack->size_data--;
    }
    else
    {
        Log_report (LOG_ARGS, "In push recalloc didn't work correctly\n");
        Stack_dump (stack);
        return STACK_POP_ERR;
    }

    Stack_hash_save (stack);

    if (Stack_check (stack))
    {
        Stack_dump (stack);
        return STACK_POP_ERR;
    }

    return 0;
}

//=======================================================================================================


