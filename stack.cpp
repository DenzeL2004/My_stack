#include <math.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>

#ifndef elem_t
    #include "stack_type.h"
#endif

#include "Generals_func\generals.h"
#include "log_info.h"
#include "log_def.h"
#include "config.h"
#include "stack.h"

#ifdef HASH

    #define Stack_hash_save(stack)         \
        Stack_hash_save_ (stack)
#else

    #define Stack_hash_save(stack)         \

#endif

static int Read_stack_info_  (Stack_info *stack_info, LOG_PARAMETS);

static int Init_stack_valls_ (Stack *stack);

static int Init_stack_info_  (Stack_info *stack_info);

static int Stack_hash_save_  (Stack *stack);

static int Recalloc_stack_          (Stack *stack, int mode);

static int Increase_stack_capacity_ (Stack *stack);

static int Decrease_stack_capacity_ (Stack *stack);

//=======================================================================================================

int Stack_ctor_ (Stack *stack, int size, LOG_PARAMETS)
{
    if (stack->data == (elem_t*) POISON_PTR)
    {
        printf ("Sorry, but the memory stack is already free.\n");
        return 0;
    }

    stack->stack_info = {};
    
    Init_stack_info_ (&stack->stack_info);
    Read_stack_info_ (&(stack->stack_info), LOG_VAR);

    if (size <= 0) //
    {
        printf ("YOU'RE SERIOUS!!!\nNON-POSITIVE SIZE?. "
                "Memory will be allocated, but please don't do that again. -_- \n");
    }
    
    int _size = size; // 
    if (_size < MIN_SIZE_CAPACITY) _size = MIN_SIZE_CAPACITY;

    stack->data = (elem_t*) calloc (size, sizeof(elem_t));
    //

    stack->size_data = 0;
    stack->capacity  = _size; 

    Init_stack_valls_ (stack);// stack_value_set_zero_

    #ifdef CANARY_PROTECT
        stack->canary_vall_begin = CANARY_VALL;
        stack->canary_vall_end   = CANARY_VALL;
    #endif

    #ifdef HASH
        Stack_hash_save (stack);
    #endif
    
    Stack_check (stack, STACK_CTOR_ERR);

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

    Stack_check (stack, STACK_DTOR_ERR);

    Init_stack_valls_ (stack);

    free (stack->data);
    stack->data = (elem_t*) POISON_PTR;

    stack->size_data = 0;
    stack->capacity  = 0;

    #ifdef CANARY_PROTECT
        stack->canary_vall_begin = 0;
        stack->canary_vall_end   = 0;
    #endif

    #ifdef HASH
        stack->hash_data   = 0;
        stack->hash_struct = 0;
    #endif

    Init_stack_info_ (&stack->stack_info);
    
    return 0;
}

//=======================================================================================================

#ifdef HASH

static int Stack_hash_save_ (Stack *stack)
{
    size_t size_struct = sizeof (Stack) - 2*sizeof (uint64_t);

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
            return EQUAL;
        else
            return DIFRENT;
    }

    return DIFRENT;
}

#endif

//=======================================================================================================

#ifdef HASH

int Check_hash_struct (Stack *stack)
{
    int size_struct = sizeof (*stack) - 2 * sizeof (uint64_t);

    #ifdef CANARY_PROTECT
        size_struct -= sizeof (uint64_t);
    #endif

    uint64_t hash = Get_hash ((char*) stack, (unsigned int) size_struct);

    if (hash == stack->hash_struct) 
        return EQUAL;   //
    else
        return DIFRENT;
}

#endif

//=======================================================================================================

static int Read_stack_info_ (Stack_info *stack_info, LOG_PARAMETS) //
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

static int Init_stack_valls_ (Stack *stack) //
{
    if (stack->data == nullptr || stack->data == (elem_t*) POISON_PTR)
    {
        Print_err (INIT_STACK_VALLS_ERR);
        Stack_dump (stack);
        return INIT_STACK_VALLS_ERR;
    }

    for (int id_elem = stack->size_data; id_elem < stack->capacity; id_elem++)
        stack->data[id_elem] = POISON_VALL;

    return 0;
}

//=======================================================================================================

static int Recalloc_stack_(Stack *stack, int mode) // resize(.., option)
{
    Stack_check (stack, REALLOC_STACK_ERR);

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
    Stack_check (stack, INCREASE_STACK_ERR);

    if (stack->size_data != stack->capacity) return 0;

    stack->capacity *= 2;

    stack->data = (elem_t*) realloc (stack->data, sizeof(elem_t)*stack->capacity); 

    Init_stack_valls_ (stack);

    Stack_hash_save (stack);

    return 0;    
}

//=======================================================================================================

static int Decrease_stack_capacity_ (Stack *stack)
{
    Stack_check (stack, DECREASE);

    if (stack->capacity     <= MIN_SIZE_CAPACITY) return 0;

    if (stack->capacity / 4 <= stack->size_data ) return 0;
    
    stack->capacity /= 2; //-> increase_step

    stack->data = (elem_t*) realloc (stack->data, sizeof(elem_t)*stack->capacity);
    
    Init_stack_valls_ (stack);

    Stack_hash_save (stack);

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

    Stack_check (stack, STACK_PUSH_ERR);

    if (!Recalloc_stack_(stack, INCREASE)){
        stack->data[stack->size_data++] = vall;
    }
    else
    {
        Print_err (STACK_PUSH_ERR);
        Stack_dump (stack);
        return STACK_PUSH_ERR;
    }

    Stack_hash_save (stack);

    return 0;
}

//=======================================================================================================

int Stack_pop (Stack *stack, elem_t *vall)
{
    if (stack->data == (elem_t*) POISON_PTR){
        printf ("The memory has been freed, you cannot use this stack.\n");
        return 0;
    }

    Stack_check (stack, STACK_POP_ERR);

    if (stack->size_data == 0)
    {
        printf ("Sorry baby stack is empty, next time. (o^_^o) \n");
        return 0;
    }

    if (!Recalloc_stack_(stack, DECREASE))
    {
        *vall = stack->data[stack->size_data - 1];

        stack->data[stack->size_data - 1] = POISON_VALL;

        stack->size_data--;
    }
    else
    {
        Print_err (STACK_POP_ERR);
        Stack_dump (stack);
        return STACK_POP_ERR;
    }

    Stack_hash_save (stack);

    return 0;
}

//=======================================================================================================
