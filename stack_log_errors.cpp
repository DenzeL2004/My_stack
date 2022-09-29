#include <stdio.h>
#include <stdlib.h>

#include "stack_log_errors.h"
#include "log_errors.h"
#include "stack.h"
#include "config.h"
#include "Generals_func\generals.h"

static FILE *fp_logs = nullptr;

//=======================================================================================================

int Open_stack_logs_file ()
{   
    fp_logs = Open_file_ptr ("logs_info.txt", "a");

    if (!fp_logs)
    {
        fprintf (stderr, "Logs file does not open\n");
        return ERR_FILE_OPEN;
    }

    return 0;
}

//=======================================================================================================

int Stack_data_ptr_check (Stack *stack, 
                          const char* file_name, const char* func_name, int line)
{
    if (stack->data == nullptr)
    {
        Stack_dump (stack);
        return 1;
    }

    if (stack->data == (elem_t*) POISON_PTR)
    {
        Log_report (LOG_VAR, "The user tried to use a pointer with a poisonous value\n");
        return 1;
    }

    return 0;
}

//=======================================================================================================

int Stack_dump_ (Stack *stack, const char* file_name, 
                                const char* func_name, int line)
{
    uint64_t err_code = Stack_check (stack);
    if (!err_code) return 0;

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

    if (err_code & DATA_IS_NULLPTR) fprintf (fp_logs, "stack pointer data is nullptr.\n");
    if (err_code & DATA_IS_POISON ) fprintf (fp_logs, "stack pointer data is poison vallue.\n");

    fprintf (fp_logs, "\n");

    if (err_code & SIZE_LOWER_ZERO)      fprintf (fp_logs, "stack size_data is a negative number.\n");
    if (err_code & CAPACITY_LOWER_ZERO)  fprintf (fp_logs, "stack capacity is a negative number.\n");
    if (err_code & CAPACITY_LOWER_SIZE)  fprintf (fp_logs, "stack capacity is lower size_data:\n");

    fprintf (fp_logs, "\n");
    
    #ifdef CANARY_PROTECT
        if (err_code & CANARY_CURUPTED) fprintf (fp_logs, "stack canary_begin is currupted.\n");
    #endif

    if (!((err_code & DATA_IS_NULLPTR) || (err_code & DATA_IS_POISON) || 
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

uint64_t Stack_check (Stack *stack)
{
    uint64_t err_code = 0;

    if (stack->data == nullptr)              err_code |= DATA_IS_NULLPTR;
    if (stack->data == (elem_t*) POISON_PTR) err_code |= DATA_IS_POISON;
    
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

    return err_code;    
}

//=======================================================================================================


int Close_stack_logs_file ()
{ 
    if (Close_file_ptr (fp_logs))
    {
        fprintf (stderr, "Logs file does not close\n");
        return ERR_FILE_OPEN;
    }

    return 0;
}

//=======================================================================================================
