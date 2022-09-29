#ifndef _LOG_INFO_H_
#define _LOG_INFO_H_

#include "log_def.h"
#include "config.h"
#include "stack.h"

#ifdef USE_LOG

    #define Stack_dump(stack)                         \
            Stack_dump_ (stack, LOG_ARGS)

#else  
    
    #define Stack_dump(stack)                   

#endif

int Stack_data_ptr_check (Stack *stack, LOG_PARAMETS);

int Open_stack_logs_file  ();

int Close_stack_logs_file ();

uint64_t Stack_check (Stack *stack);

int Stack_dump_ (Stack *stack, LOG_PARAMETS);

#endif