#include "types.h"
#include "stat.h"
#include "user.h"

long long
stack_fault(long long n)
{
  long long j = n - 1;

  if (n == 0)
  {
    return 0;
  }

  j = stack_fault(j);
  return 0;
}

int main(int argc, char *argv[])
{
   int pid=0;

   pid=fork();

   if(pid==0){
       printf(1, "stack_fault() returned %d\n", stack_fault(1000));
       exit();
   }

   wait();
   exit();
}
