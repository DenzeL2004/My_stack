#ifndef _LOG_INFO_H_
#define _LOG_INFO_H_

#include "stack.h"
#include "config.h"

#define LOG_ARGS                                            \
    __FILE__, __PRETTY_FUNCTION__, __LINE__

#define LOG_PARAMETS                                        \
    const char *file_name, const char *func_name, int line

#ifdef USE_LOG

    #define Print_err(stack, stack_err, err)                            \
            Print_error_ (stack, stack_err, err, LOG_ARGS)

#else

     #define Print_err(stack, stack_err, err)   ;

#endif

int Open_logs_file  ();

int Close_logs_file ();

int Print_error_ (Stack *stack, Stack_err stack_errors, int err, LOG_PARAMETS);

static int Short_logs_ (int err, LOG_PARAMETS);

static int Long_logs_  (Stack *stack, Stack_err stack_errors, LOG_PARAMETS);

#endif