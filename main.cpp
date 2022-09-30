#include <stdio.h>


#include "stack.h"
#include "log_errors.h"

const int N = 50;

int main()
{
   FILE *fp_logs = stdout;
   #ifdef USE_LOG
      fp_logs = Open_logs_file ();
   #endif
   
   Stack stack = {};

   if (Stack_ctor (&stack, 10))
   {
      Log_report ("Construct Struct failed\n");

      #ifdef USE_LOG
         Close_logs_file (fp_logs);
      #endif

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
      }
   }

   if (Stack_dtor (&stack))
   {
      Log_report ("Destruct Struct failed\n");

      #ifdef USE_LOG
         Close_logs_file (fp_logs);
      #endif

      return 1;
   }

   #ifdef USE_LOG
      Close_logs_file (fp_logs);
   #endif
}