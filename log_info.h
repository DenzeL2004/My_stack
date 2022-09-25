#ifndef _LOG_INFO_H_
#define _LOG_INFO_H_


#include "Generals_func\generals.h"
#include "log_def.h"
#include "config.h"
#include "stack.h"

#ifdef USE_LOG

    #define Print_err(stack, stack_err, err)                            \
            Print_error_ (stack, stack_err, err, LOG_ARGS)

#else

     #define Print_err(stack, stack_err, err)   ;

#endif

int Open_logs_file  ();

int Close_logs_file ();

int Print_error_ (Stack *stack, Stack_err stack_errors, int err, LOG_PARAMETS);

const char* Process_error (int error);

#endif