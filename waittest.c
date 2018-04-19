
#include "user.h"

char*argv[] = {"forktest", 0};

int
factorial(int n)
{
  if (n == 0)
  {
    return 1;
  }

  return n * factorial(n - 1);
}

int
waittest(void)
{
  int n, pid;

  int waitid = fork();
  int status;

  if (waitid < 0)
  {
    printf(1, "Fork error!\n");
    exit(1);
  }
  else if (waitid == 0)
  {
    int fact = factorial(5);
    printf(1, "Child id %d - !10 = %d\n", getpid(), fact);
    printf(1, "Child id %d - running forktest\n", getpid());
    if (exec("forktest", argv) < 0)
    {
      printf(1, "failed to run forktest!\n");
      exit(1);
    }
  }

  for (n = 0; n < 2; n++)
  {
    pid = fork();
    if (pid < 0)
    {
      printf(1, "error!\n");
      exit(1);
    }
    else if (pid == 0)
    {
      printf(1, "Child id %d - will exit with %d\n", getpid(), n);
      exit(n);
    }
    else
    {
      wait(&status);
      printf(1, "Parent - done waiting for subprocess %d, exit status %d\n", getpid(), status);
    }
  }

  if (waitpid(waitid, &status, 0) < 0)
  {
    return -1;
  }

  printf(1, "Parent - done waiting for subprocess %d, exit status %d\n", waitid, status);
  if (status != 0)
  {
    printf(1, "incorrect exit status %d!\n", status);
  }
  return status;
}

int
main(void)
{
  int status = waittest();
  exit(status);
}