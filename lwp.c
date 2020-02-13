#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "lwp.h"

int proc_IDs = 0;
thread proc_list= NULL;

tid_t lwp_create(lwpfun func, void * arg, size_t stack_size)
{
    thread new = malloc(sizeof(struct threadinfo_st));
    new->tid = ++proc_IDs;
    new->stack = malloc(stack_size*sizeof(long));
    new->stacksize = (int)stack_size;
    new->state->rdx;
    if (arg != NULL)
    {

    }

}

void  lwp_exit(void);
{

}

tid_t lwp_gettid(void)
{

}

void  lwp_yield(void)
{

}
void  lwp_start(void)
{

}
void  lwp_stop(void)
{

}


