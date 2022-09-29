#ifndef _LOG_ERRORS_H_
#define _LOG_ERRORS_H_

#include "log_def.h"
#include "config.h"

#ifdef USE_LOG

    #define Log_report(LOG_VAR, message)                            \
            Log_report_ (LOG_VAR, message)

#else

    #define Log_report(LOG_VAR, message)    

#endif

int Open_logs_file  ();

int Close_logs_file ();

int Log_report_ (LOG_PARAMETS, const char *format, ...);

#endif