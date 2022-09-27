#ifndef _LOG_INFO_H_
#define _LOG_INFO_H_

#ifndef elem_t
    #include "stack_type.h"
#endif

#include "log_def.h"
#include "config.h"
#include "stack.h"

#ifdef USE_LOG

    #define Print_err(err)                            \
            Print_error_ (err, LOG_ARGS)

    #define Stack_dump(stack)                         \
            Stack_dump_ (stack, LOG_ARGS)

#else

    #define Print_err(stack, stack_err, err)    ""
    
    #define Stack_dump(stack)                   ""

#endif

 #define Stack_check(stack, err)                            \
    if (Stack_check_ (stack))                               \
    {                                                       \
        Print_err (err);                                    \
        Stack_dump (stack);                                 \
        return err;                                         \
    }

int Open_logs_file  ();

int Close_logs_file ();

int Print_error_ (int err, LOG_PARAMETS);

const char* Process_error (int error);

uint64_t Stack_check_ (Stack *stack);

int Stack_dump_ (Stack *stack, LOG_PARAMETS);

#endif