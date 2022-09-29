#include <stdio.h>


#include "stack.h"
#include "stack_log_errors.h"
#include "log_errors.h"

const int N = 50;

int main()
{
   #ifdef USE_LOG
      Open_logs_file ();
      Open_stack_logs_file ();
   #endif
   
   Stack stack = {};

   if (Stack_ctor (&stack, 10))
   {
      Log_report (LOG_ARGS, "Construct Struct failed\n");
      return 1;
   }


   for (int count = 0; count < N; count++) 
   {
      int val = 0;
      scanf("%d", &val);
      if (val == 0) 
      {
         elem_t last_stack_val = POISON_VAL;
         Stack_pop (&stack, &last_stack_val);

         printf ("last stack val = %" PRINT_TYPE "\n", last_stack_val);
      } 
      else
      {
         Stack_push (&stack, val);
         stack.capacity = -1;
      }
   }

   if (Stack_dtor (&stack))
   {
      Log_report (LOG_ARGS, "Destruct Struct failed\n");
      return 1;
   }

   #ifdef USE_LOG
      Close_logs_file ();
      Close_stack_logs_file ();
   #endif
}