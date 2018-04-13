
#include "types.h"
#include "user.h"

char *argv[] = { "forktest", 0 };

int
waitpidtest(void)
{
    int n, pid;

    int waitid = fork();
    int status;

    if (waitid < 0) {
        printf(1, "error!\n");
        exit(1);
    }
    else if (waitid == 0)
    {
        exec("forktest", argv);
        exit(0);
    }

    for(n=0; n<2; n++){
        pid = fork();
        if(pid < 0)
            break;
        if(pid == 0)
            exit(1);
        wait(NULL);
    }

    if (waitpid(waitid, &status, 0) < 0) {
        return -1;
    }
    printf(1, "done waiting\n");
    return (status < 0 ? 1 : status);
}


int
main(void)
{
    int status = waitpidtest();
    exit(status);
}