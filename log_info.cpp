#include <stdio.h>
#include <time.h>

#include "log_info.h"
#include "Generals_func\generals.h"

static int Short_logs_ (int err, LOG_PARAMETS);

static int Long_logs_  (Stack *stack, Stack_err stack_errors, LOG_PARAMETS);

FILE *fp_logs = nullptr;

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

int Print_error_ (Stack *stack, Stack_err stack_errors, int err, LOG_PARAMETS)
{
    Short_logs_ (err, LOG_VAR);
    Long_logs_  (stack, stack_errors, LOG_VAR);

    return 0;
}

//=======================================================================================================

static int Long_logs_ (Stack *stack, Stack_err stack_errors, LOG_PARAMETS)
{
    time_t seconds = time (NULL) + 3 * 60* 60;

    fprintf (fp_logs, "=================================================\n\n");

    fprintf (fp_logs, "REFERENCE:\n");

    fprintf (fp_logs, "struct variable created in file %s in function %s line %d.\n", 
            stack->stack_info.origin_file,  stack->stack_info.origin_func, stack->stack_info.origin_line);
                       
    fprintf (fp_logs, "Caused an error in file %s, function %s, line %d\n\n", 
                LOG_VAR);

    fprintf (fp_logs, "Error time: %s\n\n", asctime(gmtime(&seconds)));

    Stack_dump (fp_logs, stack, &stack_errors);
    
    fprintf (fp_logs, "=================================================\n\n\n");

    return 0;
}

//=======================================================================================================

static int Short_logs_ (int err, LOG_PARAMETS) 
{    
    time_t seconds = time (NULL) + 3 * 60* 60;

    fprintf (fp_logs, "=================================================\n\n");

    fprintf (fp_logs, "SHORT REFERENCE:\n");

    fprintf (fp_logs, "WTF!!!\n");
    fprintf (fp_logs, "Something went wrong the way you wanted\n\n");

    fprintf (fp_logs, "Error time: %s\n", asctime(gmtime(&seconds)));
    
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

        default:
            return "ACHTUNG!!! OMG!!! WTF!!! Unknown error\n";
            break;
    }       

    return "ACHTUNG!!! OMG!!! WTF!!! Unknown error\n";
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