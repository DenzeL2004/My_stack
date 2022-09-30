#include <math.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>

#include "Generals_func\generals.h"
#include "log_errors.h"
#include "log_def.h"
#include "config.h"
#include "stack.h"

#define Stack_info_ctor(stack_info)                       \
        Stack_info_ctor_ (stack_info, LOG_VAR, fp_logs)

static int Stack_info_ctor_  (Stack_info *stack_info, LOG_PARAMETS, FILE *fp_logs);

#define Stack_info_dtor(stack_info)                       \
        Stack_info_dtor_ (stack_info, fp_logs)

static int Stack_info_dtor_  (Stack_info *stack_info, FILE *fp_logs);

#ifdef HASH

    #define Stack_hash_save(stack)                        \
        Stack_hash_save_ (stack, fp_logs)
#else

    #define Stack_hash_save(stack)               \

#endif

static int Stack_hash_save_         (Stack *stack, FILE *fp_logs);

#define Stack_vals_poison_set(stack)                      \
        Stack_vals_poison_set_ (stack, fp_logs)

static int Stack_vals_poison_set_   (Stack *stack, FILE *fp_logs);

#define Recalloc_stack(stack, option)                       \
        Recalloc_stack_ (stack, option, fp_logs)

static int Recalloc_stack_          (Stack *stack, const int option, FILE *fp_logs);

#define Increase_stack_capacity(stack)                    \
        Increase_stack_capacity_ (stack, fp_logs)

static int Increase_stack_capacity_ (Stack *stack, FILE *fp_logs);

#define Decrease_stack_capacity(stack)                    \
        Decrease_stack_capacity_ (stack, fp_logs)

static int Decrease_stack_capacity_ (Stack *stack, FILE *fp_logs);

#define Check_hash_data(stack)                            \
        Check_hash_data_ (stack, fp_logs)

static int Check_hash_data_   (const Stack *stack, FILE *fp_logs);

#define Check_hash_struct(stack)                          \
        Check_hash_struct_ (stack, fp_logs)

static int Check_hash_struct_ (const Stack *stack, FILE *fp_logs);

#define Check_stack_data_ptr(stack)                       \
        Check_stack_data_ptr_ (stack, fp_logs)

static int Check_stack_data_ptr_ (Stack *stack, FILE *fp_logs);

#ifdef USE_LOG

    #define Stack_dump_jr(stack)                       \
            Stack_dump_jr_ (stack, fp_logs)

#else  
    
    #define Stack_dump_jr(stack)                   

#endif

int Stack_dump_jr_ (Stack *stack, FILE *fp_logs);

#define Stack_check(stack)                                \
        Stack_check_ (stack, fp_logs)

static uint64_t Stack_check_     (Stack *stack, FILE *fp_logs);

//=======================================================================================================

int Stack_ctor_ (Stack *stack, long capacity, 
                 const char* file_name, const char* func_name, const int line, FILE *fp_logs)
{
    assert (stack != nullptr && "stack is nullptr");

    stack->stack_info = {};
    Stack_info_ctor (&stack->stack_info);

    stack->data = (elem_t*) NOT_ALLOC_PTR;

    stack->size_data = 0;

    if (capacity < 0)
    {
        printf ("You can't use a negative number for capacity\n");

        Log_report ("The user tried to assign a negative number to the size\n");
        return STACK_CTOR_ERR;
    }

    stack->capacity  = capacity; 

    #ifdef CANARY_PROTECT
        stack->canary_val_begin = CANARY_VAL;
        stack->canary_val_end   = CANARY_VAL;
    #endif

    #ifdef HASH
        Stack_hash_save (stack);
    #endif

    
    uint64_t err_code = Stack_check (stack);
    if (err_code)
    {
        Stack_dump (stack);
        return STACK_CTOR_ERR;
    }

    return 0;
}

//=======================================================================================================

int Stack_dtor_ (Stack *stack, FILE* fp_logs)
{   
    assert (stack != nullptr && "stack is nullptr");

    uint64_t err_code = Stack_check (stack);
    if (Stack_check (stack))
    {
        Stack_dump (stack);
        return STACK_DTOR_ERR;
    }

    Stack_vals_poison_set (stack);

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

    Stack_info_dtor (&stack->stack_info);
    
    return 0;
}

//=======================================================================================================

static int Stack_info_ctor_ (Stack_info *stack_info, 
                             const char* file_name, const char* func_name, int line, FILE *fp_logs) 
{
    assert (stack_info != nullptr && "stack_info is nullptr");

    stack_info->origin_line = line;
    stack_info->origin_file = strdup (file_name);
    stack_info->origin_func = strdup (func_name);
    
    return 0;
}

//=======================================================================================================

static int Stack_info_dtor_ (Stack_info *stack_info, FILE *fp_logs) 
{
    assert (stack_info != nullptr && "stack_info is nullptr");

    stack_info->origin_line = -1;
    
    free(stack_info->origin_file);
    stack_info->origin_file = (char*) POISON_PTR;

    free(stack_info->origin_func);
    stack_info->origin_func = (char*) POISON_PTR;
    
    return 0;
}

//=======================================================================================================

#ifdef HASH

static int Stack_hash_save_ (Stack *stack, FILE *fp_logs)
{
    assert (stack != nullptr && "stack is nullptr");

    size_t size_struct = sizeof (*stack) - 2 * sizeof (uint64_t);

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

static int Check_hash_data_ (const Stack *stack, FILE *fp_logs)
{
    assert (stack != nullptr && "stack is nullptr");

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

static int Check_hash_struct_ (const Stack *stack, FILE *fp_logs)
{
    assert (stack != nullptr && "stack is nullptr");

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

static int Stack_vals_poison_set_ (Stack *stack, FILE *fp_logs)
{
    assert (stack != nullptr && "stack is nullptr");

    if (Check_stack_data_ptr (stack))
    {
        Log_report ("Stack vals don't assigned\n");
        
        uint64_t err_code = Stack_check (stack);
        Stack_dump (stack);
        
        return INIT_STACK_VALLS_ERR;
    }

    for (int id_elem = stack->size_data; id_elem < stack->capacity; id_elem++)
        stack->data[id_elem] = POISON_VAL;

    return 0;
}

//=======================================================================================================

static int Recalloc_stack_(Stack *stack, const int option, FILE *fp_logs) 
{
    assert (stack != nullptr && "stack is nullptr");

    uint64_t err_code = Stack_check (stack);
    if (err_code)
    {
        Stack_dump (stack);
        return RECALLOC_STACK_ERR;
    }

    if (option == INCREASE)
        return Increase_stack_capacity (stack);      //recalloc

    if (option == DECREASE)
        return Decrease_stack_capacity (stack);     // recalloc

    err_code = Stack_check (stack);
    if (err_code)
    {
        Stack_dump (stack);
        return RECALLOC_STACK_ERR;
    }

    return 0;
}

//=======================================================================================================

static int Increase_stack_capacity_ (Stack *stack, FILE *fp_logs)
{
    assert (stack != nullptr && "stack is nullptr");

    uint64_t err_code = Stack_check (stack);
    if (err_code)
    {
        Stack_dump (stack);
        return INCREASE_STACK_ERR;
    }

    if (stack->size_data != stack->capacity) return 0;

    stack->capacity *= 2;

    stack->data = (elem_t*) realloc (stack->data, sizeof(elem_t)*stack->capacity); 

    Stack_vals_poison_set (stack);

    Stack_hash_save (stack);

    err_code = Stack_check (stack);
    if (err_code)
    {
        Stack_dump (stack);
        return INCREASE;
    }

    return 0;    
}

//=======================================================================================================

static int Decrease_stack_capacity_ (Stack *stack, FILE *fp_logs)
{
    assert (stack != nullptr && "stack is nullptr");

    uint64_t err_code = Stack_check (stack);
    if (err_code)
    {
        Stack_dump (stack);
        return DECREASE_STACK_ERR;
    }

    if (stack->capacity / 4 <= stack->size_data ) return 0;
    
    stack->capacity /= 2; 

    stack->data = (elem_t*) realloc (stack->data, sizeof(elem_t)*stack->capacity);
    
    Stack_vals_poison_set (stack);

    Stack_hash_save (stack);

    err_code = Stack_check (stack);
    if (Stack_check (stack))
    {
        Stack_dump (stack);
        return DECREASE_STACK_ERR;
    }

    return 0;
}

//=======================================================================================================

int Stack_push_ (Stack *stack, elem_t vall, FILE *fp_logs)
{
    assert (stack != nullptr && "stack is nullptr");
    
    if (stack->data == (elem_t*) NOT_ALLOC_PTR)
    {
        stack->data = (elem_t*) calloc (stack->capacity, sizeof (elem_t));
        
        if (Check_stack_data_ptr (stack)) return STACK_PUSH_ERR;

        Stack_vals_poison_set (stack);

        Stack_hash_save (stack);
    }

    if (Check_stack_data_ptr (stack)) return STACK_PUSH_ERR;

    uint64_t err_code = Stack_check (stack);
    if (err_code)
    {
        Stack_dump (stack);
        return STACK_PUSH_ERR;
    }

    if (!Recalloc_stack (stack, INCREASE)){
        stack->data[stack->size_data++] = vall;
    }
    else
    {
        Log_report ("In push recalloc didn't work correctly\n");
        
        err_code = Stack_check (stack);
        Stack_dump (stack);
        
        return STACK_PUSH_ERR;
    }

    Stack_hash_save (stack);

    err_code = Stack_check (stack);
    if (err_code)
    {
        Stack_dump (stack);
        return STACK_PUSH_ERR;
    }

    return 0;
}

//=======================================================================================================

int Stack_pop_ (Stack *stack, elem_t *vall, FILE *fp_logs)
{
    assert (stack != nullptr && "stack is nullptr");

    if (Check_stack_data_ptr (stack)) return STACK_PUSH_ERR;

    uint64_t err_code = Stack_check (stack);
    if (err_code)
    {
        Stack_dump (stack);
        return STACK_POP_ERR;
    }

    if (!Recalloc_stack (stack, DECREASE))
    {
        *vall = stack->data[stack->size_data - 1];

        stack->data[stack->size_data - 1] = POISON_VAL;

        stack->size_data--;
    }
    else
    {
        Log_report ("In pop recalloc didn't work correctly\n");
        
        err_code = Stack_check (stack);
        Stack_dump (stack);
        
        return STACK_POP_ERR;
    }

    Stack_hash_save (stack);

    err_code = Stack_check (stack);
    if (err_code)
    {
        Stack_dump (stack);
        return STACK_POP_ERR;
    }

    return 0;
}

//=======================================================================================================

int Stack_dump_ (Stack *stack, uint64_t err_code,
                 const char* file_name, const char* func_name, int line, FILE *fp_logs)
{
    assert (stack != nullptr && "stack is nullptr");
    
    fprintf (fp_logs, "=================================================\n\n");

    fprintf (fp_logs, "REFERENCE:\n");

    fprintf (fp_logs, "Stack variable created in file %s in function %s line %d.\n", 
            stack->stack_info.origin_file,  stack->stack_info.origin_func, stack->stack_info.origin_line);
                       
    fprintf (fp_logs, "Caused an error in file %s, function %s, line %d\n\n", LOG_VAR);
    
    fprintf (fp_logs, "ERR CODE: ");
    Bin_represent (fp_logs, err_code, sizeof (err_code));
    fprintf (fp_logs, "\n");

    fprintf (fp_logs, "Stack pointer to data is |%p|\n\n", (char*) stack->data);

    fprintf (fp_logs, "Stack size_data = %ld\n", stack->size_data);
    fprintf (fp_logs, "Stack capacity  = %ld\n",  stack->capacity);

    #ifdef CANARY_PROTECT
        fprintf (fp_logs, "Stack canary vall begin = 0x%lluX\n", stack->canary_val_begin);
        fprintf (fp_logs, "Stack canary vall end   = 0x%lluX\n", stack->canary_val_end);
    #endif

    #ifdef HASH 
        fprintf (fp_logs, "Stack hash data     = %llu\n", stack->hash_data);
        fprintf (fp_logs, "Stack hash struct   = %llu\n", stack->canary_val_end);
    #endif

    fprintf (fp_logs, "\n");

    if (err_code & BAD_DATA_PTR) fprintf (fp_logs, "stack pointer data is BAD.\n");

    fprintf (fp_logs, "\n");

    if (err_code & SIZE_LOWER_ZERO)      fprintf (fp_logs, "stack size_data is a negative number.\n");
    if (err_code & CAPACITY_LOWER_ZERO)  fprintf (fp_logs, "stack capacity is a negative number.\n");
    if (err_code & CAPACITY_LOWER_SIZE)  fprintf (fp_logs, "stack capacity is lower size_data:\n");

    fprintf (fp_logs, "\n");
    
    #ifdef CANARY_PROTECT
        if (err_code & CANARY_CURUPTED) fprintf (fp_logs, "stack canary_begin is currupted.\n");
    #endif

    if (!((err_code & BAD_DATA_PTR) || 
          (err_code & SIZE_LOWER_ZERO) || (err_code & CAPACITY_LOWER_ZERO)))
    {
        for (int id_elem = 0; id_elem < stack->capacity; id_elem++)
        {
            if (id_elem < stack->size_data)
                fprintf (fp_logs, "%5d. *[%" PRINT_TYPE "]\n", id_elem, stack->data[id_elem]);
            else
                fprintf (fp_logs, "%5d.  [%" PRINT_TYPE "] is POISON\n", id_elem, stack->data[id_elem]);
        }
    }

    fprintf (fp_logs, "=================================================\n\n");

    return 0;
}

//=======================================================================================================


static int Check_stack_data_ptr_ (Stack *stack, FILE *fp_logs)
{
    assert (stack != nullptr && "stack is nullptr");
    
    if (stack->data == nullptr)
    {
        Log_report ("The user tried to use nullptr. Data is nullptr\n");
        return BAD_DATA_PTR;
    }

    if (stack->data == (elem_t*) POISON_PTR)
    {
        Log_report ("The user tried to use a pointer with a pison value\n");
        return BAD_DATA_PTR;
    }

    if (stack->data == (elem_t*) NOT_ALLOC_PTR && 
        stack->capacity == 0 && stack->size_data == 0)
    {
            return 0;
    }

    if (stack->data == (elem_t*) NOT_ALLOC_PTR && 
        (stack->size_data != 0))
    {
        Log_report ("The user tried to use a pointer which has't yet been allocateв\n");
        return BAD_DATA_PTR;
    }

    return 0;
}

//=======================================================================================================

static uint64_t Stack_check_ (Stack *stack, FILE *fp_logs)
{
    assert (stack != nullptr && "stack is nullptr");   

    uint64_t err_code = 0;

    if (Check_stack_data_ptr (stack)) err_code |= BAD_DATA_PTR;

    if (stack->size_data < 0) err_code |= SIZE_LOWER_ZERO;
    if (stack->capacity  < 0) err_code |= CAPACITY_LOWER_ZERO;

    if (stack->capacity < stack->size_data) err_code |= CAPACITY_LOWER_SIZE;
    
    #ifdef CANARY_PROTECT
        if (stack->canary_val_begin != CANARY_VAL || stack->canary_val_end != CANARY_VAL)
            err_code |= CANARY_CURUPTED;
    #endif

    #ifdef HASH
        if (!(err_code & CAPACITY_LOWER_ZERO))
            if (Check_hash_data (stack))   err_code |= HASH_DATA_CURUPTED;

        if (Check_hash_struct (stack)) err_code |= HASH_STRUCT_CURUPTED;
    #endif

    if (!err_code) Stack_dump_jr (stack);

    return err_code;    
}

//=======================================================================================================

int Stack_dump_jr_ (Stack *stack, FILE *fp_logs)
{
    assert (stack != nullptr && "stack is nullptr");

    fprintf (fp_logs, "=================================================\n\n");

    fprintf (fp_logs, "REFERENCE:\n");

    fprintf (fp_logs, "Check_stack did't detect any errors in the stack structure");

    fprintf (fp_logs, "Stack pointer to data is |%p|\n\n", (char*) stack->data);

    fprintf (fp_logs, "Stack size_data = %ld\n", stack->size_data);
    fprintf (fp_logs, "Stack capacity  = %ld\n",  stack->capacity);

    #ifdef CANARY_PROTECT
        fprintf (fp_logs, "Stack canary vall begin = 0x%lluX\n", stack->canary_val_begin);
        fprintf (fp_logs, "Stack canary vall end   = 0x%lluX\n", stack->canary_val_end);
    #endif

    #ifdef HASH 
        fprintf (fp_logs, "Stack hash data     = %llu\n", stack->hash_data);
        fprintf (fp_logs, "Stack hash struct   = %llu\n", stack->canary_val_end);
    #endif

    return 0;
}

//=======================================================================================================

