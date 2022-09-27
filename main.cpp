#ifndef elem_t
    #include "stack_type.h"
#endif

#include <stdio.h>

#include "stack.h"
#include "log_info.h"

const int N = 50;

int main()
{
   #ifdef USE_LOG
      Open_logs_file ();
   #endif
   
   Stack stack = {};

   Stack_ctor (&stack, 10);

   for (int count = 0; count < N; count++) 
   {
      int val = 0;
      scanf("%d", &val);
      if (val == 0) 
      {
         elem_t last_stack_val = POISON_VALL;
         Stack_pop (&stack, &last_stack_val);

         printf ("last stack val = %" PRINT_TYPE "\n", last_stack_val);
      } 
      else
      {
         Stack_push(&stack, val);
      }
   }

   Stack_dump (&stack);

   Stack_dtor (&stack);

   #ifdef USE_LOG
      Close_logs_file ();
   #endif
}