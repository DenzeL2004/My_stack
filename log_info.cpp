#include <stdio.h>
#include <time.h>

#include "log_info.h"
#include "Generals_func\generals.h"

static FILE *fp_logs = nullptr;

static char Can_print_stack (uint64_t err_code);

//=======================================================================================================

int Open_logs_file ()
{    
    fp_logs = Open_file_ptr ("logs_info.txt", "a");

    time_t seconds = time (NULL)  + 3 * 60* 60;;   

    fprintf (fp_logs, "-------------Time open logs file: %s\n\n", 
                    asctime(gmtime(&seconds))); 
    
    if (!fp_logs)
    {
        fprintf (stderr, "Logs file does not open\n");
        return ERR_FILE_OPEN;
    }

    return 0;
}

//=======================================================================================================

int Print_error_ (int err, LOG_PARAMETS) 
{    
    time_t seconds = time (NULL) + 3 * 60* 60;

    fprintf (fp_logs, "=================================================\n\n");

    fprintf (fp_logs, "SHORT REFERENCE:\n");

    fprintf (fp_logs, "WTF!!!\n");
    fprintf (fp_logs, "Something went wrong the way you wanted\n\n");

    fprintf (fp_logs, "Error time: %s\n", asctime(gmtime(&seconds)));
    //

    fprintf (fp_logs, "In file %s\n", file_name);
    fprintf (fp_logs, "In function %s\n", func_name);
    fprintf (fp_logs, "In line %d\n\n", line);

    fprintf (fp_logs, "Function returned an error value: %d\n", err);
    fprintf (fp_logs, "Error value: %s\n", Process_error (err));
    fprintf (fp_logs, "=================================================\n\n\n");

    return 0;                                                       
}

//=======================================================================================================

const char* Process_error (int error)
{
    switch (error){
    
        case STACK_CTOR_ERR:
            return "Failure to create a structure Stack\n";
            break;

        case INIT_STACK_VALLS_ERR:
            return "Stack initialization failure\n";
            break;

        case REALLOC_STACK_ERR:
            return "Stack realloc failure\n";
            break;
        
        case INCREASE_STACK_ERR:
            return  "Failure stack increase\n";
            break;

        case DECREASE_STACK_ERR:
            return  "Failure stack decrease\n";
            break;

        case STACK_PUSH_ERR:
            return "Push stack command failed\n";
            break;

        case STACK_POP_ERR:
            return "Pop stack command failed\n";
            break;

        case STACK_DTOR_ERR:
            return "Failure to deconstruct a structure Stack";

        case STACK_SAVE_HASH_ERR:
            return "Failure to save hash to struct Stack";

        default:
            return "ACHTUNG!!! OMG!!! WTF!!! Unknown error\n";
            break;
    }       

    return "ACHTUNG!!! OMG!!! WTF!!! Unknown error\n";
}

//=======================================================================================================

int Stack_dump_ (Stack *stack, LOG_PARAMETS)
{
    uint64_t err_code = Stack_check_ (stack);
    if (!err_code) return 0;

    time_t seconds = time (NULL) + 3 * 60* 60;

    fprintf (fp_logs, "=================================================\n\n");

    fprintf (fp_logs, "REFERENCE:\n");

    fprintf (fp_logs, "Stack variable created in file %s in function %s line %d.\n", 
            stack->stack_info.origin_file,  stack->stack_info.origin_func, stack->stack_info.origin_line);
                       
    fprintf (fp_logs, "Caused an error in file %s, function %s, line %d\n\n", LOG_VAR);

    fprintf (fp_logs, "Error time: %s\n\n", asctime(gmtime(&seconds)));
    
    fprintf (fp_logs, "ERR CODE: ");
    Bin_represent (fp_logs, err_code, sizeof (err_code));
    fprintf (fp_logs, "\n");

    fprintf (fp_logs, "Stack pointer to data is |%p|\n\n", (char*) stack->data);

    fprintf (fp_logs, "Stack size_data = %ld\n", stack->size_data);
    fprintf (fp_logs, "Stack capacity = %ld\n",  stack->capacity);

    #ifdef CANARY_PROTECT
        fprintf (fp_logs, "Stack canary vall begin = 0x%lluX\n", stack->canary_vall_begin);
        fprintf (fp_logs, "Stack canary vall end   = 0x%lluX\n", stack->canary_vall_end);
    #endif

    #ifdef HASH 
        fprintf (fp_logs, "Stack hash data = %llu\n", stack->hash_data);
        fprintf (fp_logs, "Stack hash struct   = %llu\n", stack->canary_vall_end);
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

    if (Can_print_stack (err_code))
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

uint64_t Stack_check_ (Stack *stack)
{
    uint64_t err_code = 0;

    if (stack->data == nullptr)              err_code |= DATA_IS_NULLPTR;
    if (stack->data == (elem_t*) POISON_PTR) err_code |= DATA_IS_POISON;
    
    if (stack->size_data < 0) err_code |= SIZE_LOWER_ZERO;
    if (stack->capacity  < 0) err_code |= CAPACITY_LOWER_ZERO;

    if (stack->capacity < stack->size_data) err_code |= CAPACITY_LOWER_SIZE;
    
    #ifdef CANARY_PROTECT
        if (stack->canary_vall_begin != CANARY_VALL || stack->canary_vall_end != CANARY_VALL)
            err_code |= CANARY_CURUPTED;
    #endif

    #ifdef HASH
        if (Check_hash_data (stack))   err_code |= HASH_DATA_CURUPTED;
        if (Check_hash_struct (stack)) err_code |= HASH_STRUCT_CURUPTED;
    #endif

    return err_code;    
}

//=======================================================================================================

static char Can_print_stack (uint64_t err_code)
{
    return !((err_code & DATA_IS_NULLPTR) || (err_code & DATA_IS_POISON) || 
             (err_code & SIZE_LOWER_ZERO) || (err_code & CAPACITY_LOWER_ZERO));
}

//=======================================================================================================

int Close_logs_file ()
{
    time_t seconds = time (NULL)  + 3 * 60* 60;;   

    fprintf (fp_logs, "-------------Time close logs file: %s\n\n", 
                    asctime(gmtime(&seconds))); 

    if (Close_file_ptr (fp_logs))
    {
        fprintf (stderr, "Logs file does not close\n");
        return ERR_FILE_OPEN;
    }

    return 0;
}