#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

void ChildProcess();
void ParentProcess();
int *mem_alloc;

void ChildProcess()
{
    int temp;
    sleep(2);
    printf("Child pid: %d.  [Var 'mem_alloc']vaddr: 0x%lx, paddr: 0x%lx, val: %d\n", getpid(), mem_alloc, syscall(314,getpid(),mem_alloc),*mem_alloc);

    temp = *mem_alloc;
    *mem_alloc = 1;
    printf("\n*** Modify variable 'mem_alloc' from %d to %d ***\n\n", temp, *mem_alloc);

    sleep(2);
    printf("Child pid: %d.  [Var 'mem_alloc']vaddr: 0x%lx, paddr: 0x%lx, val: %d\n", getpid(), mem_alloc, syscall(314,getpid(),mem_alloc),*mem_alloc);

}

void ParentProcess()
{
    sleep(1);
    printf("Parent pid: %d. [Var 'mem_alloc']vaddr: 0x%lx, paddr: 0x%lx, val: %d\n", getpid(), mem_alloc, syscall(314,getpid(),mem_alloc),*mem_alloc);

    sleep(3);
    printf("Parent pid: %d. [Var 'mem_alloc']vaddr: 0x%lx, paddr: 0x%lx, val: %d\n", getpid(), mem_alloc, syscall(314,getpid(),mem_alloc),*mem_alloc);

}



int main()
{
    int status;
    pid_t pid;

    mem_alloc = (int*) malloc(sizeof(int));
    *mem_alloc = 1000;

    pid = fork();

    if (pid == 0)
        ChildProcess();
    else
        ParentProcess();

    wait(&status);
    return 0;
}
