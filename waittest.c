#include "types.h"
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

  printf(1, "####################################################\n");
  printf(1, "#       Part1: testing wait and exit status        #\n");
  printf(1, "####################################################\n");

  pid = fork();
  int status;

  if (pid < 0)
  {
    printf(1, "Fork error!\n");
    exit(1);
  }
  else if (pid == 0)
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
      printf(1, "Fork error!\n");
      exit(1);
    }
    else if (pid == 0)
    {
      printf(1, "Child id %d - will exit with %d\n", getpid(), n);
      exit(n);
    }
    else
    {
      pid = wait(&status);
      printf(1, "Parent - done waiting for subprocess %d, exit status %d\n", pid, status);
    }
  }

  pid = wait(&status);
  printf(1, "Parent - done waiting for subprocess %d, exit status %d\n", pid, status);
  if (status != 0)
  {
    printf(1, "Incorrect exit status %d for process %d!\n", status, pid);
  }

  printf(1, "\n\n");
  printf(1, "####################################################\n");
  printf(1, "#              Part2: testing waitpid              #\n");
  printf(1, "####################################################\n");

  int pids[] = {0, 0, 0, 0};
  int waitorder[] = {3, 1, 2, 0};

  for (int i = 0; i < 4; ++i)
  {
    pids[i] = fork();

    if (pids[i] == 0)
    {
      printf(1, "Child id %d - will exit with %d\n", getpid(), i);
      exit(i);
    }
  }

  for (int i = 0; i < 4; ++i)
  {
    sleep(5);
    printf(1, "Parent - waiting for process id %d\n", pids[waitorder[i]]);
    pid = waitpid(pids[waitorder[i]], &status, 0);
    if (pid != -1)
      printf(1, "Parent - process id %d exited with status %d\n", pid, status);
    else
      printf(1, "Parent - no process found or already exited\n", pid, status);

  }

  return 0;
}

int
main(void)
{
  int status = waittest();
  exit(status);
}